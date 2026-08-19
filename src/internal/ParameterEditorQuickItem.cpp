#include "ParameterEditorQuickItem_p.h"
#include "ParameterEditorQuickItem_p_p.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>

#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSet>

#include <ScopicFlowCore/AnchorParameterViewModel.h>
#include <ScopicFlowCore/FreeParameterViewModel.h>
#include <ScopicFlowCore/ParameterAnchorViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>

namespace sflow {

    namespace {

        constexpr double lineWidth = 1.5;
        constexpr double accentLineWidth = 2.0;
        constexpr double dashLength = 6.0;
        constexpr double dashGap = 4.0;
        constexpr double anchorRadius = 4.0;
        constexpr double selectedAnchorRadius = 6.0;
        constexpr int circleSegments = 12;

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

        int boundedRoundPosition(double position) {
            if (!std::isfinite(position) || position <= 0.0) {
                return 0;
            }
            if (position >= std::numeric_limits<int>::max()) {
                return std::numeric_limits<int>::max();
            }
            return qRound(position);
        }

        int extendedPosition(int position, int extension) {
            return static_cast<int>(std::min<qint64>(std::numeric_limits<int>::max(),
                                                     static_cast<qint64>(position) + extension));
        }

        int inclusiveLength(int start, int end) {
            return static_cast<int>(std::min<qint64>(std::numeric_limits<int>::max(),
                                                     static_cast<qint64>(end) - start + 1));
        }

        enum NodeIndex {
            FillNode,
            SolidNode,
            DashedNode,
            AccentNode,
            SelectedAnchorNode,
            AnchorNode,
            ReferenceNode,
            NodeCount,
        };

        void appendTriangle(QVector<QPointF> &geometry, const QPointF &a, const QPointF &b, const QPointF &c) {
            geometry.append(a);
            geometry.append(b);
            geometry.append(c);
        }

