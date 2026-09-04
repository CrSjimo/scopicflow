#include "DynamicMixingEditorQuickItem_p.h"
#include "DynamicMixingEditorQuickItem_p_p.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <numbers>
#include <utility>

#include <QGuiApplication>
#include <QPainter>
#include <QPainterPath>
#include <QQuickWindow>
#include <QSet>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSGRendererInterface>
#include <QStyleHints>

#include <SVSCraftQuick/SoftwarePainterNode.h>

#include <ScopicFlowCore/DynamicMixingAnchorViewModel.h>
#include <ScopicFlowCore/DynamicMixingViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>

namespace sflow {

    namespace {

        constexpr double lineWidth = 1.5;
        constexpr double selectedLineWidth = 2.0;
        constexpr double handleRadius = 4.0;
        constexpr double selectedHandleRadius = 6.0;
        constexpr int circleSegments = 12;

        enum RootNodeIndex {
            FillContainerNode,
            BoundaryNode,
            AnchorLineNode,
            SelectedAnchorLineNode,
            SelectedHandleBorderNode,
            HandleNode,
            RootNodeCount,
        };

        class DynamicMixingHardwareNode : public QSGNode {
        };

        QPainterPath trianglePath(const QVector<QPointF> &points) {
            QPainterPath path;
            path.setFillRule(Qt::WindingFill);
            for (qsizetype i = 0; i + 2 < points.size(); i += 3) {
                path.moveTo(points.at(i));
                path.lineTo(points.at(i + 1));
                path.lineTo(points.at(i + 2));
                path.closeSubpath();
            }
            return path;
        }

        class DynamicMixingSoftwareNode : public SVS::SoftwarePainterNode {
        public:
            explicit DynamicMixingSoftwareNode(QQuickItem *item) : SoftwarePainterNode(item) {
            }

            void synchronize(const DynamicMixingEditorGeometrySnapshot &snapshot,
                             quint64 revision,
                             const QList<QColor> &colors,
                             const QColor &boundaryLineColor,
                             const QColor &anchorLineColor,
                             const QColor &selectedAnchorLineColor,
                             const QColor &selectedHandleBorderColor,
                             const QColor &handleColor) {
                if (m_revision != revision) {
                    m_fillPaths.clear();
                    m_fillPaths.reserve(snapshot.fills.size());
                    for (const auto &fill : snapshot.fills) {
                        m_fillPaths.append(trianglePath(fill));
                    }
                    m_boundaryPath = trianglePath(snapshot.boundaries);
                    m_anchorLinePath = trianglePath(snapshot.anchorLines);
                    m_selectedAnchorLinePath = trianglePath(snapshot.selectedAnchorLines);
                    m_selectedHandleBorderPath = trianglePath(snapshot.selectedHandleBorders);
                    m_handlePath = trianglePath(snapshot.handles);
                    QRectF bounds;
                    for (const auto &path : std::as_const(m_fillPaths)) {
                        bounds |= path.boundingRect();
                    }
                    bounds |= m_boundaryPath.boundingRect();
                    bounds |= m_anchorLinePath.boundingRect();
                    bounds |= m_selectedAnchorLinePath.boundingRect();
                    bounds |= m_selectedHandleBorderPath.boundingRect();
                    bounds |= m_handlePath.boundingRect();
                    if (!bounds.isEmpty()) {
                        bounds.adjust(-0.5, -0.5, 0.5, 0.5);
                    }
                    setBoundingRect(bounds);
                    m_revision = revision;
                    markDirty(QSGNode::DirtyGeometry);
                }
                if (m_colors != colors
                    || m_boundaryLineColor != boundaryLineColor
                    || m_anchorLineColor != anchorLineColor
                    || m_selectedAnchorLineColor != selectedAnchorLineColor
                    || m_selectedHandleBorderColor != selectedHandleBorderColor
                    || m_handleColor != handleColor) {
                    m_colors = colors;
                    m_boundaryLineColor = boundaryLineColor;
                    m_anchorLineColor = anchorLineColor;
                    m_selectedAnchorLineColor = selectedAnchorLineColor;
                    m_selectedHandleBorderColor = selectedHandleBorderColor;
                    m_handleColor = handleColor;
                    markDirty(QSGNode::DirtyMaterial);
                }
            }

        protected:
            void paint(QPainter *painter) override {
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(Qt::NoPen);
                for (qsizetype i = 0; i < m_fillPaths.size() && i < m_colors.size(); ++i) {
                    painter->fillPath(m_fillPaths.at(i), m_colors.at(i));
                }
                painter->fillPath(m_boundaryPath, m_boundaryLineColor);
                painter->fillPath(m_anchorLinePath, m_anchorLineColor);
                painter->fillPath(m_selectedAnchorLinePath, m_selectedAnchorLineColor);
                painter->fillPath(m_selectedHandleBorderPath, m_selectedHandleBorderColor);
                painter->fillPath(m_handlePath, m_handleColor);
            }

        private:
            QVector<QPainterPath> m_fillPaths;
            QPainterPath m_boundaryPath;
            QPainterPath m_anchorLinePath;
            QPainterPath m_selectedAnchorLinePath;
            QPainterPath m_selectedHandleBorderPath;
            QPainterPath m_handlePath;
            QList<QColor> m_colors;
            QColor m_boundaryLineColor;
            QColor m_anchorLineColor;
            QColor m_selectedAnchorLineColor;
            QColor m_selectedHandleBorderColor;
            QColor m_handleColor;
            quint64 m_revision = std::numeric_limits<quint64>::max();
        };

        bool usesSoftwareRenderer(const QQuickItem *item) {
            return item->window()
                && item->window()->rendererInterface()->graphicsApi() == QSGRendererInterface::Software;
        }

        int boundedFloorPosition(double position) {
            if (!std::isfinite(position) || position <= 0.0) {
                return 0;
            }
            if (position >= std::numeric_limits<int>::max()) {
                return std::numeric_limits<int>::max();
            }
            return static_cast<int>(std::floor(position));
        }

        int boundedCeilPosition(double position) {
            if (!std::isfinite(position) || position <= 0.0) {
                return 0;
            }
            if (position >= std::numeric_limits<int>::max()) {
                return std::numeric_limits<int>::max();
            }
            return static_cast<int>(std::ceil(position));
        }

        QObjectList effectiveSliceInclusive(const DynamicMixingViewModel *viewModel,
                                            int first,
                                            int last) {
            const qint64 length = static_cast<qint64>(last) - first + 1;
            if (length <= std::numeric_limits<int>::max()) {
                return viewModel->effectiveSlice(first, static_cast<int>(length));
            }
            auto result = viewModel->effectiveSlice(first, std::numeric_limits<int>::max());
            if (auto *lastItem = viewModel->itemAtPosition(last);
                lastItem && !result.contains(lastItem)) {
                result.append(lastItem);
            }
            return result;
        }

        void appendTriangle(QVector<QPointF> &geometry,
                            const QPointF &a,
                            const QPointF &b,
                            const QPointF &c) {
            geometry.append(a);
            geometry.append(b);
            geometry.append(c);
        }

        void appendLineQuad(QVector<QPointF> &geometry,
                            const QPointF &a,
                            const QPointF &b,
                            double width) {
            const QPointF direction = b - a;
            const double length = std::hypot(direction.x(), direction.y());
            if (qFuzzyIsNull(length)) {
                return;
            }
            const QPointF normal(-direction.y() * width * 0.5 / length,
                                 direction.x() * width * 0.5 / length);
            const QPointF a1 = a + normal;
            const QPointF a2 = a - normal;
            const QPointF b1 = b + normal;
            const QPointF b2 = b - normal;
            appendTriangle(geometry, a1, a2, b1);
            appendTriangle(geometry, b1, a2, b2);
        }

        void appendBandQuad(QVector<QPointF> &geometry,
                            const QPointF &topLeft,
                            const QPointF &bottomLeft,
                            const QPointF &topRight,
                            const QPointF &bottomRight) {
            appendTriangle(geometry, topLeft, bottomLeft, topRight);
            appendTriangle(geometry, topRight, bottomLeft, bottomRight);
        }

        void appendCircle(QVector<QPointF> &geometry, const QPointF &center, double radius) {
            for (int i = 0; i < circleSegments; ++i) {
                const double angle1 = 2.0 * std::numbers::pi * i / circleSegments;
                const double angle2 = 2.0 * std::numbers::pi * (i + 1) / circleSegments;
                appendTriangle(geometry,
                               center,
                               center + QPointF(std::cos(angle1) * radius,
                                                std::sin(angle1) * radius),
                               center + QPointF(std::cos(angle2) * radius,
                                                std::sin(angle2) * radius));
            }
        }

        QSGGeometryNode *createGeometryNode() {
            auto *node = new QSGGeometryNode;
            auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
            geometry->setDrawingMode(QSGGeometry::DrawTriangles);
            node->setGeometry(geometry);
            node->setFlag(QSGNode::OwnsGeometry);
            auto *material = new QSGFlatColorMaterial;
            node->setMaterial(material);
            node->setFlag(QSGNode::OwnsMaterial);
            return node;
        }