        void appendLineQuad(QVector<QPointF> &geometry, const QPointF &a, const QPointF &b, double width) {
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

        void appendDashedLine(QVector<QPointF> &geometry,
                              const QPointF &a,
                              const QPointF &b,
                              double width,
                              double &phase) {
            const QPointF delta = b - a;
            const double length = std::hypot(delta.x(), delta.y());
            if (qFuzzyIsNull(length)) {
                return;
            }
            const double patternLength = dashLength + dashGap;
            double offset = 0.0;
            while (offset < length) {
                const double patternPosition = std::fmod(phase + offset, patternLength);
                const bool drawing = patternPosition < dashLength;
                const double patternEnd = drawing ? dashLength : patternLength;
                const double endOffset = std::min(length, offset + patternEnd - patternPosition);
                if (drawing) {
                    appendLineQuad(geometry,
                                   a + delta * (offset / length),
                                   a + delta * (endOffset / length),
                                   width);
                }
                offset = endOffset;
            }
            phase = std::fmod(phase + length, patternLength);
        }

        void appendFillQuad(QVector<QPointF> &geometry,
                            const QPointF &a,
                            const QPointF &b,
                            double baseline) {
            const QPointF bottomA(a.x(), baseline);
            const QPointF bottomB(b.x(), baseline);
            appendTriangle(geometry, a, bottomA, b);
            appendTriangle(geometry, b, bottomA, bottomB);
        }

        void appendCircle(QVector<QPointF> &geometry, const QPointF &center, double radius) {
            for (int i = 0; i < circleSegments; ++i) {
                const double angle1 = 2.0 * std::numbers::pi * i / circleSegments;
                const double angle2 = 2.0 * std::numbers::pi * (i + 1) / circleSegments;
                appendTriangle(geometry,
                               center,
                               center + QPointF(std::cos(angle1) * radius, std::sin(angle1) * radius),
                               center + QPointF(std::cos(angle2) * radius, std::sin(angle2) * radius));
            }
        }

        void appendDiamond(QVector<QPointF> &geometry, const QPointF &center, double radius) {
            const double vertexRadius = radius * std::numbers::sqrt2;
            const QPointF top(center.x(), center.y() - vertexRadius);
            const QPointF right(center.x() + vertexRadius, center.y());
            const QPointF bottom(center.x(), center.y() + vertexRadius);
            const QPointF left(center.x() - vertexRadius, center.y());
            appendTriangle(geometry, top, right, bottom);
            appendTriangle(geometry, top, bottom, left);
        }

        void appendSquare(QVector<QPointF> &geometry, const QPointF &center, double radius) {
            const QPointF topLeft(center.x() - radius, center.y() - radius);
            const QPointF topRight(center.x() + radius, center.y() - radius);
            const QPointF bottomRight(center.x() + radius, center.y() + radius);
            const QPointF bottomLeft(center.x() - radius, center.y() + radius);
            appendTriangle(geometry, topLeft, topRight, bottomRight);
            appendTriangle(geometry, topLeft, bottomRight, bottomLeft);
        }

        void appendAnchor(QVector<QPointF> &geometry,
                          const QPointF &center,
                          double radius,
                          ParameterAnchorViewModel::InterpolationMode interpolationMode) {
            switch (interpolationMode) {
                case ParameterAnchorViewModel::Hermite:
                    appendCircle(geometry, center, radius);
                    break;
                case ParameterAnchorViewModel::Linear:
                    appendDiamond(geometry, center, radius);
                    break;
                case ParameterAnchorViewModel::None:
                    appendSquare(geometry, center, radius);
                    break;
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

        void updateGeometryNode(QSGGeometryNode *node, const QVector<QPointF> &points, const QColor &color) {
            auto *geometry = node->geometry();
            geometry->allocate(static_cast<int>(points.size()));
            auto *vertices = geometry->vertexDataAsPoint2D();
            for (int i = 0; i < points.size(); ++i) {
                vertices[i].set(static_cast<float>(points.at(i).x()), static_cast<float>(points.at(i).y()));
            }
            auto *material = static_cast<QSGFlatColorMaterial *>(node->material());
            material->setColor(color);
            node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        }

        QVariant preferredValue(const AnchorParameterViewModel *anchorViewModel,
                                const FreeParameterViewModel *freeViewModel,
                                double position) {
            if (anchorViewModel) {
                const auto anchorValue = anchorViewModel->valueAt(position);
                if (anchorValue.isValid()) {
                    return anchorValue;
                }
            }
            return freeViewModel ? freeViewModel->valueAt(position) : QVariant();
        }

    }

    void ParameterEditorQuickItemPrivate::invalidate() {
        Q_Q(ParameterEditorQuickItem);
        q->polish();
        q->update();
    }

    void ParameterEditorQuickItemPrivate::reconnectModels() {
        Q_Q(ParameterEditorQuickItem);
        const QList<QObject *> objects {
            freeParameterViewModel.data(),
            anchorParameterViewModel.data(),
            originalParameterViewModel.data(),
            freeTransformParameterViewModel.data(),
            anchorTransformParameterViewModel.data(),
            timeViewModel.data(),
            timeLayoutViewModel.data(),
        };
        auto invalidate = [this] { this->invalidate(); };
        QSet<QObject *> disconnected;
        for (auto *object : objects) {
            if (object && !disconnected.contains(object)) {
                QObject::disconnect(object, nullptr, q, nullptr);
                QObject::connect(object, &QObject::destroyed, q, invalidate);
                disconnected.insert(object);
            }
        }
        if (freeParameterViewModel) {
            QObject::connect(freeParameterViewModel, &FreeParameterViewModel::valuesChanged, q, invalidate);
            QObject::connect(freeParameterViewModel, &FreeParameterViewModel::sizeChanged, q, invalidate);
        }
        if (originalParameterViewModel) {
            QObject::connect(originalParameterViewModel, &FreeParameterViewModel::valuesChanged, q, invalidate);
            QObject::connect(originalParameterViewModel, &FreeParameterViewModel::sizeChanged, q, invalidate);
        }
        if (freeTransformParameterViewModel) {
            QObject::connect(freeTransformParameterViewModel, &FreeParameterViewModel::valuesChanged, q, invalidate);
            QObject::connect(freeTransformParameterViewModel, &FreeParameterViewModel::sizeChanged, q, invalidate);
        }
        const auto connectAnchorModel = [q, invalidate](AnchorParameterViewModel *viewModel) {
            if (!viewModel) {
                return;
            }
            QObject::connect(viewModel, &AnchorParameterViewModel::itemsChanged, q, invalidate);
            QObject::connect(viewModel, &AnchorParameterViewModel::itemUpdated, q, invalidate);
            QObject::connect(viewModel, &AnchorParameterViewModel::movingItemChanged, q, invalidate);
        };
        connectAnchorModel(anchorParameterViewModel);
        connectAnchorModel(anchorTransformParameterViewModel);
        if (timeViewModel) {
            QObject::connect(timeViewModel, &TimeViewModel::startChanged, q, invalidate);
            QObject::connect(timeViewModel, &TimeViewModel::endChanged, q, invalidate);
        }
        if (timeLayoutViewModel) {
            QObject::connect(timeLayoutViewModel, &TimeLayoutViewModel::pixelDensityChanged, q, invalidate);
        }
    }

    double ParameterEditorQuickItemPrivate::positionForX(double x) const {
        if (!timeViewModel || !timeLayoutViewModel || timeLayoutViewModel->pixelDensity() <= 0.0) {
            return 0.0;
        }
        return timeViewModel->start() + x / timeLayoutViewModel->pixelDensity();
    }

    double ParameterEditorQuickItemPrivate::xForPosition(double position) const {
        if (!timeViewModel || !timeLayoutViewModel) {
            return 0.0;
        }
        return (position - timeViewModel->start()) * timeLayoutViewModel->pixelDensity();
    }

    double ParameterEditorQuickItemPrivate::transformFactor(double position) const {
        const auto value = preferredValue(anchorTransformParameterViewModel, freeTransformParameterViewModel, position);
        return value.isValid() ? value.toDouble() * 2.0 : 1.0;
    }

    QList<double> ParameterEditorQuickItemPrivate::transformFactorsAt(const QList<double> &sortedPositions) const {
        const auto anchorValues = anchorTransformParameterViewModel
            ? anchorTransformParameterViewModel->valuesAt(sortedPositions) : QList<QVariant>();
        QList<double> result;
        result.reserve(sortedPositions.size());
        for (qsizetype i = 0; i < sortedPositions.size(); ++i) {
            if (!anchorValues.isEmpty() && anchorValues.at(i).isValid()) {
                result.append(anchorValues.at(i).toDouble() * 2.0);
                continue;
            }
            if (freeTransformParameterViewModel) {
                const auto value = freeTransformParameterViewModel->valueAt(sortedPositions.at(i));
                if (value.isValid()) {
                    result.append(value.toDouble() * 2.0);
                    continue;
                }
            }
            result.append(1.0);
        }
        return result;
    }

    QVariant ParameterEditorQuickItemPrivate::untransformedValue(double position, double transformedValue) const {
        const double factor = transformFactor(position);
        if (!std::isfinite(factor) || qFuzzyIsNull(factor)) {
            return {};
        }
        return std::clamp(transformedValue / factor, 0.0, 1.0);
    }

    double ParameterEditorQuickItemPrivate::yForValue(double value) const {
        Q_Q(const ParameterEditorQuickItem);
        const double unclamped = (1.0 - value) * q->height();
        return std::clamp(unclamped, -4.0 * q->height(), 5.0 * q->height());
    }

    double ParameterEditorQuickItemPrivate::transformedAnchorValue(const ParameterAnchorViewModel *item) const {
        return item->value() * transformFactor(item->position());
    }

    void ParameterEditorQuickItemPrivate::rebuildGeometry() {
        Q_Q(ParameterEditorQuickItem);
        snapshot = {};
        if (!timeViewModel || !timeLayoutViewModel || timeLayoutViewModel->pixelDensity() <= 0.0
            || q->width() <= 0.0 || q->height() <= 0.0) {
            return;
        }

        QVector<double> sampleXs;
        const int integerWidth = static_cast<int>(std::ceil(q->width()));
        sampleXs.reserve(integerWidth + 16);
        for (int x = 0; x <= integerWidth; ++x) {
            sampleXs.append(std::min<double>(x, q->width()));
        }
        const double visibleStart = positionForX(0.0);
        const double visibleEnd = positionForX(q->width());
        const auto addAnchorBreakpoints = [&](AnchorParameterViewModel *viewModel) {
            if (!viewModel || visibleEnd < 0.0) {
                return;
            }
            const qsizetype mergeStart = sampleXs.size();
            if (visibleStart > std::numeric_limits<int>::max()) {
                return;
            }
            const int start = std::max(0, boundedFloorPosition(visibleStart) - 1);
            const int end = std::max(start, extendedPosition(boundedCeilPosition(visibleEnd), 2));
            for (auto *object : viewModel->effectiveSlice(start, inclusiveLength(start, end))) {
                auto *item = qobject_cast<ParameterAnchorViewModel *>(object);
                if (!item) {
                    continue;
                }
                const double x = xForPosition(item->position());
                if (x >= 0.0 && x <= q->width()) {
                    sampleXs.append(x);
                }
            }
            std::inplace_merge(sampleXs.begin(), sampleXs.begin() + mergeStart, sampleXs.end());
        };
        addAnchorBreakpoints(anchorParameterViewModel);
        addAnchorBreakpoints(anchorTransformParameterViewModel);
        sampleXs.erase(std::unique(sampleXs.begin(), sampleXs.end(), [](double a, double b) {
            return qFuzzyCompare(a + 1.0, b + 1.0);
        }), sampleXs.end());

        struct Sample {
            QPointF point;
            ValueSource source = NoSource;
            bool valid = false;
        };

        QList<double> samplePositions;
        samplePositions.reserve(sampleXs.size());
        for (double x : sampleXs) {
            samplePositions.append(positionForX(x));
        }
        const auto anchorValues = anchorParameterViewModel
            ? anchorParameterViewModel->valuesAt(samplePositions) : QList<QVariant>();
        const auto transformFactors = transformFactorsAt(samplePositions);

        const auto evaluateFinal = [&](qsizetype index) {
            Sample sample;
            QVariant value;
            if (!anchorValues.isEmpty()) {
                value = anchorValues.at(index);
            }
            if (!value.isValid() && freeParameterViewModel) {
                value = freeParameterViewModel->valueAt(samplePositions.at(index));
            }
            if (value.isValid()) {
                sample.source = EditedSource;
            } else if (originalParameterViewModel) {
                value = originalParameterViewModel->valueAt(samplePositions.at(index));
                if (value.isValid()) {
                    sample.source = OriginalSource;
                }
            }
            if (!value.isValid() && defaultValueEnabled) {
                value = defaultValue;
                sample.source = DefaultSource;
            }
            if (value.isValid()) {
                const double transformedValue = value.toDouble() * transformFactors.at(index);
                if (std::isfinite(transformedValue)) {
                    sample.valid = true;
                    sample.point.setY(yForValue(transformedValue));
                }
            }
            return sample;
        };

        const auto evaluateOverlay = [&](qsizetype index) {
            QVariant value;
            if (editLayer == ParameterEditorQuickItem::FreeLayer && freeParameterViewModel) {
                value = freeParameterViewModel->valueAt(samplePositions.at(index));
            } else if (editLayer == ParameterEditorQuickItem::AnchorLayer && anchorParameterViewModel) {
                value = anchorValues.at(index);
            }
            if (!value.isValid()) {
                return QVariant();
            }
            const double transformedValue = value.toDouble() * transformFactors.at(index);
            return std::isfinite(transformedValue) ? QVariant(transformedValue) : QVariant();
        };

        double fillY = q->height();
        if (fillMode == ParameterEditorQuickItem::TopFill) {
            fillY = 0.0;
        } else if (fillMode == ParameterEditorQuickItem::BaselineFill) {
            fillY = yForValue(fillBaseline);
        }

        Sample previousFinal;
        QPointF previousOverlay;
        bool previousOverlayValid = false;
        double dashedPhase = 0.0;
        bool previousSegmentDashed = false;
        for (qsizetype index = 0; index < sampleXs.size(); ++index) {
            const double x = sampleXs.at(index);
            auto currentFinal = evaluateFinal(index);
            currentFinal.point.setX(x);
            bool currentSegmentDashed = false;
            if (previousFinal.valid && currentFinal.valid) {
                if (fillMode != ParameterEditorQuickItem::NoFill) {
                    appendFillQuad(snapshot.fill, previousFinal.point, currentFinal.point, fillY);
                }
                const bool fallbackSegment = previousFinal.source != EditedSource
                    && currentFinal.source != EditedSource;
                if (!fallbackSegment || fallbackDisplayMode == ParameterEditorQuickItem::CurveSolid) {
                    appendLineQuad(snapshot.solid, previousFinal.point, currentFinal.point, lineWidth);
                } else if (fallbackDisplayMode == ParameterEditorQuickItem::CurveDashed) {
                    if (!previousSegmentDashed) {
                        dashedPhase = 0.0;
                    }
                    appendDashedLine(snapshot.dashed, previousFinal.point, currentFinal.point, lineWidth, dashedPhase);
                    currentSegmentDashed = true;
                }
            }
            previousSegmentDashed = currentSegmentDashed;
            previousFinal = currentFinal;

            const auto overlayValue = evaluateOverlay(index);
            const bool overlayValid = overlayValue.isValid();
            const QPointF currentOverlay(x, overlayValid ? yForValue(overlayValue.toDouble()) : 0.0);
            if (previousOverlayValid && overlayValid) {
                appendLineQuad(snapshot.accent, previousOverlay, currentOverlay, accentLineWidth);
            }
            previousOverlay = currentOverlay;
            previousOverlayValid = overlayValid;
        }

        if (referenceVisible) {
            appendLineQuad(snapshot.reference,
                           QPointF(0.0, yForValue(referenceBaseline)),
                           QPointF(q->width(), yForValue(referenceBaseline)),
                           lineWidth);
        }

        if (editLayer == ParameterEditorQuickItem::AnchorLayer && anchorParameterViewModel && visibleEnd >= 0.0) {
            const int start = std::max(0, boundedFloorPosition(visibleStart) - 1);
            const int end = std::max(start, extendedPosition(boundedCeilPosition(visibleEnd), 2));
            const auto objects = anchorParameterViewModel->effectiveSlice(start, inclusiveLength(start, end));
            QList<double> anchorPositions;
            anchorPositions.reserve(objects.size());
            for (auto *object : objects) {
                anchorPositions.append(qobject_cast<ParameterAnchorViewModel *>(object)->position());
            }
            const auto anchorTransformFactors = transformFactorsAt(anchorPositions);
            for (qsizetype i = 0; i < objects.size(); ++i) {
                auto *item = qobject_cast<ParameterAnchorViewModel *>(objects.at(i));
                const QPointF center(xForPosition(item->position()),
                                     yForValue(item->value() * anchorTransformFactors.at(i)));
                if (item->isSelected()) {
                    appendAnchor(snapshot.selectedAnchors,
                                 center,
                                 selectedAnchorRadius,
                                 item->interpolationMode());
                }
                appendAnchor(snapshot.anchors, center, anchorRadius, item->interpolationMode());
            }
        }
    }

    ParameterEditorQuickItem::ParameterEditorQuickItem(QQuickItem *parent)
        : QQuickItem(parent), d_ptr(new ParameterEditorQuickItemPrivate) {
        Q_D(ParameterEditorQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
        connect(this, &QQuickItem::widthChanged, this, [d] { d->invalidate(); });
        connect(this, &QQuickItem::heightChanged, this, [d] { d->invalidate(); });
    }

    ParameterEditorQuickItem::~ParameterEditorQuickItem() = default;

    FreeParameterViewModel *ParameterEditorQuickItem::freeParameterViewModel() const {
        Q_D(const ParameterEditorQuickItem);
        return d->freeParameterViewModel;
    }

    void ParameterEditorQuickItem::setFreeParameterViewModel(FreeParameterViewModel *viewModel) {
        Q_D(ParameterEditorQuickItem);
        if (d->freeParameterViewModel == viewModel) {
            return;
        }
        if (d->freeParameterViewModel) {
            QObject::disconnect(d->freeParameterViewModel, nullptr, this, nullptr);
        }
        d->freeParameterViewModel = viewModel;
        d->reconnectModels();
        d->invalidate();
        Q_EMIT freeParameterViewModelChanged();
    }

    AnchorParameterViewModel *ParameterEditorQuickItem::anchorParameterViewModel() const {
        Q_D(const ParameterEditorQuickItem);
        return d->anchorParameterViewModel;
    }

    void ParameterEditorQuickItem::setAnchorParameterViewModel(AnchorParameterViewModel *viewModel) {
        Q_D(ParameterEditorQuickItem);
        if (d->anchorParameterViewModel == viewModel) {
            return;
        }
        if (d->anchorParameterViewModel) {
            QObject::disconnect(d->anchorParameterViewModel, nullptr, this, nullptr);
        }
        d->anchorParameterViewModel = viewModel;
        d->reconnectModels();
        d->invalidate();
        Q_EMIT anchorParameterViewModelChanged();
    }

    FreeParameterViewModel *ParameterEditorQuickItem::originalParameterViewModel() const {
        Q_D(const ParameterEditorQuickItem);
        return d->originalParameterViewModel;
    }

    void ParameterEditorQuickItem::setOriginalParameterViewModel(FreeParameterViewModel *viewModel) {
        Q_D(ParameterEditorQuickItem);
        if (d->originalParameterViewModel == viewModel) {
            return;
        }
        if (d->originalParameterViewModel) {
            QObject::disconnect(d->originalParameterViewModel, nullptr, this, nullptr);
        }
        d->originalParameterViewModel = viewModel;
        d->reconnectModels();
        d->invalidate();
        Q_EMIT originalParameterViewModelChanged();
    }

    FreeParameterViewModel *ParameterEditorQuickItem::freeTransformParameterViewModel() const {
        Q_D(const ParameterEditorQuickItem);
        return d->freeTransformParameterViewModel;
    }

    void ParameterEditorQuickItem::setFreeTransformParameterViewModel(FreeParameterViewModel *viewModel) {
        Q_D(ParameterEditorQuickItem);
        if (d->freeTransformParameterViewModel == viewModel) {
            return;
        }
        if (d->freeTransformParameterViewModel) {
            QObject::disconnect(d->freeTransformParameterViewModel, nullptr, this, nullptr);
        }
        d->freeTransformParameterViewModel = viewModel;
        d->reconnectModels();
        d->invalidate();
        Q_EMIT freeTransformParameterViewModelChanged();
    }

    AnchorParameterViewModel *ParameterEditorQuickItem::anchorTransformParameterViewModel() const {
        Q_D(const ParameterEditorQuickItem);
        return d->anchorTransformParameterViewModel;
    }

    void ParameterEditorQuickItem::setAnchorTransformParameterViewModel(AnchorParameterViewModel *viewModel) {
        Q_D(ParameterEditorQuickItem);
        if (d->anchorTransformParameterViewModel == viewModel) {
            return;
        }
        if (d->anchorTransformParameterViewModel) {
            QObject::disconnect(d->anchorTransformParameterViewModel, nullptr, this, nullptr);
        }
        d->anchorTransformParameterViewModel = viewModel;
        d->reconnectModels();
        d->invalidate();
        Q_EMIT anchorTransformParameterViewModelChanged();
    }

    TimeViewModel *ParameterEditorQuickItem::timeViewModel() const {
        Q_D(const ParameterEditorQuickItem);
        return d->timeViewModel;
    }

    void ParameterEditorQuickItem::setTimeViewModel(TimeViewModel *viewModel) {
        Q_D(ParameterEditorQuickItem);
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

    TimeLayoutViewModel *ParameterEditorQuickItem::timeLayoutViewModel() const {
        Q_D(const ParameterEditorQuickItem);
        return d->timeLayoutViewModel;
    }

    void ParameterEditorQuickItem::setTimeLayoutViewModel(TimeLayoutViewModel *viewModel) {
        Q_D(ParameterEditorQuickItem);
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

    ParameterEditorQuickItem::FillMode ParameterEditorQuickItem::fillMode() const {
        Q_D(const ParameterEditorQuickItem);
        return d->fillMode;
    }

    void ParameterEditorQuickItem::setFillMode(FillMode mode) {
        Q_D(ParameterEditorQuickItem);
        if (d->fillMode == mode) {
            return;
        }
        d->fillMode = mode;
        d->invalidate();
        Q_EMIT fillModeChanged();
    }

    double ParameterEditorQuickItem::fillBaseline() const {
        Q_D(const ParameterEditorQuickItem);
        return d->fillBaseline;
    }

    void ParameterEditorQuickItem::setFillBaseline(double value) {
        Q_D(ParameterEditorQuickItem);
        if (d->fillBaseline == value) {
            return;
        }
        d->fillBaseline = value;
        d->invalidate();
        Q_EMIT fillBaselineChanged();
    }

    bool ParameterEditorQuickItem::referenceVisible() const {
        Q_D(const ParameterEditorQuickItem);
        return d->referenceVisible;
    }

    void ParameterEditorQuickItem::setReferenceVisible(bool visible) {
        Q_D(ParameterEditorQuickItem);
        if (d->referenceVisible == visible) {
            return;
        }
        d->referenceVisible = visible;
        d->invalidate();
        Q_EMIT referenceVisibleChanged();
    }

    double ParameterEditorQuickItem::referenceBaseline() const {
        Q_D(const ParameterEditorQuickItem);
        return d->referenceBaseline;
    }

    void ParameterEditorQuickItem::setReferenceBaseline(double value) {
        Q_D(ParameterEditorQuickItem);
        if (d->referenceBaseline == value) {
            return;
        }
        d->referenceBaseline = value;
        d->invalidate();
        Q_EMIT referenceBaselineChanged();
    }

    bool ParameterEditorQuickItem::defaultValueEnabled() const {
        Q_D(const ParameterEditorQuickItem);
        return d->defaultValueEnabled;
    }

    void ParameterEditorQuickItem::setDefaultValueEnabled(bool enabled) {
        Q_D(ParameterEditorQuickItem);
        if (d->defaultValueEnabled == enabled) {
            return;
        }
        d->defaultValueEnabled = enabled;
        d->invalidate();
        Q_EMIT defaultValueEnabledChanged();
    }

    double ParameterEditorQuickItem::defaultValue() const {
        Q_D(const ParameterEditorQuickItem);
        return d->defaultValue;
    }

    void ParameterEditorQuickItem::setDefaultValue(double value) {
        Q_D(ParameterEditorQuickItem);
        if (d->defaultValue == value) {
            return;
        }
        d->defaultValue = value;
        d->invalidate();
        Q_EMIT defaultValueChanged();
    }

    ParameterEditorQuickItem::EditLayer ParameterEditorQuickItem::editLayer() const {
        Q_D(const ParameterEditorQuickItem);
        return d->editLayer;
    }

    void ParameterEditorQuickItem::setEditLayer(EditLayer layer) {
        Q_D(ParameterEditorQuickItem);
        if (d->editLayer == layer) {
            return;
        }
        d->editLayer = layer;
        d->invalidate();
        Q_EMIT editLayerChanged();
    }

    ParameterEditorQuickItem::CurveDisplayMode ParameterEditorQuickItem::originalAndDefaultCurveDisplayMode() const {
        Q_D(const ParameterEditorQuickItem);
        return d->fallbackDisplayMode;
    }

    void ParameterEditorQuickItem::setOriginalAndDefaultCurveDisplayMode(CurveDisplayMode mode) {
        Q_D(ParameterEditorQuickItem);
        if (d->fallbackDisplayMode == mode) {
            return;
        }
        d->fallbackDisplayMode = mode;
        d->invalidate();
        Q_EMIT originalAndDefaultCurveDisplayModeChanged();
    }

    QColor ParameterEditorQuickItem::curveColor() const {
        Q_D(const ParameterEditorQuickItem);
        return d->curveColor;
    }

    void ParameterEditorQuickItem::setCurveColor(const QColor &color) {
        Q_D(ParameterEditorQuickItem);
        if (d->curveColor == color) {
            return;
        }
        d->curveColor = color;
        update();
        Q_EMIT curveColorChanged();
    }

    QColor ParameterEditorQuickItem::dimmedCurveColor() const {
        Q_D(const ParameterEditorQuickItem);
        return d->dimmedCurveColor;
    }

    void ParameterEditorQuickItem::setDimmedCurveColor(const QColor &color) {
        Q_D(ParameterEditorQuickItem);
        if (d->dimmedCurveColor == color) {
            return;
        }
        d->dimmedCurveColor = color;
        update();
        Q_EMIT dimmedCurveColorChanged();
    }

    QColor ParameterEditorQuickItem::fillColor() const {
        Q_D(const ParameterEditorQuickItem);
        return d->fillColor;
    }

    void ParameterEditorQuickItem::setFillColor(const QColor &color) {
        Q_D(ParameterEditorQuickItem);
        if (d->fillColor == color) {
            return;
        }
        d->fillColor = color;
        update();
        Q_EMIT fillColorChanged();
    }

    QColor ParameterEditorQuickItem::dimmedFillColor() const {
        Q_D(const ParameterEditorQuickItem);
        return d->dimmedFillColor;
    }

    void ParameterEditorQuickItem::setDimmedFillColor(const QColor &color) {
        Q_D(ParameterEditorQuickItem);
        if (d->dimmedFillColor == color) {
            return;
        }
        d->dimmedFillColor = color;
        update();
        Q_EMIT dimmedFillColorChanged();
    }

    QColor ParameterEditorQuickItem::accentColor() const {
        Q_D(const ParameterEditorQuickItem);
        return d->accentColor;
    }

    void ParameterEditorQuickItem::setAccentColor(const QColor &color) {
        Q_D(ParameterEditorQuickItem);
        if (d->accentColor == color) {
            return;
        }
        d->accentColor = color;
        update();
        Q_EMIT accentColorChanged();
    }

    QColor ParameterEditorQuickItem::referenceColor() const {
        Q_D(const ParameterEditorQuickItem);
        return d->referenceColor;
    }

    void ParameterEditorQuickItem::setReferenceColor(const QColor &color) {
        Q_D(ParameterEditorQuickItem);
        if (d->referenceColor == color) {
            return;
        }
        d->referenceColor = color;
        update();
        Q_EMIT referenceColorChanged();
    }

    QColor ParameterEditorQuickItem::selectedAnchorColor() const {
        Q_D(const ParameterEditorQuickItem);
        return d->selectedAnchorColor;
    }

    void ParameterEditorQuickItem::setSelectedAnchorColor(const QColor &color) {
        Q_D(ParameterEditorQuickItem);
        if (d->selectedAnchorColor == color) {
            return;
        }
        d->selectedAnchorColor = color;
        update();
        Q_EMIT selectedAnchorColorChanged();
    }

    QVariant ParameterEditorQuickItem::transformedValueFromPoint(const QPointF &point) const {
        Q_D(const ParameterEditorQuickItem);
        if (height() <= 0.0) {
            return {};
        }
        const int position = boundedRoundPosition(d->positionForX(point.x()));
        return d->untransformedValue(position, 1.0 - point.y() / height());
    }

    bool ParameterEditorQuickItemPrivate::writeFreeLineSegment(const QPointF &from, const QPointF &to) {
        Q_Q(ParameterEditorQuickItem);
        if (!freeParameterViewModel || q->height() <= 0.0 || !timeViewModel || !timeLayoutViewModel) {
            return false;
        }
        const double fromPosition = std::max(0.0, positionForX(from.x()));
        const double toPosition = std::max(0.0, positionForX(to.x()));
        const double fromScaledPosition = fromPosition / FreeParameterViewModel::step();
        const double toScaledPosition = toPosition / FreeParameterViewModel::step();
        if (fromScaledPosition > std::numeric_limits<int>::max()
            || toScaledPosition > std::numeric_limits<int>::max()) {
            return false;
        }
        int fromIndex = boundedRoundPosition(fromScaledPosition);
        int toIndex = boundedRoundPosition(toScaledPosition);
        int firstIndex = std::min(fromIndex, toIndex);
        int lastIndex = std::max(fromIndex, toIndex);
        const qint64 sampleCount64 = static_cast<qint64>(lastIndex) - firstIndex + 1;
        if (sampleCount64 > std::numeric_limits<int>::max()) {
            return false;
        }
        const int sampleCount = static_cast<int>(sampleCount64);

        QList<double> samplePositions;
        samplePositions.reserve(sampleCount);
        for (int index = firstIndex;; ++index) {
            samplePositions.append(static_cast<double>(index) * FreeParameterViewModel::step());
            if (index == lastIndex) {
                break;
            }
        }
        const QList<double> transformFactors = transformFactorsAt(samplePositions);

        QList<QVariant> values;
        values.reserve(sampleCount);
        bool changed = false;
        for (int index = firstIndex;; ++index) {
            const qsizetype sampleIndex = index - firstIndex;
            const double samplePosition = samplePositions.at(sampleIndex);
            double ratio = 1.0;
            if (!qFuzzyCompare(fromPosition + 1.0, toPosition + 1.0)) {
                ratio = std::clamp((samplePosition - fromPosition) / (toPosition - fromPosition), 0.0, 1.0);
            }
            const double sampleY = from.y() + ratio * (to.y() - from.y());
            const double factor = transformFactors.at(sampleIndex);
            if (std::isfinite(factor) && !qFuzzyIsNull(factor)) {
                const double value = std::clamp((1.0 - sampleY / q->height()) / factor, 0.0, 1.0);
                values.append(value);
                const auto oldValue = freeParameterViewModel->valueAtIndex(index);
                changed |= !oldValue.isValid() || oldValue.toDouble() != value;
            } else {
                values.append(freeParameterViewModel->valueAtIndex(index));
            }
            if (index == lastIndex) {
                break;
            }
        }
        return changed && freeParameterViewModel->setValues(firstIndex, values);
    }

    bool ParameterEditorQuickItem::drawFreeSegment(const QPointF &from, const QPointF &to, bool erase) {
        Q_D(ParameterEditorQuickItem);
        if (!d->freeParameterViewModel || height() <= 0.0 || !d->timeViewModel || !d->timeLayoutViewModel) {
            return false;
        }
        if (!erase) {
            return d->writeFreeLineSegment(from, to);
        }
        const double fromPosition = std::max(0.0, d->positionForX(from.x()));
        const double toPosition = std::max(0.0, d->positionForX(to.x()));
        const double fromScaledPosition = fromPosition / FreeParameterViewModel::step();
        const double toScaledPosition = toPosition / FreeParameterViewModel::step();
        if (fromScaledPosition > std::numeric_limits<int>::max()
            || toScaledPosition > std::numeric_limits<int>::max()) {
            return false;
        }
        int fromIndex = boundedRoundPosition(fromScaledPosition);
        int toIndex = boundedRoundPosition(toScaledPosition);
        int firstIndex = std::min(fromIndex, toIndex);
        int lastIndex = std::max(fromIndex, toIndex);
        lastIndex = std::min(lastIndex, d->freeParameterViewModel->size() - 1);
        if (firstIndex > lastIndex) {
            return false;
        }
        const qint64 sampleCount64 = static_cast<qint64>(lastIndex) - firstIndex + 1;
        if (sampleCount64 > std::numeric_limits<int>::max()) {
            return false;
        }
        const int sampleCount = static_cast<int>(sampleCount64);

        QList<QVariant> values;
        values.reserve(sampleCount);
        bool changed = false;
        for (int index = firstIndex;; ++index) {
            values.append(QVariant());
            changed |= d->freeParameterViewModel->valueAtIndex(index).isValid();
            if (index == lastIndex) {
                break;
            }
        }
        return changed && d->freeParameterViewModel->setValues(firstIndex, values);
    }

    bool ParameterEditorQuickItem::brushFreeSegment(const QPointF &from, const QPointF &to) {
        Q_D(ParameterEditorQuickItem);
        if (!d->freeParameterViewModel || !d->originalParameterViewModel || height() <= 0.0
            || !d->timeViewModel || !d->timeLayoutViewModel) {
            return false;
        }
        const double fromPosition = std::max(0.0, d->positionForX(from.x()));
        const double toPosition = std::max(0.0, d->positionForX(to.x()));
        const double fromScaledPosition = fromPosition / FreeParameterViewModel::step();
        const double toScaledPosition = toPosition / FreeParameterViewModel::step();
        if (fromScaledPosition > std::numeric_limits<int>::max()
            || toScaledPosition > std::numeric_limits<int>::max()) {
            return false;
        }
        int fromIndex = boundedRoundPosition(fromScaledPosition);
        int toIndex = boundedRoundPosition(toScaledPosition);
        int firstIndex = std::min(fromIndex, toIndex);
        int lastIndex = std::max(fromIndex, toIndex);
        lastIndex = std::min(lastIndex,
                             std::min(d->freeParameterViewModel->size(), d->originalParameterViewModel->size()) - 1);
        if (firstIndex > lastIndex) {
            return false;
        }
        const qint64 sampleCount64 = static_cast<qint64>(lastIndex) - firstIndex + 1;
        if (sampleCount64 > std::numeric_limits<int>::max()) {
            return false;
        }
        const int sampleCount = static_cast<int>(sampleCount64);

        QList<QVariant> values;
        values.reserve(sampleCount);
        bool changed = false;
        for (int index = firstIndex;; ++index) {
            const auto originalValue = d->originalParameterViewModel->valueAtIndex(index);
            if (originalValue.isValid()) {
                values.append(originalValue);
                const auto oldValue = d->freeParameterViewModel->valueAtIndex(index);
                changed |= !oldValue.isValid() || oldValue.toDouble() != originalValue.toDouble();
            } else {
                values.append(d->freeParameterViewModel->valueAtIndex(index));
            }
            if (index == lastIndex) {
                break;
            }
        }
        return changed && d->freeParameterViewModel->setValues(firstIndex, values);
    }

    bool ParameterEditorQuickItem::beginFreeLine(const QPointF &point) {
        Q_D(ParameterEditorQuickItem);
        d->lineStartPoint = point;
        d->lineSnapshotFirst = 0;
        d->lineSnapshotLast = -1;
        d->lineSnapshot.clear();
        return drawFreeSegment(point, point, false);
    }

    bool ParameterEditorQuickItem::updateFreeLine(const QPointF &point) {
        Q_D(ParameterEditorQuickItem);
        if (!d->freeParameterViewModel || height() <= 0.0 || !d->timeViewModel || !d->timeLayoutViewModel) {
            return false;
        }

        if (d->lineSnapshotFirst <= d->lineSnapshotLast) {
            d->freeParameterViewModel->setValues(d->lineSnapshotFirst, d->lineSnapshot);
        }

        const double fromPosition = std::max(0.0, d->positionForX(d->lineStartPoint.x()));
        const double toPosition = std::max(0.0, d->positionForX(point.x()));
        const double fromScaledPosition = fromPosition / FreeParameterViewModel::step();
        const double toScaledPosition = toPosition / FreeParameterViewModel::step();
        if (fromScaledPosition > std::numeric_limits<int>::max()
            || toScaledPosition > std::numeric_limits<int>::max()) {
            return false;
        }
        int fromIndex = boundedRoundPosition(fromScaledPosition);
        int toIndex = boundedRoundPosition(toScaledPosition);
        const int newFirstIndex = std::min(fromIndex, toIndex);
        const int newLastIndex = std::max(fromIndex, toIndex);

        if (d->lineSnapshotLast < 0) {
            d->lineSnapshot.reserve(newLastIndex - newFirstIndex + 1);
            for (int index = newFirstIndex; index <= newLastIndex; ++index) {
                d->lineSnapshot.append(d->freeParameterViewModel->valueAtIndex(index));
            }
            d->lineSnapshotFirst = newFirstIndex;
            d->lineSnapshotLast = newLastIndex;
        } else {
            if (newFirstIndex < d->lineSnapshotFirst) {
                const int count = d->lineSnapshotFirst - newFirstIndex;
                QList<QVariant> prepended;
                prepended.reserve(count);
                for (int index = newFirstIndex; index < d->lineSnapshotFirst; ++index) {
                    prepended.append(d->freeParameterViewModel->valueAtIndex(index));
                }
                d->lineSnapshot = prepended + d->lineSnapshot;
                d->lineSnapshotFirst = newFirstIndex;
            }
            if (newLastIndex > d->lineSnapshotLast) {
                const int startIndex = std::max(d->lineSnapshotLast + 1, newFirstIndex);
                for (int index = startIndex; index <= newLastIndex; ++index) {
                    d->lineSnapshot.append(d->freeParameterViewModel->valueAtIndex(index));
                }
                d->lineSnapshotLast = newLastIndex;
            }
        }

        return d->writeFreeLineSegment(d->lineStartPoint, point);
    }

    void ParameterEditorQuickItem::commitFreeLine() {
        Q_D(ParameterEditorQuickItem);
        d->lineSnapshotFirst = 0;
        d->lineSnapshotLast = -1;
        d->lineSnapshot.clear();
    }

    bool ParameterEditorQuickItem::abortFreeLine() {
        Q_D(ParameterEditorQuickItem);
        if (d->lineSnapshotFirst <= d->lineSnapshotLast && d->freeParameterViewModel) {
            d->freeParameterViewModel->setValues(d->lineSnapshotFirst, d->lineSnapshot);
        }
        d->lineSnapshotFirst = 0;
        d->lineSnapshotLast = -1;
        d->lineSnapshot.clear();
        return true;
    }

    ParameterAnchorViewModel *ParameterEditorQuickItem::anchorAt(const QPointF &point, double radius) const {
        Q_D(const ParameterEditorQuickItem);
        if (!d->anchorParameterViewModel || radius < 0.0 || !d->timeLayoutViewModel || d->timeLayoutViewModel->pixelDensity() <= 0.0) {
            return nullptr;
        }
        const double leftPosition = std::max(0.0, d->positionForX(point.x() - radius));
        const double rightPosition = std::max(leftPosition, d->positionForX(point.x() + radius));
        if (leftPosition > std::numeric_limits<int>::max()) {
            return nullptr;
        }
        const int start = boundedFloorPosition(leftPosition);
        const int end = boundedCeilPosition(rightPosition);
        ParameterAnchorViewModel *nearestItem = nullptr;
        double nearestDistance = radius;
        for (auto *object : d->anchorParameterViewModel->effectiveSlice(start, inclusiveLength(start, end))) {
            auto *item = qobject_cast<ParameterAnchorViewModel *>(object);
            if (!item) {
                continue;
            }
            const QPointF itemPoint(d->xForPosition(item->position()), d->yForValue(d->transformedAnchorValue(item)));
            const double distance = std::hypot(itemPoint.x() - point.x(), itemPoint.y() - point.y());
            if (distance <= nearestDistance) {
                nearestDistance = distance;
                nearestItem = item;
            }
        }
        return nearestItem;
    }

    QObjectList ParameterEditorQuickItem::anchorsInRect(const QRectF &rect) const {
        Q_D(const ParameterEditorQuickItem);
        if (!d->anchorParameterViewModel) {
            return {};
        }
        const QRectF normalizedRect = rect.normalized();
        const double leftPosition = std::max(0.0, d->positionForX(normalizedRect.left()));
        const double rightPosition = std::max(leftPosition, d->positionForX(normalizedRect.right()));
        if (leftPosition > std::numeric_limits<int>::max()) {
            return {};
        }
        const int start = boundedFloorPosition(leftPosition);
        const int end = boundedCeilPosition(rightPosition);
        QObjectList result;
        for (auto *object : d->anchorParameterViewModel->effectiveSlice(start, inclusiveLength(start, end))) {
            auto *item = qobject_cast<ParameterAnchorViewModel *>(object);
            if (!item) {
                continue;
            }
            const QPointF itemPoint(d->xForPosition(item->position()), d->yForValue(d->transformedAnchorValue(item)));
            if (normalizedRect.contains(itemPoint)) {
                result.append(item);
            }
        }
        return result;
    }

    QObjectList ParameterEditorQuickItem::anchorsInTimeRange(int firstPosition, int lastPosition) const {
        Q_D(const ParameterEditorQuickItem);
        if (!d->anchorParameterViewModel || (firstPosition < 0 && lastPosition < 0)) {
            return {};
        }
        firstPosition = std::max(0, firstPosition);
        lastPosition = std::max(0, lastPosition);
        if (firstPosition > lastPosition) {
            std::swap(firstPosition, lastPosition);
        }

        const qint64 length = static_cast<qint64>(lastPosition) - firstPosition + 1;
        if (length <= std::numeric_limits<int>::max()) {
            return d->anchorParameterViewModel->effectiveSlice(firstPosition, static_cast<int>(length));
        }

        auto result = d->anchorParameterViewModel->effectiveSlice(firstPosition,
                                                                   std::numeric_limits<int>::max());
        if (auto *lastItem = d->anchorParameterViewModel->itemAtPosition(lastPosition)) {
            result.append(lastItem);
        }
        return result;
    }

    bool ParameterEditorQuickItem::beginAnchorMove(const QObjectList &items,
                                                    ParameterAnchorViewModel *primaryItem,
                                                    const QPointF &point) {
        Q_D(ParameterEditorQuickItem);
        if (!primaryItem || primaryItem->anchorParameterViewModel() != d->anchorParameterViewModel
            || !d->movingAnchors.isEmpty() || height() <= 0.0 || !d->timeViewModel
            || !d->timeLayoutViewModel || d->timeLayoutViewModel->pixelDensity() <= 0.0) {
            return false;
        }

        QList<ParameterAnchorViewModel *> movingItems;
        QSet<ParameterAnchorViewModel *> uniqueItems;
        movingItems.reserve(items.size() + 1);
        for (auto *object : items) {
            auto *item = qobject_cast<ParameterAnchorViewModel *>(object);
            if (!item || item->anchorParameterViewModel() != d->anchorParameterViewModel
                || uniqueItems.contains(item)) {
                continue;
            }
            uniqueItems.insert(item);
            movingItems.append(item);
        }
        if (!uniqueItems.contains(primaryItem)) {
            movingItems.append(primaryItem);
        }
        if (!d->anchorParameterViewModel->beginInteractiveMove(movingItems, primaryItem)) {
            return false;
        }

        d->movingAnchors.reserve(movingItems.size());
        for (auto *item : movingItems) {
            d->movingAnchors.append(ParameterEditorQuickItemPrivate::MovingAnchorState {
                item,
                item->position(),
                item->value(),
                d->transformedAnchorValue(item),
            });
        }
        d->primaryMovingAnchor = primaryItem;
        d->anchorMoveStartPosition = boundedRoundPosition(d->positionForX(point.x()));
        d->anchorMoveStartY = point.y();
        return true;
    }

    bool ParameterEditorQuickItem::updateAnchorMove(const QPointF &point) {
        Q_D(ParameterEditorQuickItem);
        if (!d->primaryMovingAnchor || d->movingAnchors.isEmpty() || !d->anchorParameterViewModel
            || height() <= 0.0) {
            return false;
        }

        qint64 minimumPosition = std::numeric_limits<int>::max();
        qint64 maximumPosition = 0;
        for (const auto &state : d->movingAnchors) {
            if (!state.item) {
                return false;
            }
            minimumPosition = std::min<qint64>(minimumPosition, state.position);
            maximumPosition = std::max<qint64>(maximumPosition, state.position);
        }

        const qint64 currentPosition = boundedRoundPosition(d->positionForX(point.x()));
        qint64 deltaPosition = currentPosition - d->anchorMoveStartPosition;
        deltaPosition = std::clamp(deltaPosition,
                                   -minimumPosition,
                                   static_cast<qint64>(std::numeric_limits<int>::max()) - maximumPosition);
        const double deltaTransformedValue = (d->anchorMoveStartY - point.y()) / height();

        QList<ParameterAnchorViewModel *> items;
        QList<int> positions;
        QList<double> values;
        items.reserve(d->movingAnchors.size());
        positions.reserve(d->movingAnchors.size());
        values.reserve(d->movingAnchors.size());
        for (const auto &state : d->movingAnchors) {
            const int position = static_cast<int>(state.position + deltaPosition);
            double value = state.value;
            const double factor = d->transformFactor(position);
            const double transformedValue = state.transformedValue + deltaTransformedValue;
            if (std::isfinite(factor) && !qFuzzyIsNull(factor) && std::isfinite(transformedValue)) {
                value = std::clamp(transformedValue / factor, 0.0, 1.0);
            }
            items.append(state.item);
            positions.append(position);
            values.append(value);
        }
        return d->anchorParameterViewModel->updateInteractiveMove(items, positions, values);
    }

    QObjectList ParameterEditorQuickItem::commitAnchorMove() {
        Q_D(ParameterEditorQuickItem);
        if (!d->primaryMovingAnchor || !d->anchorParameterViewModel) {
            return {};
        }
        const auto removedItems = d->anchorParameterViewModel->commitInteractiveMove(d->primaryMovingAnchor);
        d->movingAnchors.clear();
        d->primaryMovingAnchor = nullptr;
        return removedItems;
    }

    bool ParameterEditorQuickItem::abortAnchorMove() {
        Q_D(ParameterEditorQuickItem);
        if (!d->primaryMovingAnchor || !d->anchorParameterViewModel) {
            return false;
        }
        const bool aborted = d->anchorParameterViewModel->abortInteractiveMove(d->primaryMovingAnchor);
        if (aborted) {
            d->movingAnchors.clear();
            d->primaryMovingAnchor = nullptr;
        }
        return aborted;
    }

    bool ParameterEditorQuickItem::updateAnchorFromPoint(ParameterAnchorViewModel *item, const QPointF &point) {
        Q_D(ParameterEditorQuickItem);
        if (!item || item->anchorParameterViewModel() != d->anchorParameterViewModel || height() <= 0.0) {
            return false;
        }
        const int position = boundedRoundPosition(d->positionForX(point.x()));
        const bool positionChanged = d->anchorParameterViewModel->moveItem(item, position);
        const auto value = d->untransformedValue(position, 1.0 - point.y() / height());
        if (value.isValid()) {
            item->setValue(value.toDouble());
            return true;
        }
        return positionChanged;
    }

    void ParameterEditorQuickItem::updatePolish() {
        Q_D(ParameterEditorQuickItem);
        d->rebuildGeometry();
    }

    QSGNode *ParameterEditorQuickItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
        Q_D(ParameterEditorQuickItem);
        auto *root = oldNode;
        if (!root) {
            root = new QSGNode;
            for (int i = 0; i < NodeCount; ++i) {
                root->appendChildNode(createGeometryNode());
            }
        }
        const QColor finalCurveColor = d->editLayer == FinalLayer ? d->curveColor : d->dimmedCurveColor;
        const QColor finalFillColor = d->editLayer == FinalLayer ? d->fillColor : d->dimmedFillColor;
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(FillNode)), d->snapshot.fill, finalFillColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(SolidNode)), d->snapshot.solid, finalCurveColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(DashedNode)), d->snapshot.dashed, finalCurveColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(AccentNode)), d->snapshot.accent, d->accentColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(SelectedAnchorNode)), d->snapshot.selectedAnchors, d->selectedAnchorColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(AnchorNode)), d->snapshot.anchors, d->accentColor);
        updateGeometryNode(static_cast<QSGGeometryNode *>(root->childAtIndex(ReferenceNode)), d->snapshot.reference, d->referenceColor);
        return root;
    }

}

#include "moc_ParameterEditorQuickItem_p.cpp"