        void updateGeometryNode(QSGGeometryNode *node,
                                const QVector<QPointF> &points,
                                const QColor &color) {
            auto *geometry = node->geometry();
            geometry->allocate(static_cast<int>(points.size()));
            auto *vertices = geometry->vertexDataAsPoint2D();
            for (int i = 0; i < points.size(); ++i) {
                vertices[i].set(static_cast<float>(points.at(i).x()),
                                static_cast<float>(points.at(i).y()));
            }
            auto *material = static_cast<QSGFlatColorMaterial *>(node->material());
            material->setColor(color);
            node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        }

        void ensureGeometryNodeCount(QSGNode *container, int count) {
            while (container->childCount() < count) {
                container->appendChildNode(createGeometryNode());
            }
            while (container->childCount() > count) {
                auto *node = container->lastChild();
                container->removeChildNode(node);
                delete node;
            }
        }

        QList<double> boundedRatio(QList<double> ratio) {
            double sum = 0.0;
            for (double &value : ratio) {
                value = std::clamp(value, 0.0, 1.0);
                sum += value;
            }
            double excess = std::max(0.0, sum - 1.0);
            for (qsizetype i = ratio.size(); i > 0 && excess > 0.0; --i) {
                const double correction = std::min(ratio.at(i - 1), excess);
                ratio[i - 1] -= correction;
                excess -= correction;
            }
            return ratio;
        }

    }

    void DynamicMixingEditorQuickItemPrivate::invalidate() {
        Q_Q(DynamicMixingEditorQuickItem);
        q->polish();
        q->update();
    }

    void DynamicMixingEditorQuickItemPrivate::reconnectModels() {
        Q_Q(DynamicMixingEditorQuickItem);
        const QList<QObject *> objects {
            dynamicMixingViewModel.data(),
            timeViewModel.data(),
            timeLayoutViewModel.data(),
        };
        for (auto *object : objects) {
            if (object) {
                QObject::disconnect(object, nullptr, q, nullptr);
                QObject::connect(object, &QObject::destroyed, q, [this] { invalidate(); });
            }
        }
        if (dynamicMixingViewModel) {
            QObject::connect(dynamicMixingViewModel, &DynamicMixingViewModel::voiceCountChanged,
                             q, [this] {
                                 clearMoveState();
                                 invalidate();
                             });
            QObject::connect(dynamicMixingViewModel, &DynamicMixingViewModel::itemsChanged,
                             q, [this] { invalidate(); });
            QObject::connect(dynamicMixingViewModel, &DynamicMixingViewModel::itemUpdated,
                             q, [this] { invalidate(); });
            QObject::connect(dynamicMixingViewModel, &DynamicMixingViewModel::movingItemChanged,
                             q, [this] { invalidate(); });
        }
        if (timeViewModel) {
            QObject::connect(timeViewModel, &TimeViewModel::startChanged,
                             q, [this] { invalidate(); });
            QObject::connect(timeViewModel, &TimeViewModel::endChanged,
                             q, [this] { invalidate(); });
        }
        if (timeLayoutViewModel) {
            QObject::connect(timeLayoutViewModel, &TimeLayoutViewModel::pixelDensityChanged,
                             q, [this] { invalidate(); });
        }
    }

    double DynamicMixingEditorQuickItemPrivate::positionForX(double x) const {
        if (!timeViewModel || !timeLayoutViewModel
            || timeLayoutViewModel->pixelDensity() <= 0.0) {
            return 0.0;
        }
        return timeViewModel->start() + x / timeLayoutViewModel->pixelDensity();
    }

    double DynamicMixingEditorQuickItemPrivate::xForPosition(double position) const {
        if (!timeViewModel || !timeLayoutViewModel) {
            return 0.0;
        }
        return (position - timeViewModel->start()) * timeLayoutViewModel->pixelDensity();
    }

    QVector<double> DynamicMixingEditorQuickItemPrivate::cumulativeRatio(
        const QList<double> &ratio) const {
        QVector<double> result;
        result.reserve(ratio.size());
        double sum = 0.0;
        for (const double value : ratio) {
            sum = std::clamp(sum + value, 0.0, 1.0);
            result.append(sum);
        }
        return result;
    }

    DynamicMixingEditorQuickItemPrivate::HitResult DynamicMixingEditorQuickItemPrivate::hitTest(
        const QPointF &point,
        double radius) const {
        HitResult result;
        if (!dynamicMixingViewModel || radius < 0.0 || !timeLayoutViewModel
            || timeLayoutViewModel->pixelDensity() <= 0.0) {
            return result;
        }

        const double leftPosition = std::max(0.0, positionForX(point.x() - radius));
        const double rightPosition = std::max(leftPosition, positionForX(point.x() + radius));
        if (leftPosition > std::numeric_limits<int>::max()) {
            return result;
        }
        const int first = boundedFloorPosition(leftPosition);
        const int last = boundedCeilPosition(rightPosition);
        const auto objects = effectiveSliceInclusive(dynamicMixingViewModel, first, last);

        double nearestHandleDistance = radius;
        for (auto *object : objects) {
            auto *item = qobject_cast<DynamicMixingAnchorViewModel *>(object);
            if (!item) {
                continue;
            }
            const double x = xForPosition(item->position());
            const auto cumulative = cumulativeRatio(dynamicMixingViewModel->effectiveRatio(item));
            for (int handleIndex = 0; handleIndex < cumulative.size(); ++handleIndex) {
                const double y = cumulative.at(handleIndex) * q_func()->height();
                const double distance = std::hypot(x - point.x(), y - point.y());
                if (distance <= nearestHandleDistance) {
                    nearestHandleDistance = distance;
                    result.anchor = item;
                    result.handleIndex = handleIndex;
                }
            }
        }
        if (result.anchor) {
            return result;
        }

        double nearestLineDistance = radius;
        for (auto *object : objects) {
            auto *item = qobject_cast<DynamicMixingAnchorViewModel *>(object);
            if (!item) {
                continue;
            }
            const double distance = std::abs(xForPosition(item->position()) - point.x());
            if (distance <= nearestLineDistance) {
                nearestLineDistance = distance;
                result.anchor = item;
                result.handleIndex = -1;
            }
        }
        return result;
    }

    void DynamicMixingEditorQuickItemPrivate::clearMoveState() {
        movingAnchors.clear();
        primaryMovingAnchor = nullptr;
        anchorMoveStartPoint = {};
        primaryAnchorStartPosition = 0;
        movingHandleIndex = -1;
        movingVoiceCount = 1;
        moveAxis = PendingAxis;
    }

    void DynamicMixingEditorQuickItemPrivate::rebuildGeometry() {
        Q_Q(DynamicMixingEditorQuickItem);
        snapshot = {};
        ++snapshotRevision;
        if (!dynamicMixingViewModel || !timeViewModel || !timeLayoutViewModel
            || timeLayoutViewModel->pixelDensity() <= 0.0 || q->width() <= 0.0
            || q->height() <= 0.0 || dynamicMixingViewModel->voiceCount() < 1) {
            return;
        }

        const int voiceCount = dynamicMixingViewModel->voiceCount();
        const int colorGroupCount = std::min(static_cast<int>(colors.size()), voiceCount);
        snapshot.fills.resize(colorGroupCount);
        const double visibleStart = positionForX(0.0);
        const double visibleEnd = positionForX(q->width());
        const int firstPosition = boundedFloorPosition(std::max(0.0, visibleStart));
        const int lastPosition = boundedCeilPosition(std::max(0.0, visibleEnd));
        const auto objects = effectiveSliceInclusive(dynamicMixingViewModel,
                                                     firstPosition,
                                                     lastPosition);

        struct Sample {
            double x = 0.0;
            QList<double> ratio;
        };
        QVector<Sample> samples;
        samples.reserve(objects.size() + 2);
        const double leftSamplePosition = std::clamp(visibleStart, 0.0,
                                                      static_cast<double>(std::numeric_limits<int>::max()));
        const double rightSamplePosition = std::clamp(visibleEnd, 0.0,
                                                       static_cast<double>(std::numeric_limits<int>::max()));
        samples.append({0.0, dynamicMixingViewModel->ratioAt(leftSamplePosition)});
        for (auto *object : objects) {
            auto *item = qobject_cast<DynamicMixingAnchorViewModel *>(object);
            if (!item) {
                continue;
            }
            const double x = xForPosition(item->position());
            if (x >= 0.0 && x <= q->width()) {
                samples.append({x, dynamicMixingViewModel->effectiveRatio(item)});
            }
        }
        samples.append({q->width(), dynamicMixingViewModel->ratioAt(rightSamplePosition)});

        for (qsizetype sampleIndex = 1; sampleIndex < samples.size(); ++sampleIndex) {
            const auto &left = samples.at(sampleIndex - 1);
            const auto &right = samples.at(sampleIndex);
            const auto leftCumulative = cumulativeRatio(left.ratio);
            const auto rightCumulative = cumulativeRatio(right.ratio);
            for (int voiceIndex = 0; voiceIndex < voiceCount; ++voiceIndex) {
                if (colorGroupCount == 0) {
                    break;
                }
                const double leftTop = voiceIndex == 0
                    ? 0.0 : leftCumulative.at(voiceIndex - 1) * q->height();
                const double rightTop = voiceIndex == 0
                    ? 0.0 : rightCumulative.at(voiceIndex - 1) * q->height();
                const double leftBottom = voiceIndex == voiceCount - 1
                    ? q->height() : leftCumulative.at(voiceIndex) * q->height();
                const double rightBottom = voiceIndex == voiceCount - 1
                    ? q->height() : rightCumulative.at(voiceIndex) * q->height();
                appendBandQuad(snapshot.fills[voiceIndex % colorGroupCount],
                               QPointF(left.x, leftTop),
                               QPointF(left.x, leftBottom),
                               QPointF(right.x, rightTop),
                               QPointF(right.x, rightBottom));
            }
            for (int boundaryIndex = 0; boundaryIndex < voiceCount - 1; ++boundaryIndex) {
                appendLineQuad(snapshot.boundaries,
                               QPointF(left.x, leftCumulative.at(boundaryIndex) * q->height()),
                               QPointF(right.x, rightCumulative.at(boundaryIndex) * q->height()),
                               lineWidth);
            }
        }

        for (auto *object : objects) {
            auto *item = qobject_cast<DynamicMixingAnchorViewModel *>(object);
            if (!item) {
                continue;
            }
            const double x = xForPosition(item->position());
            if (x < 0.0 || x > q->width()) {
                continue;
            }
            auto &lineGeometry = item->isSelected()
                ? snapshot.selectedAnchorLines : snapshot.anchorLines;
            appendLineQuad(lineGeometry, QPointF(x, 0.0), QPointF(x, q->height()),
                           item->isSelected() ? selectedLineWidth : lineWidth);
            const auto cumulative = cumulativeRatio(dynamicMixingViewModel->effectiveRatio(item));
            for (const double value : cumulative) {
                const QPointF center(x, value * q->height());
                if (item->isSelected()) {
                    appendCircle(snapshot.selectedHandleBorders, center, selectedHandleRadius);
                }
                appendCircle(snapshot.handles, center, handleRadius);
            }
        }
    }

    DynamicMixingEditorQuickItem::DynamicMixingEditorQuickItem(QQuickItem *parent)
        : QQuickItem(parent), d_ptr(new DynamicMixingEditorQuickItemPrivate) {
        Q_D(DynamicMixingEditorQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
        connect(this, &QQuickItem::widthChanged, this, [d] { d->invalidate(); });
        connect(this, &QQuickItem::heightChanged, this, [d] { d->invalidate(); });
    }

    DynamicMixingEditorQuickItem::~DynamicMixingEditorQuickItem() = default;

    DynamicMixingViewModel *DynamicMixingEditorQuickItem::dynamicMixingViewModel() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->dynamicMixingViewModel;
    }

    void DynamicMixingEditorQuickItem::setDynamicMixingViewModel(DynamicMixingViewModel *viewModel) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->dynamicMixingViewModel == viewModel) {
            return;
        }
        if (d->dynamicMixingViewModel && d->primaryMovingAnchor) {
            d->dynamicMixingViewModel->abortInteractiveMove(d->primaryMovingAnchor);
        }
        if (d->dynamicMixingViewModel) {
            QObject::disconnect(d->dynamicMixingViewModel, nullptr, this, nullptr);
        }
        d->dynamicMixingViewModel = viewModel;
        d->clearMoveState();
        d->reconnectModels();
        d->invalidate();
        Q_EMIT dynamicMixingViewModelChanged();
    }

    TimeViewModel *DynamicMixingEditorQuickItem::timeViewModel() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->timeViewModel;
    }

    void DynamicMixingEditorQuickItem::setTimeViewModel(TimeViewModel *viewModel) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->timeViewModel == viewModel) {
            return;
        }
        if (d->timeViewModel) {
            QObject::disconnect(d->timeViewModel, nullptr, this, nullptr);
        }
        d->timeViewModel = viewModel;
        d->reconnectModels();
        d->invalidate();
        Q_EMIT timeViewModelChanged();
    }

    TimeLayoutViewModel *DynamicMixingEditorQuickItem::timeLayoutViewModel() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->timeLayoutViewModel;
    }

    void DynamicMixingEditorQuickItem::setTimeLayoutViewModel(TimeLayoutViewModel *viewModel) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->timeLayoutViewModel == viewModel) {
            return;
        }
        if (d->timeLayoutViewModel) {
            QObject::disconnect(d->timeLayoutViewModel, nullptr, this, nullptr);
        }
        d->timeLayoutViewModel = viewModel;
        d->reconnectModels();
        d->invalidate();
        Q_EMIT timeLayoutViewModelChanged();
    }

    QList<QColor> DynamicMixingEditorQuickItem::colors() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->colors;
    }

    void DynamicMixingEditorQuickItem::setColors(const QList<QColor> &colors) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->colors == colors) {
            return;
        }
        d->colors = colors;
        d->invalidate();
        Q_EMIT colorsChanged();
    }

    QColor DynamicMixingEditorQuickItem::boundaryLineColor() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->boundaryLineColor;
    }

    void DynamicMixingEditorQuickItem::setBoundaryLineColor(const QColor &color) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->boundaryLineColor == color) {
            return;
        }
        d->boundaryLineColor = color;
        update();
        Q_EMIT boundaryLineColorChanged();
    }

    QColor DynamicMixingEditorQuickItem::anchorLineColor() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->anchorLineColor;
    }

    void DynamicMixingEditorQuickItem::setAnchorLineColor(const QColor &color) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->anchorLineColor == color) {
            return;
        }
        d->anchorLineColor = color;
        update();
        Q_EMIT anchorLineColorChanged();
    }

    QColor DynamicMixingEditorQuickItem::selectedAnchorLineColor() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->selectedAnchorLineColor;
    }

    void DynamicMixingEditorQuickItem::setSelectedAnchorLineColor(const QColor &color) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->selectedAnchorLineColor == color) {
            return;
        }
        d->selectedAnchorLineColor = color;
        update();
        Q_EMIT selectedAnchorLineColorChanged();
    }

    QColor DynamicMixingEditorQuickItem::handleColor() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->handleColor;
    }

    void DynamicMixingEditorQuickItem::setHandleColor(const QColor &color) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->handleColor == color) {
            return;
        }
        d->handleColor = color;
        update();
        Q_EMIT handleColorChanged();
    }

    QColor DynamicMixingEditorQuickItem::selectedHandleBorderColor() const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->selectedHandleBorderColor;
    }

    void DynamicMixingEditorQuickItem::setSelectedHandleBorderColor(const QColor &color) {
        Q_D(DynamicMixingEditorQuickItem);
        if (d->selectedHandleBorderColor == color) {
            return;
        }
        d->selectedHandleBorderColor = color;
        update();
        Q_EMIT selectedHandleBorderColorChanged();
    }

    QVariantMap DynamicMixingEditorQuickItem::hitTest(const QPointF &point, double radius) const {
        Q_D(const DynamicMixingEditorQuickItem);
        const auto result = d->hitTest(point, radius);
        if (!result.anchor) {
            return {};
        }
        return {
            {QStringLiteral("anchor"), QVariant::fromValue(result.anchor)},
            {QStringLiteral("handleIndex"), result.handleIndex},
        };
    }

    DynamicMixingAnchorViewModel *DynamicMixingEditorQuickItem::anchorAt(const QPointF &point,
                                                                         double radius) const {
        Q_D(const DynamicMixingEditorQuickItem);
        return d->hitTest(point, radius).anchor;
    }

    QObjectList DynamicMixingEditorQuickItem::anchorsInTimeRange(int firstPosition,
                                                                  int lastPosition) const {
        Q_D(const DynamicMixingEditorQuickItem);
        if (!d->dynamicMixingViewModel || (firstPosition < 0 && lastPosition < 0)) {
            return {};
        }
        firstPosition = std::max(0, firstPosition);
        lastPosition = std::max(0, lastPosition);
        if (firstPosition > lastPosition) {
            std::swap(firstPosition, lastPosition);
        }
        const qint64 length = static_cast<qint64>(lastPosition) - firstPosition + 1;
        if (length <= std::numeric_limits<int>::max()) {
            return d->dynamicMixingViewModel->effectiveSlice(firstPosition,
                                                              static_cast<int>(length));
        }
        auto result = d->dynamicMixingViewModel->effectiveSlice(firstPosition,
                                                                 std::numeric_limits<int>::max());
        if (auto *lastItem = d->dynamicMixingViewModel->itemAtPosition(lastPosition);
            lastItem && !result.contains(lastItem)) {
            result.append(lastItem);
        }
        return result;
    }

    bool DynamicMixingEditorQuickItem::beginAnchorMove(const QObjectList &items,
                                                        DynamicMixingAnchorViewModel *primaryItem,
                                                        int handleIndex,
                                                        const QPointF &point) {
        Q_D(DynamicMixingEditorQuickItem);
        if (!primaryItem || primaryItem->dynamicMixingViewModel() != d->dynamicMixingViewModel
            || !d->movingAnchors.isEmpty() || height() <= 0.0 || !d->timeViewModel
            || !d->timeLayoutViewModel || d->timeLayoutViewModel->pixelDensity() <= 0.0
            || handleIndex < -1
            || handleIndex >= d->dynamicMixingViewModel->voiceCount() - 1) {
            return false;
        }

        QList<DynamicMixingAnchorViewModel *> movingItems;
        QSet<DynamicMixingAnchorViewModel *> uniqueItems;
        movingItems.reserve(items.size() + 1);
        for (auto *object : items) {
            auto *item = qobject_cast<DynamicMixingAnchorViewModel *>(object);
            if (!item || item->dynamicMixingViewModel() != d->dynamicMixingViewModel
                || uniqueItems.contains(item)) {
                continue;
            }
            uniqueItems.insert(item);
            movingItems.append(item);
        }
        if (!uniqueItems.contains(primaryItem)) {
            movingItems.append(primaryItem);
        }
        if (!d->dynamicMixingViewModel->beginInteractiveMove(movingItems, primaryItem)) {
            return false;
        }

        d->movingAnchors.reserve(movingItems.size());
        for (auto *item : movingItems) {
            d->movingAnchors.append(DynamicMixingEditorQuickItemPrivate::MovingAnchorState {
                item,
                item->position(),
                item->ratio(),
                d->dynamicMixingViewModel->effectiveRatio(item),
            });
        }
        d->primaryMovingAnchor = primaryItem;
        d->anchorMoveStartPoint = point;
        d->primaryAnchorStartPosition = primaryItem->position();
        d->movingHandleIndex = handleIndex;
        d->movingVoiceCount = d->dynamicMixingViewModel->voiceCount();
        d->moveAxis = handleIndex < 0
            ? DynamicMixingEditorQuickItemPrivate::HorizontalAxis
            : DynamicMixingEditorQuickItemPrivate::PendingAxis;
        return true;
    }

    bool DynamicMixingEditorQuickItem::updateAnchorMove(const QPointF &point, int targetPosition) {
        Q_D(DynamicMixingEditorQuickItem);
        if (!d->primaryMovingAnchor || d->movingAnchors.isEmpty()
            || !d->dynamicMixingViewModel || height() <= 0.0) {
            return false;
        }

        if (d->moveAxis == DynamicMixingEditorQuickItemPrivate::PendingAxis) {
            const QPointF delta = point - d->anchorMoveStartPoint;
            const int startDragDistance = QGuiApplication::styleHints()->startDragDistance();
            if (std::hypot(delta.x(), delta.y()) < startDragDistance) {
                return true;
            }
            d->moveAxis = std::abs(delta.x()) >= std::abs(delta.y())
                ? DynamicMixingEditorQuickItemPrivate::HorizontalAxis
                : DynamicMixingEditorQuickItemPrivate::VerticalAxis;
        }

        QList<DynamicMixingAnchorViewModel *> items;
        QList<int> positions;
        QList<QList<double>> ratios;
        items.reserve(d->movingAnchors.size());
        positions.reserve(d->movingAnchors.size());
        ratios.reserve(d->movingAnchors.size());

        if (d->moveAxis == DynamicMixingEditorQuickItemPrivate::HorizontalAxis) {
            qint64 minimumPosition = std::numeric_limits<int>::max();
            qint64 maximumPosition = 0;
            for (const auto &state : std::as_const(d->movingAnchors)) {
                if (!state.item) {
                    return false;
                }
                minimumPosition = std::min<qint64>(minimumPosition, state.position);
                maximumPosition = std::max<qint64>(maximumPosition, state.position);
            }
            qint64 deltaPosition = static_cast<qint64>(targetPosition)
                - d->primaryAnchorStartPosition;
            deltaPosition = std::clamp(deltaPosition,
                                       -minimumPosition,
                                       static_cast<qint64>(std::numeric_limits<int>::max())
                                           - maximumPosition);
            for (const auto &state : std::as_const(d->movingAnchors)) {
                items.append(state.item);
                positions.append(static_cast<int>(state.position + deltaPosition));
                ratios.append(state.sourceRatio);
            }
        } else {
            if (d->movingVoiceCount != d->dynamicMixingViewModel->voiceCount()
                || d->movingHandleIndex < 0
                || d->movingHandleIndex >= d->movingVoiceCount - 1) {
                return false;
            }
            double minimumDelta = -1.0;
            double maximumDelta = 1.0;
            for (const auto &state : std::as_const(d->movingAnchors)) {
                if (!state.item || state.effectiveRatio.size() != d->movingVoiceCount - 1) {
                    return false;
                }
                double lowerBoundary = 0.0;
                for (int i = 0; i < d->movingHandleIndex; ++i) {
                    lowerBoundary += state.effectiveRatio.at(i);
                }
                const double boundary = lowerBoundary
                    + state.effectiveRatio.at(d->movingHandleIndex);
                const double upperBoundary = d->movingHandleIndex + 1 < state.effectiveRatio.size()
                    ? boundary + state.effectiveRatio.at(d->movingHandleIndex + 1)
                    : 1.0;
                minimumDelta = std::max(minimumDelta, lowerBoundary - boundary);
                maximumDelta = std::min(maximumDelta, upperBoundary - boundary);
            }
            const double requestedDelta = (point.y() - d->anchorMoveStartPoint.y()) / height();
            const double ratioDelta = std::clamp(requestedDelta, minimumDelta, maximumDelta);
            for (const auto &state : std::as_const(d->movingAnchors)) {
                auto ratio = state.effectiveRatio;
                const int index = d->movingHandleIndex;
                const double upperPairBoundary = index + 1 < ratio.size()
                    ? ratio.at(index) + ratio.at(index + 1)
                    : ratio.at(index) + (1.0 - std::accumulate(ratio.cbegin(), ratio.cend(), 0.0));
                ratio[index] = std::clamp(ratio.at(index) + ratioDelta, 0.0, upperPairBoundary);
                if (index + 1 < ratio.size()) {
                    ratio[index + 1] = std::clamp(upperPairBoundary - ratio.at(index),
                                                  0.0, 1.0);
                }
                items.append(state.item);
                positions.append(state.position);
                ratios.append(boundedRatio(std::move(ratio)));
            }
        }
        return d->dynamicMixingViewModel->updateInteractiveMove(items, positions, ratios);
    }

    QObjectList DynamicMixingEditorQuickItem::commitAnchorMove() {
        Q_D(DynamicMixingEditorQuickItem);
        if (!d->primaryMovingAnchor || !d->dynamicMixingViewModel) {
            return {};
        }
        const auto removedItems = d->dynamicMixingViewModel->commitInteractiveMove(
            d->primaryMovingAnchor);
        d->clearMoveState();
        return removedItems;
    }

    bool DynamicMixingEditorQuickItem::abortAnchorMove() {
        Q_D(DynamicMixingEditorQuickItem);
        if (!d->primaryMovingAnchor || !d->dynamicMixingViewModel) {
            return false;
        }
        const bool aborted = d->dynamicMixingViewModel->abortInteractiveMove(
            d->primaryMovingAnchor);
        if (aborted) {
            d->clearMoveState();
        }
        return aborted;
    }

    void DynamicMixingEditorQuickItem::updatePolish() {
        Q_D(DynamicMixingEditorQuickItem);
        d->rebuildGeometry();
    }

    QSGNode *DynamicMixingEditorQuickItem::updatePaintNode(QSGNode *oldNode,
                                                           UpdatePaintNodeData *) {
        Q_D(DynamicMixingEditorQuickItem);
        if (usesSoftwareRenderer(this)) {
            auto *node = dynamic_cast<DynamicMixingSoftwareNode *>(oldNode);
            if (!node) {
                delete oldNode;
                node = new DynamicMixingSoftwareNode(this);
            }
            node->synchronize(d->snapshot,
                              d->snapshotRevision,
                              d->colors,
                              d->boundaryLineColor,
                              d->anchorLineColor,
                              d->selectedAnchorLineColor,
                              d->selectedHandleBorderColor,
                              d->handleColor);
            return node;
        }
        auto *root = dynamic_cast<DynamicMixingHardwareNode *>(oldNode);
        if (!root) {
            delete oldNode;
            root = new DynamicMixingHardwareNode;
            root->appendChildNode(new QSGNode);
            for (int i = 1; i < RootNodeCount; ++i) {
                root->appendChildNode(createGeometryNode());
            }
        }

        auto *fillContainer = root->childAtIndex(FillContainerNode);
        ensureGeometryNodeCount(fillContainer, d->snapshot.fills.size());
        for (int i = 0; i < d->snapshot.fills.size(); ++i) {
            updateGeometryNode(static_cast<QSGGeometryNode *>(fillContainer->childAtIndex(i)),
                               d->snapshot.fills.at(i), d->colors.at(i));
        }
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(BoundaryNode)),
                           d->snapshot.boundaries, d->boundaryLineColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(AnchorLineNode)),
                           d->snapshot.anchorLines, d->anchorLineColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(SelectedAnchorLineNode)),
                           d->snapshot.selectedAnchorLines, d->selectedAnchorLineColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(SelectedHandleBorderNode)),
                           d->snapshot.selectedHandleBorders, d->selectedHandleBorderColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(HandleNode)),
                           d->snapshot.handles, d->handleColor);
        return root;
    }

}

#include "moc_DynamicMixingEditorQuickItem_p.cpp"
