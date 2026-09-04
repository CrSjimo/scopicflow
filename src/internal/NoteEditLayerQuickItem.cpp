#include "NoteEditLayerQuickItem_p.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

#include <QFontMetricsF>
#include <QMatrix4x4>
#include <QPainter>
#include <QPainterPath>
#include <QQuickWindow>
#include <QSGClipNode>
#include <QSGFlatColorMaterial>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGRendererInterface>
#include <QSGTextNode>
#include <QSGTransformNode>
#include <QTextLayout>
#include <QTimer>
#include <QVariant>

#include <SVSCraftQuick/SoftwarePainterNode.h>

#include <ScopicFlowCore/ClavierViewModel.h>
#include <ScopicFlowCore/NoteViewModel.h>
#include <ScopicFlowCore/RangeSequenceViewModel.h>
#include <ScopicFlowCore/SelectionController.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>

#include <ScopicFlowInternal/private/EdgeDragHandler_p.h>
#include <ScopicFlowInternal/private/NoteEditLayerQuickItem_p_p.h>
#include <ScopicFlowInternal/private/RubberBandSelector_p.h>

namespace sflow {

    namespace {

        constexpr qreal BodyInset = 0.5;
        constexpr qreal BorderWidth = 2.0;
        constexpr qreal CornerRadius = 4.0;
        constexpr int CornerSegments = 4;
        constexpr qreal ThumbnailBodyHeight = 4.0;
        constexpr qreal LyricHorizontalMargin = 6.0;
        constexpr qreal AdditionalTextHorizontalPadding = 6.5;
        constexpr qreal AdditionalTextVerticalPadding = 4.0;
        constexpr qreal EdgeHitMaximumWidth = 8.0;
        constexpr qreal EdgeHitWidthFraction = 1.0 / 3.0;
        constexpr qreal TransparentFillOpacity = 0.25;
        constexpr qreal ThumbnailFillOpacity = 0.5;
        constexpr qreal TransparentTextOpacity = 0.75;
        constexpr qreal MarkerOffset = 16.0;
        constexpr qsizetype NodePoolCapacity = 256;
        constexpr int ResidentTrimDelay = 75;
        constexpr qreal Pi = 3.14159265358979323846;

        QColor multiplyAlpha(QColor color, qreal factor) {
            color.setAlphaF(std::clamp(color.alphaF() * factor, 0.0, 1.0));
            return color;
        }

        QSizeF measureSingleLineText(const QString &text, const QFont &font) {
            QTextLayout layout(text, font);
            layout.beginLayout();
            const QTextLine line = layout.createLine();
            layout.endLayout();
            const QFontMetricsF metrics(font);
            return {
                line.isValid() ? line.naturalTextWidth() : 0,
                std::max(layout.boundingRect().height(), metrics.height()),
            };
        }

        QSGGeometryNode *createGeometryNode() {
            auto *node = new QSGGeometryNode;
            node->setFlag(QSGNode::OwnedByParent);
            node->setFlag(QSGNode::OwnsGeometry);
            node->setFlag(QSGNode::OwnsMaterial);
            auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
            geometry->setDrawingMode(QSGGeometry::DrawTriangles);
            node->setGeometry(geometry);
            node->setMaterial(new QSGFlatColorMaterial);
            return node;
        }

        std::vector<QPointF> roundedPerimeter(const QRectF &rect, qreal sourceRadius) {
            const qreal radius = std::clamp(sourceRadius, 0.0, 0.5 * std::min(rect.width(), rect.height()));
            std::vector<QPointF> perimeter;
            perimeter.reserve(CornerSegments * 4);
            const std::array<QPointF, 4> centers = {
                QPointF(rect.left() + radius, rect.top() + radius),
                QPointF(rect.right() - radius, rect.top() + radius),
                QPointF(rect.right() - radius, rect.bottom() - radius),
                QPointF(rect.left() + radius, rect.bottom() - radius),
            };
            const std::array<qreal, 4> startAngles = {Pi, -0.5 * Pi, 0.0, 0.5 * Pi};
            for (qsizetype corner = 0; corner < std::ssize(centers); ++corner) {
                for (int segment = 0; segment < CornerSegments; ++segment) {
                    const qreal angle = startAngles[corner] + 0.5 * Pi * segment / CornerSegments;
                    perimeter.emplace_back(
                        centers[corner].x() + std::cos(angle) * radius,
                        centers[corner].y() + std::sin(angle) * radius);
                }
            }
            return perimeter;
        }

        QPainterPath perimeterPath(const QRectF &rect, qreal radius) {
            QPainterPath path;
            if (rect.width() <= 0 || rect.height() <= 0) {
                return path;
            }
            const auto perimeter = roundedPerimeter(rect.normalized(), radius);
            if (perimeter.empty()) {
                return path;
            }
            path.moveTo(perimeter.front());
            for (qsizetype i = 1; i < std::ssize(perimeter); ++i) {
                path.lineTo(perimeter[i]);
            }
            path.closeSubpath();
            return path;
        }

        QPainterPath borderPath(const QRectF &rect, qreal radius, qreal borderWidth) {
            QPainterPath path = perimeterPath(rect, radius);
            const QRectF innerRect = rect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth);
            if (innerRect.width() > 0 && innerRect.height() > 0) {
                path.addPath(perimeterPath(innerRect, std::max<qreal>(0, radius - borderWidth)));
                path.setFillRule(Qt::OddEvenFill);
            }
            return path;
        }

        void updateRoundedGeometry(QSGGeometryNode *node, const QRectF &sourceRect, qreal sourceRadius, const QColor &color) {
            auto *geometry = node->geometry();
            if (sourceRect.width() <= 0 || sourceRect.height() <= 0 || color.alpha() == 0) {
                geometry->allocate(0);
                node->markDirty(QSGNode::DirtyGeometry);
                return;
            }
            const QRectF rect = sourceRect.normalized();
            const std::vector<QPointF> perimeter = roundedPerimeter(rect, sourceRadius);

            geometry->allocate(static_cast<int>(perimeter.size() * 3));
            auto *vertices = geometry->vertexDataAsPoint2D();
            const QPointF center = rect.center();
            for (qsizetype index = 0; index < std::ssize(perimeter); ++index) {
                const QPointF &first = perimeter[index];
                const QPointF &second = perimeter[(index + 1) % perimeter.size()];
                vertices[index * 3].set(center.x(), center.y());
                vertices[index * 3 + 1].set(first.x(), first.y());
                vertices[index * 3 + 2].set(second.x(), second.y());
            }
            static_cast<QSGFlatColorMaterial *>(node->material())->setColor(color);
            node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        }

        void updateRoundedBorderGeometry(QSGGeometryNode *node, const QRectF &sourceRect, qreal sourceRadius, qreal borderWidth, const QColor &color) {
            auto *geometry = node->geometry();
            if (sourceRect.width() <= 0 || sourceRect.height() <= 0 || borderWidth <= 0 || color.alpha() == 0) {
                geometry->allocate(0);
                node->markDirty(QSGNode::DirtyGeometry);
                return;
            }

            const QRectF outerRect = sourceRect.normalized();
            const QRectF innerRect = outerRect.adjusted(borderWidth, borderWidth, -borderWidth, -borderWidth);
            if (innerRect.width() <= 0 || innerRect.height() <= 0) {
                updateRoundedGeometry(node, outerRect, sourceRadius, color);
                return;
            }

            const std::vector<QPointF> outerPerimeter = roundedPerimeter(outerRect, sourceRadius);
            const std::vector<QPointF> innerPerimeter = roundedPerimeter(innerRect, std::max<qreal>(0, sourceRadius - borderWidth));
            geometry->allocate(static_cast<int>(outerPerimeter.size() * 6));
            auto *vertices = geometry->vertexDataAsPoint2D();
            for (qsizetype index = 0; index < std::ssize(outerPerimeter); ++index) {
                const qsizetype nextIndex = (index + 1) % outerPerimeter.size();
                const QPointF &outer = outerPerimeter[index];
                const QPointF &outerNext = outerPerimeter[nextIndex];
                const QPointF &inner = innerPerimeter[index];
                const QPointF &innerNext = innerPerimeter[nextIndex];
                vertices[index * 6].set(outer.x(), outer.y());
                vertices[index * 6 + 1].set(outerNext.x(), outerNext.y());
                vertices[index * 6 + 2].set(innerNext.x(), innerNext.y());
                vertices[index * 6 + 3].set(outer.x(), outer.y());
                vertices[index * 6 + 4].set(innerNext.x(), innerNext.y());
                vertices[index * 6 + 5].set(inner.x(), inner.y());
            }
            static_cast<QSGFlatColorMaterial *>(node->material())->setColor(color);
            node->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        }

        struct ClippedTextVisualState {
            QString text;
            QFont font;
            QColor color;
            QRectF clipRect;
            qreal left = 0;
            qreal top = 0;
            qreal centerHeight = 0;
            bool verticallyCentered = false;
        };

        class ClippedTextVisualNode : public QSGClipNode {
        public:
            ClippedTextVisualNode() {
                setFlag(QSGNode::OwnedByParent);
                setIsRectangular(true);
            }

            void synchronize(const ClippedTextVisualState &state, QQuickWindow *window) {
                const bool textVisible = window && !state.text.isEmpty() && state.color.alpha() > 0
                    && state.clipRect.width() > 0 && state.clipRect.height() > 0;
                const bool textChanged = m_text != state.text || m_font != state.font
                    || m_color != state.color || m_window != window || !m_textNode;
                if (!textVisible) {
                    clearTextNode();
                } else if (textChanged) {
                    clearTextNode();
                    QTextLayout layout(state.text, state.font);
                    layout.beginLayout();
                    layout.createLine();
                    layout.endLayout();
                    m_textHeight = layout.boundingRect().height();
                    m_textNode = window->createTextNode();
                    m_textNode->setFlag(QSGNode::OwnedByParent);
                    m_textNode->setColor(state.color);
                    m_textNode->addTextLayout({}, &layout);
                    appendChildNode(m_textNode);
                }

                const qreal textTop = state.verticallyCentered
                    ? 0.5 * (state.centerHeight - m_textHeight)
                    : state.top;
                if (m_textNode) {
                    QMatrix4x4 textMatrix;
                    textMatrix.translate(state.left, textTop);
                    m_textNode->setMatrix(textMatrix);
                }

                QRectF clipRect = state.clipRect;
                if (state.verticallyCentered) {
                    const qreal clipTop = std::min(clipRect.top(), textTop);
                    const qreal clipBottom = std::max(clipRect.bottom(), textTop + m_textHeight);
                    clipRect.setTop(clipTop);
                    clipRect.setBottom(clipBottom);
                }
                setClipRect(clipRect);

                m_text = state.text;
                m_font = state.font;
                m_color = state.color;
                m_window = window;
            }

        private:
            void clearTextNode() {
                if (!m_textNode)
                    return;
                removeChildNode(m_textNode);
                delete m_textNode;
                m_textNode = nullptr;
                m_textHeight = 0;
            }

            QSGTextNode *m_textNode = nullptr;
            QString m_text;
            QFont m_font;
            QColor m_color;
            qreal m_textHeight = 0;
            QQuickWindow *m_window = nullptr;
        };

        struct NoteVisualState {
            NoteEditLayerRecord record;
            qreal positionX = 0;
            qreal positionY = 0;
            qreal noteWidth = 0;
            qreal noteHeight = 0;
            QColor fillColor;
            QColor borderColor;
            QColor overlappedBorderColor;
            QColor textColor;
            QFont font;
            bool thumbnailDisplay = false;
        };

        class NoteShapeSoftwareNode : public SVS::SoftwarePainterNode {
        public:
            explicit NoteShapeSoftwareNode(QQuickItem *item) : SoftwarePainterNode(item) {
                setFlag(QSGNode::OwnedByParent);
            }

            void synchronize(const QRectF &bodyRect,
                             bool thumbnailDisplay,
                             const QColor &fillColor,
                             const QColor &borderColor,
                             const QColor &overlappedBorderColor) {
                if (m_bodyRect != bodyRect || m_thumbnailDisplay != thumbnailDisplay) {
                    m_bodyRect = bodyRect;
                    m_thumbnailDisplay = thumbnailDisplay;
                    if (thumbnailDisplay) {
                        m_borderPath = {};
                        m_fillPath = perimeterPath(bodyRect, CornerRadius);
                    } else {
                        m_borderPath = borderPath(bodyRect, CornerRadius, BorderWidth);
                        m_fillPath = perimeterPath(
                            bodyRect.adjusted(BorderWidth, BorderWidth, -BorderWidth, -BorderWidth),
                            std::max<qreal>(0, CornerRadius - BorderWidth));
                    }
                    setBoundingRect(bodyRect.adjusted(-1, -1, 1, 1));
                    markDirty(QSGNode::DirtyGeometry);
                }
                if (m_fillColor != fillColor
                    || m_borderColor != borderColor
                    || m_overlappedBorderColor != overlappedBorderColor) {
                    m_fillColor = fillColor;
                    m_borderColor = borderColor;
                    m_overlappedBorderColor = overlappedBorderColor;
                    markDirty(QSGNode::DirtyMaterial);
                }
            }

        protected:
            void paint(QPainter *painter) override {
                painter->setRenderHint(QPainter::Antialiasing);
                painter->setPen(Qt::NoPen);
                if (!m_thumbnailDisplay) {
                    painter->fillPath(m_borderPath, m_borderColor);
                }
                painter->fillPath(m_fillPath, m_fillColor);
                if (!m_thumbnailDisplay) {
                    painter->fillPath(m_borderPath, m_overlappedBorderColor);
                }
            }

        private:
            QRectF m_bodyRect;
            QPainterPath m_borderPath;
            QPainterPath m_fillPath;
            QColor m_fillColor;
            QColor m_borderColor;
            QColor m_overlappedBorderColor;
            bool m_thumbnailDisplay = false;
        };

        class NoteVisualNode : public QSGTransformNode {
        public:
            explicit NoteVisualNode(QQuickItem *) {
                setFlag(QSGNode::OwnedByParent, false);
                appendChildNode(m_borderNode = createGeometryNode());
                appendChildNode(m_fillNode = createGeometryNode());
                appendChildNode(m_overlappedBorderNode = createGeometryNode());
                appendChildNode(m_textNode = new ClippedTextVisualNode);
            }

            void synchronize(const NoteVisualState &state, QQuickWindow *window) {
                QMatrix4x4 matrix;
                matrix.translate(state.positionX, state.positionY);
                setMatrix(matrix);

                const qreal bodyHeight = state.thumbnailDisplay
                    ? ThumbnailBodyHeight
                    : std::max<qreal>(0, state.noteHeight - 2 * BodyInset);
                const QRectF bodyRect(
                    BodyInset,
                    0.5 * (state.noteHeight - bodyHeight),
                    std::max<qreal>(0, state.noteWidth - 2 * BodyInset),
                    bodyHeight);

                if (state.thumbnailDisplay) {
                    updateRoundedGeometry(m_borderNode, {}, 0, {});
                    updateRoundedGeometry(m_fillNode, bodyRect, CornerRadius, state.fillColor);
                    updateRoundedGeometry(m_overlappedBorderNode, {}, 0, {});
                } else {
                    updateRoundedBorderGeometry(m_borderNode, bodyRect, CornerRadius, BorderWidth, state.borderColor);
                    updateRoundedGeometry(
                        m_fillNode,
                        bodyRect.adjusted(BorderWidth, BorderWidth, -BorderWidth, -BorderWidth),
                        std::max<qreal>(0, CornerRadius - BorderWidth),
                        state.fillColor);
                    updateRoundedBorderGeometry(
                        m_overlappedBorderNode,
                        bodyRect,
                        CornerRadius,
                        BorderWidth,
                        state.overlappedBorderColor);
                }

                const qreal lyricLeft = BodyInset + LyricHorizontalMargin;
                const qreal lyricWidth = std::max<qreal>(0, state.noteWidth - 2 * lyricLeft);
                m_textNode->synchronize({
                    .text = state.thumbnailDisplay ? QString() : state.record.lyric,
                    .font = state.font,
                    .color = state.textColor,
                    .clipRect = QRectF(lyricLeft, 0, lyricWidth, state.noteHeight),
                    .left = lyricLeft,
                    .centerHeight = state.noteHeight,
                    .verticallyCentered = true,
                }, window);
            }

        private:
            QSGGeometryNode *m_borderNode = nullptr;
            QSGGeometryNode *m_fillNode = nullptr;
            QSGGeometryNode *m_overlappedBorderNode = nullptr;
            ClippedTextVisualNode *m_textNode = nullptr;
        };

        class SoftwareNoteVisualNode : public QSGTransformNode {
        public:
            explicit SoftwareNoteVisualNode(QQuickItem *item) {
                setFlag(QSGNode::OwnedByParent, false);
                appendChildNode(m_shapeNode = new NoteShapeSoftwareNode(item));
                appendChildNode(m_textNode = new ClippedTextVisualNode);
            }

            void synchronize(const NoteVisualState &state, QQuickWindow *window) {
                QMatrix4x4 matrix;
                matrix.translate(state.positionX, state.positionY);
                if (this->matrix() != matrix) {
                    setMatrix(matrix);
                }

                const qreal bodyHeight = state.thumbnailDisplay
                    ? ThumbnailBodyHeight
                    : std::max<qreal>(0, state.noteHeight - 2 * BodyInset);
                const QRectF bodyRect(
                    BodyInset,
                    0.5 * (state.noteHeight - bodyHeight),
                    std::max<qreal>(0, state.noteWidth - 2 * BodyInset),
                    bodyHeight);
                m_shapeNode->synchronize(bodyRect,
                                         state.thumbnailDisplay,
                                         state.fillColor,
                                         state.borderColor,
                                         state.overlappedBorderColor);

                const qreal lyricLeft = BodyInset + LyricHorizontalMargin;
                const qreal lyricWidth = std::max<qreal>(0, state.noteWidth - 2 * lyricLeft);
                m_textNode->synchronize({
                    .text = state.thumbnailDisplay ? QString() : state.record.lyric,
                    .font = state.font,
                    .color = state.textColor,
                    .clipRect = QRectF(lyricLeft, 0, lyricWidth, state.noteHeight),
                    .left = lyricLeft,
                    .centerHeight = state.noteHeight,
                    .verticallyCentered = true,
                }, window);
            }

        private:
            NoteShapeSoftwareNode *m_shapeNode = nullptr;
            ClippedTextVisualNode *m_textNode = nullptr;
        };

        struct AdditionalTextVisualState {
            QString text;
            QFont font;
            QColor color;
            qreal positionX = 0;
            qreal positionY = 0;
            qreal width = 0;
            qreal height = 0;
        };

        class AdditionalTextVisualNode : public QSGTransformNode {
        public:
            explicit AdditionalTextVisualNode(QQuickItem *) {
                setFlag(QSGNode::OwnedByParent, false);
                appendChildNode(m_textNode = new ClippedTextVisualNode);
            }

            void synchronize(const AdditionalTextVisualState &state, QQuickWindow *window) {
                QMatrix4x4 matrix;
                matrix.translate(state.positionX, state.positionY);
                setMatrix(matrix);
                m_textNode->synchronize({
                    .text = state.text,
                    .font = state.font,
                    .color = state.color,
                    .clipRect = QRectF(0, 0, state.width, state.height),
                    .left = AdditionalTextHorizontalPadding,
                    .top = AdditionalTextVerticalPadding,
                }, window);
            }

        private:
            ClippedTextVisualNode *m_textNode = nullptr;
        };

        template<typename Node, typename LayerNode>
        class LayeredNodeCollection {
        public:
            LayeredNodeCollection(QSGNode *parent, QQuickItem *item) : m_item(item) {
                for (auto &layer : m_layers) {
                    layer = new LayerNode;
                    layer->setFlag(QSGNode::OwnedByParent);
                    parent->appendChildNode(layer);
                }
            }

            ~LayeredNodeCollection() {
                for (auto &[model, visual] : m_activeNodes) {
                    Q_UNUSED(model);
                    if (visual->parent())
                        visual->parent()->removeChildNode(visual.get());
                }
            }

            void retain(const QSet<NoteViewModel *> &desiredItems, bool active) {
                for (auto it = m_activeNodes.begin(); it != m_activeNodes.end();) {
                    if (desiredItems.contains(it->first)) {
                        ++it;
                        continue;
                    }
                    if (it->second->parent())
                        it->second->parent()->removeChildNode(it->second.get());
                    if (std::ssize(m_pool) < NodePoolCapacity)
                        m_pool.push_back(std::move(it->second));
                    it = m_activeNodes.erase(it);
                }
                if (!active)
                    m_pool.clear();
            }

            void beginLayout() {
                m_nextIndices.fill(0);
            }

            Node *nodeFor(NoteViewModel *model) {
                auto activeIt = m_activeNodes.find(model);
                if (activeIt == m_activeNodes.end()) {
                    std::unique_ptr<Node> visual;
                    if (m_pool.empty()) {
                        visual = std::make_unique<Node>(m_item);
                    } else {
                        visual = std::move(m_pool.back());
                        m_pool.pop_back();
                    }
                    activeIt = m_activeNodes.emplace(model, std::move(visual)).first;
                }
                return activeIt->second.get();
            }

            void append(Node *node, int layer) {
                auto *targetLayer = m_layers[layer];
                const int targetIndex = m_nextIndices[layer]++;
                auto *currentNode = targetIndex < targetLayer->childCount()
                    ? targetLayer->childAtIndex(targetIndex)
                    : nullptr;
                if (currentNode == node) {
                    return;
                }
                if (node->parent()) {
                    node->parent()->removeChildNode(node);
                }
                if (currentNode) {
                    targetLayer->insertChildNodeBefore(node, currentNode);
                } else {
                    targetLayer->appendChildNode(node);
                }
            }

        private:
            std::array<QSGNode *, 3> m_layers{};
            std::array<int, 3> m_nextIndices{};
            std::unordered_map<NoteViewModel *, std::unique_ptr<Node>> m_activeNodes;
            std::vector<std::unique_ptr<Node>> m_pool;
            QQuickItem *m_item = nullptr;
        };

        template<typename NoteNode, typename LayerNode>
        class NoteEditLayerRootNode : public QSGNode {
        public:
            explicit NoteEditLayerRootNode(QQuickItem *item)
                : m_additionalTextNodes(this, item), m_noteNodes(this, item) {
            }

            void synchronize(NoteEditLayerQuickItemPrivate *d) {
                QSet<NoteViewModel *> desiredNoteItems;
                QSet<NoteViewModel *> desiredAdditionalTextItems;
                if (d->active && d->hasValidLayout()) {
                    desiredNoteItems = d->residentItems;
                    desiredNoteItems.remove(d->lyricEditingItem.data());
                    if (!d->thumbnailDisplay) {
                        for (auto *model : std::as_const(d->residentItems)) {
                            const auto it = d->records.constFind(model);
                            if (it != d->records.cend() && !it->additionalText.isEmpty()
                                && model != d->additionalTextEditingItem.data()
                                && (it->additionalTextHighlighted
                                        ? d->highlightedAdditionalTextColor
                                        : d->additionalTextColor).alpha() > 0) {
                                desiredAdditionalTextItems.insert(model);
                            }
                        }
                    }
                }
                m_additionalTextNodes.retain(desiredAdditionalTextItems, d->active);
                m_noteNodes.retain(desiredNoteItems, d->active);
                m_additionalTextNodes.beginLayout();
                m_noteNodes.beginLayout();

                QVector<const NoteEditLayerRecord *> sortedRecords;
                sortedRecords.reserve(d->residentItems.size());
                for (auto *model : std::as_const(d->residentItems)) {
                    const auto it = d->records.constFind(model);
                    if (it != d->records.cend())
                        sortedRecords.append(&it.value());
                }
                std::sort(sortedRecords.begin(), sortedRecords.end(), [d](const auto *left, const auto *right) {
                    const int leftLayer = d->visualLayer(*left);
                    const int rightLayer = d->visualLayer(*right);
                    if (leftLayer != rightLayer)
                        return leftLayer < rightLayer;
                    return left->insertionOrder < right->insertionOrder;
                });

                if (!d->active || !d->hasValidLayout())
                    return;

                const qreal timePixelDensity = d->timeLayoutViewModel->pixelDensity();
                const qreal clavierPixelDensity = d->clavierViewModel->pixelDensity();
                for (const auto *record : std::as_const(sortedRecords)) {
                    if (desiredAdditionalTextItems.contains(record->model)) {
                        auto *visual = m_additionalTextNodes.nodeFor(record->model);
                        const QRectF textRect = d->additionalTextRect(*record, true);
                        visual->synchronize({
                            .text = record->additionalText,
                            .font = d->font,
                            .color = record->additionalTextHighlighted
                                ? d->highlightedAdditionalTextColor
                                : d->additionalTextColor,
                            .positionX = textRect.x(),
                            .positionY = textRect.y(),
                            .width = textRect.width(),
                            .height = textRect.height(),
                        }, d->q_ptr->window());
                        m_additionalTextNodes.append(visual, d->visualLayer(*record));
                    }

                    if (!desiredNoteItems.contains(record->model))
                        continue;
                    auto *visual = m_noteNodes.nodeFor(record->model);

                    qreal fillOpacity = 1.0;
                    if (d->transparentDisplay)
                        fillOpacity *= TransparentFillOpacity;
                    if (d->thumbnailDisplay)
                        fillOpacity *= ThumbnailFillOpacity;
                    const QColor noteFillColor = record->selected ? d->selectedFillColor : d->fillColor;
                    const QColor borderColor = record->selected ? d->selectedBorderColor : d->fillColor;
                    const QColor overlappedBorderColor = record->overlapped ? d->overlappedBorderColor : QColor(Qt::transparent);
                    const QColor textColor = d->transparentDisplay
                        ? multiplyAlpha(d->textColor, TransparentTextOpacity)
                        : d->textColor;
                    NoteVisualState state {
                        .record = *record,
                        .positionX = record->position * timePixelDensity,
                        .positionY = (127 - record->key) * clavierPixelDensity,
                        .noteWidth = record->length * timePixelDensity,
                        .noteHeight = clavierPixelDensity,
                        .fillColor = multiplyAlpha(noteFillColor, fillOpacity),
                        .borderColor = borderColor,
                        .overlappedBorderColor = overlappedBorderColor,
                        .textColor = textColor,
                        .font = d->font,
                        .thumbnailDisplay = d->thumbnailDisplay,
                    };
                    visual->synchronize(state, d->q_ptr->window());
                    m_noteNodes.append(visual, d->visualLayer(*record));
                }
            }

        private:
            LayeredNodeCollection<AdditionalTextVisualNode, LayerNode> m_additionalTextNodes;
            LayeredNodeCollection<NoteNode, LayerNode> m_noteNodes;
        };

        using HardwareNoteEditLayerRootNode = NoteEditLayerRootNode<NoteVisualNode, QSGNode>;
        using SoftwareNoteEditLayerRootNode = NoteEditLayerRootNode<SoftwareNoteVisualNode, QSGTransformNode>;

        bool usesSoftwareRenderer(const QQuickItem *item) {
            return item->window()
                && item->window()->rendererInterface()->graphicsApi() == QSGRendererInterface::Software;
        }

        bool intervalIntersects(qreal start, qreal end, const NoteEditLayerRecord &record) {
            return record.position < end && record.position + std::max(record.length, 1) > start;
        }

        bool keyIntersects(qreal start, qreal end, const NoteEditLayerRecord &record) {
            return record.key >= 0 && record.key <= 127 && record.key < end && record.key + 1 > start;
        }

    }

    NoteEditLayerInterval::NoteEditLayerInterval() : interval(0, 0) {
    }

    NoteEditLayerInterval::NoteEditLayerInterval(int position, int length, NoteViewModel *model)
        : interval(position, position + std::max(length, 1) - 1), model(model) {
    }

    bool NoteEditLayerInterval::operator==(const NoteEditLayerInterval &other) const {
        return model == other.model;
    }

    NoteEditLayerMarkerModel::NoteEditLayerMarkerModel(QObject *parent) : QAbstractListModel(parent) {
    }

    int NoteEditLayerMarkerModel::rowCount(const QModelIndex &parent) const {
        return parent.isValid() ? 0 : m_markers.size();
    }

    QVariant NoteEditLayerMarkerModel::data(const QModelIndex &index, int role) const {
        if (!index.isValid() || index.row() < 0 || index.row() >= m_markers.size())
            return {};
        const auto &marker = m_markers.at(index.row());
        switch (role) {
            case ModelObjectRole:
                return QVariant::fromValue(static_cast<QObject *>(marker.model.data()));
            case MarkerTypeRole:
                return static_cast<int>(marker.type);
            case CenterXRole:
                return marker.center.x();
            case CenterYRole:
                return marker.center.y();
            case SelectedRole:
                return marker.selected;
            default:
                return {};
        }
    }

    QHash<int, QByteArray> NoteEditLayerMarkerModel::roleNames() const {
        return {
            {ModelObjectRole, "modelObject"},
            {MarkerTypeRole, "markerType"},
            {CenterXRole, "centerX"},
            {CenterYRole, "centerY"},
            {SelectedRole, "selected"},
        };
    }

    void NoteEditLayerMarkerModel::setMarkers(QVector<NoteEditLayerMarker> markers) {
        bool sameRows = markers.size() == m_markers.size();
        if (sameRows) {
            for (qsizetype index = 0; index < markers.size(); ++index) {
                if (markers[index].model != m_markers[index].model || markers[index].type != m_markers[index].type) {
                    sameRows = false;
                    break;
                }
            }
        }
        if (!sameRows) {
            beginResetModel();
            m_markers = std::move(markers);
            endResetModel();
            return;
        }
        if (markers == m_markers)
            return;
        m_markers = std::move(markers);
        if (!m_markers.isEmpty()) {
            Q_EMIT dataChanged(
                index(0), index(m_markers.size() - 1),
                {CenterXRole, CenterYRole, SelectedRole});
        }
    }

    void NoteEditLayerMarkerModel::clear() {
        if (m_markers.isEmpty())
            return;
        beginResetModel();
        m_markers.clear();
        endResetModel();
    }

    NoteEditLayerQuickItemPrivate::NoteEditLayerQuickItemPrivate(NoteEditLayerQuickItem *q)
        : q_ptr(q), markerModel(nullptr) {
    }

    NoteEditLayerQuickItemPrivate::~NoteEditLayerQuickItemPrivate() {
        if (rubberBandSelector) {
            for (auto *model : std::as_const(selectorItems))
                rubberBandSelector->removeItem(model);
        }
        for (const auto &connections : std::as_const(recordConnections)) {
            for (const auto &connection : connections)
                QObject::disconnect(connection);
        }
    }

    void NoteEditLayerQuickItemPrivate::resetNoteSequenceViewModel(RangeSequenceViewModel *viewModel) {
        Q_Q(NoteEditLayerQuickItem);
        if (noteSequenceViewModel == viewModel)
            return;
        if (noteSequenceViewModel)
            QObject::disconnect(noteSequenceViewModel, nullptr, q, nullptr);
        detachAllItems();
        noteSequenceViewModel = viewModel;
        if (noteSequenceViewModel) {
            QObject::connect(noteSequenceViewModel, &RangeSequenceViewModel::itemInserted, q, [this](QObject *item) {
                if (auto *note = qobject_cast<NoteViewModel *>(item))
                    attachItem(note);
            });
            QObject::connect(noteSequenceViewModel, &RangeSequenceViewModel::itemRemoved, q, [this](QObject *item) {
                if (auto *note = qobject_cast<NoteViewModel *>(item))
                    detachItem(note);
            });
            QObject::connect(noteSequenceViewModel, &QObject::destroyed, q, [this] {
                Q_Q(NoteEditLayerQuickItem);
                noteSequenceViewModel = nullptr;
                detachAllItems();
                updateViewport(true, true);
                Q_EMIT q->noteSequenceViewModelChanged();
            });
            resettingSequence = true;
            for (auto *item : noteSequenceViewModel->items()) {
                if (auto *note = qobject_cast<NoteViewModel *>(item))
                    attachItem(note);
            }
            resettingSequence = false;
        }
        residentRangeValid = false;
        updateViewport(true, true);
    }

    void NoteEditLayerQuickItemPrivate::attachItem(NoteViewModel *model) {
        Q_Q(NoteEditLayerQuickItem);
        if (!model || records.contains(model))
            return;
        NoteEditLayerRecord record {
            .model = model,
            .lyric = model->lyric(),
            .additionalText = model->additionalText(),
            .position = model->position(),
            .length = model->length(),
            .key = model->key(),
            .nextNotePosition = model->nextNotePosition(),
            .nextNoteKey = model->nextNoteKey(),
            .selected = model->isSelected(),
            .overlapped = model->isOverlapped(),
            .additionalTextHighlighted = model->isAdditionalTextHighlighted(),
            .insertionOrder = ++nextInsertionOrder,
        };
        records.insert(model, record);
        insertIntoIndex(record);
        updateSelectorRegistration(model);

        auto &connections = recordConnections[model];
        connections.reserve(11);
        connections.append(QObject::connect(model, &NoteViewModel::positionChanged, q, [this, model] { updateRecord(model, true, true); }));
        connections.append(QObject::connect(model, &NoteViewModel::lengthChanged, q, [this, model] { updateRecord(model, true, true); }));
        connections.append(QObject::connect(model, &NoteViewModel::keyChanged, q, [this, model] { updateRecord(model, true, true); }));
        connections.append(QObject::connect(model, &NoteViewModel::lyricChanged, q, [this, model] { updateRecord(model, false, false); }));
        connections.append(QObject::connect(model, &NoteViewModel::additionalTextChanged, q, [this, model] { updateRecord(model, true, false); }));
        connections.append(QObject::connect(model, &NoteViewModel::additionalTextHighlightedChanged, q, [this, model] { updateRecord(model, false, false); }));
        connections.append(QObject::connect(model, &NoteViewModel::selectedChanged, q, [this, model] { updateRecord(model, false, false); }));
        connections.append(QObject::connect(model, &NoteViewModel::overlappedChanged, q, [this, model] { updateRecord(model, false, false); }));
        connections.append(QObject::connect(model, &NoteViewModel::nextNotePositionChanged, q, [this, model] { updateRecord(model, false, false); }));
        connections.append(QObject::connect(model, &NoteViewModel::nextNoteKeyChanged, q, [this, model] { updateRecord(model, false, false); }));
        connections.append(QObject::connect(model, &QObject::destroyed, q, [this, model] { detachItem(model); }));

        if (resettingSequence)
            return;
        if (!residentRangeValid)
            updateViewport(true, false);
        else if (active && keyIntersects(residentKeyStart, residentKeyEnd, record)
                 && intervalIntersects(residentPositionStart, residentPositionEnd, record)) {
            residentItems.insert(model);
            scheduleMarkerRebuild();
            q->update();
        }
    }

    void NoteEditLayerQuickItemPrivate::detachItem(NoteViewModel *model) {
        Q_Q(NoteEditLayerQuickItem);
        const auto it = records.find(model);
        if (it == records.end())
            return;
        const NoteEditLayerRecord record = it.value();
        const bool wasResident = residentItems.remove(model);
        removeFromIndex(record);
        removeSelectorRegistration(model);
        for (const auto &connection : recordConnections.take(model))
            QObject::disconnect(connection);
        records.erase(it);
        bumpGeometryRevision();
        if (wasResident) {
            scheduleMarkerRebuild();
            q->update();
        }
    }

    void NoteEditLayerQuickItemPrivate::detachAllItems() {
        Q_Q(NoteEditLayerQuickItem);
        if (rubberBandSelector) {
            for (auto *model : std::as_const(selectorItems))
                rubberBandSelector->removeItem(model);
        }
        for (const auto &connections : std::as_const(recordConnections)) {
            for (const auto &connection : connections)
                QObject::disconnect(connection);
        }
        for (auto &tree : intervalTrees)
            tree.clear();
        records.clear();
        recordConnections.clear();
        residentItems.clear();
        selectorItems.clear();
        markerModel.clear();
        residentRangeValid = false;
        bumpGeometryRevision();
        q->update();
    }

    void NoteEditLayerQuickItemPrivate::updateRecord(NoteViewModel *model, bool geometryChanged, bool selectorGeometryChanged) {
        Q_Q(NoteEditLayerQuickItem);
        auto it = records.find(model);
        if (it == records.end() || !model)
            return;
        const NoteEditLayerRecord oldRecord = it.value();
        NoteEditLayerRecord &record = it.value();
        record.lyric = model->lyric();
        record.additionalText = model->additionalText();
        record.position = model->position();
        record.length = model->length();
        record.key = model->key();
        record.nextNotePosition = model->nextNotePosition();
        record.nextNoteKey = model->nextNoteKey();
        record.selected = model->isSelected();
        record.overlapped = model->isOverlapped();
        record.additionalTextHighlighted = model->isAdditionalTextHighlighted();

        if (oldRecord.additionalText != record.additionalText)
            record.additionalTextMetricsRevision = 0;

        const bool indexChanged = oldRecord.position != record.position || oldRecord.length != record.length || oldRecord.key != record.key;
        if (indexChanged) {
            removeFromIndex(oldRecord);
            insertIntoIndex(record);
        }
        if (selectorGeometryChanged && indexChanged)
            updateSelectorRegistration(model);

        const bool wasResident = residentItems.contains(model);
        const bool shouldBeResident = active && residentRangeValid
            && keyIntersects(residentKeyStart, residentKeyEnd, record)
            && intervalIntersects(residentPositionStart, residentPositionEnd, record);
        if (shouldBeResident)
            residentItems.insert(model);
        else
            residentItems.remove(model);

        if (geometryChanged)
            bumpGeometryRevision();
        if (wasResident || shouldBeResident) {
            scheduleMarkerRebuild();
            q->update();
        }
    }

    void NoteEditLayerQuickItemPrivate::insertIntoIndex(const NoteEditLayerRecord &record) {
        if (record.key < 0 || record.key >= static_cast<int>(intervalTrees.size()) || !record.model)
            return;
        intervalTrees[record.key].insert(NoteEditLayerInterval(record.position, record.length, record.model));
    }

    void NoteEditLayerQuickItemPrivate::removeFromIndex(const NoteEditLayerRecord &record) {
        if (record.key < 0 || record.key >= static_cast<int>(intervalTrees.size()) || !record.model)
            return;
        auto &tree = intervalTrees[record.key];
        const NoteEditLayerInterval interval(record.position, record.length, record.model);
        const auto it = tree.find(interval);
        if (it != tree.end())
            tree.erase(it);
    }

    QSet<NoteViewModel *> NoteEditLayerQuickItemPrivate::queryIndex(int minimumKey, int maximumKey, int position, int length) const {
        QSet<NoteViewModel *> result;
        const NoteEditLayerInterval query(position, length, nullptr);
        for (int key = std::max(0, minimumKey); key <= std::min(127, maximumKey); ++key) {
            intervalTrees[key].overlap_find_all(query, [&result](const auto &it) {
                if (auto *model = it.interval().model)
                    result.insert(model);
                return true;
            });
        }
        return result;
    }

    void NoteEditLayerQuickItemPrivate::updateViewport(bool forceResidentRange, bool geometryChanged) {
        Q_Q(NoteEditLayerQuickItem);
        if (!active || !hasValidLayout() || viewportWidth <= 0 || viewportHeight <= 0) {
            residentRangeValid = false;
            clearResidentItems();
            if (geometryChanged)
                bumpGeometryRevision();
            return;
        }

        const qreal timePixelDensity = timeLayoutViewModel->pixelDensity();
        const qreal clavierPixelDensity = clavierViewModel->pixelDensity();
        const qreal visiblePositionStart = timeViewModel->start();
        const qreal visiblePositionLength = viewportWidth / timePixelDensity;
        const qreal visiblePositionEnd = visiblePositionStart + visiblePositionLength;
        const qreal visibleKeyEnd = clavierViewModel->start();
        const qreal visibleKeyLength = viewportHeight / clavierPixelDensity;
        const qreal visibleKeyStart = visibleKeyEnd - visibleKeyLength;
        const qreal additionalTextKeyLength = thumbnailDisplay ? 0 : additionalTextHeight() / clavierPixelDensity;
        const qreal requiredKeyStart = visibleKeyStart - (additionalTextAbove ? additionalTextKeyLength : 0);
        const qreal requiredKeyEnd = visibleKeyEnd + (additionalTextAbove ? 0 : additionalTextKeyLength);

        if (!forceResidentRange && residentRangeValid
            && visiblePositionStart >= residentPositionStart && visiblePositionEnd <= residentPositionEnd
            && requiredKeyStart >= residentKeyStart && requiredKeyEnd <= residentKeyEnd) {
            if (geometryChanged) {
                scheduleMarkerRebuild();
                bumpGeometryRevision();
                q->update();
            }
            return;
        }

        residentPositionStart = visiblePositionStart - 0.5 * visiblePositionLength;
        residentPositionEnd = visiblePositionEnd + 0.5 * visiblePositionLength;
        residentKeyStart = requiredKeyStart - 0.5 * visibleKeyLength;
        residentKeyEnd = requiredKeyEnd + 0.5 * visibleKeyLength;
        residentRangeValid = true;

        const int queryPosition = static_cast<int>(std::floor(residentPositionStart));
        const int queryEnd = static_cast<int>(std::ceil(residentPositionEnd));
        const int queryLength = std::max(1, queryEnd - queryPosition);
        const int minimumKey = static_cast<int>(std::floor(residentKeyStart));
        const int maximumKey = static_cast<int>(std::ceil(residentKeyEnd)) - 1;
        residentItems = queryIndex(minimumKey, maximumKey, queryPosition, queryLength);
        rebuildMarkers();
        if (geometryChanged)
            bumpGeometryRevision();
        q->update();
    }

    void NoteEditLayerQuickItemPrivate::clearResidentItems() {
        Q_Q(NoteEditLayerQuickItem);
        if (residentItems.isEmpty() && markerModel.rowCount() == 0)
            return;
        residentItems.clear();
        markerModel.clear();
        q->update();
    }

    void NoteEditLayerQuickItemPrivate::scheduleResidentTrim() {
        const quint64 generation = ++residentTrimGeneration;
        QTimer::singleShot(ResidentTrimDelay, q_ptr, [this, generation] {
            if (generation == residentTrimGeneration)
                updateViewport(true, false);
        });
    }

    void NoteEditLayerQuickItemPrivate::scheduleMarkerRebuild() {
        if (markerRebuildPending)
            return;
        markerRebuildPending = true;
        QTimer::singleShot(0, q_ptr, [this] {
            markerRebuildPending = false;
            rebuildMarkers();
        });
    }

    void NoteEditLayerQuickItemPrivate::rebuildMarkers() {
        if (!active || !hasValidLayout()) {
            markerModel.clear();
            return;
        }
        QVector<const NoteEditLayerRecord *> sortedRecords;
        sortedRecords.reserve(residentItems.size());
        for (auto *model : std::as_const(residentItems)) {
            const auto it = records.constFind(model);
            if (it != records.cend())
                sortedRecords.append(&it.value());
        }
        std::sort(sortedRecords.begin(), sortedRecords.end(), [](const auto *left, const auto *right) {
            return left->insertionOrder < right->insertionOrder;
        });

        const qreal timePixelDensity = timeLayoutViewModel->pixelDensity();
        const qreal clavierPixelDensity = clavierViewModel->pixelDensity();
        QVector<NoteEditLayerMarker> markers;
        for (const auto *record : std::as_const(sortedRecords)) {
            const qreal noteY = (127 - record->key) * clavierPixelDensity;
            if (record->length <= shortNoteThreshold) {
                markers.append({
                    .model = record->model,
                    .type = NoteEditLayerQuickItem::ShortNoteMarker,
                    .center = QPointF(
                        (record->position + 0.5 * record->length) * timePixelDensity,
                        additionalTextAbove ? noteY + clavierPixelDensity + MarkerOffset : noteY - MarkerOffset),
                    .selected = record->selected,
                });
            }

            const int restLength = record->nextNotePosition - record->position - record->length;
            if (restLength <= 0 || restLength > shortNoteThreshold)
                continue;
            const qreal nextNoteY = (127 - record->nextNoteKey) * clavierPixelDensity;
            markers.append({
                .model = record->model,
                .type = NoteEditLayerQuickItem::ShortRestMarker,
                .center = QPointF(
                    (record->position + record->length + 0.5 * restLength) * timePixelDensity,
                    additionalTextAbove
                        ? std::min(noteY, nextNoteY) - MarkerOffset
                        : std::max(noteY, nextNoteY) + clavierPixelDensity + MarkerOffset),
                .selected = record->selected,
            });
        }
        markerModel.setMarkers(std::move(markers));
    }

    void NoteEditLayerQuickItemPrivate::updateSelectorRegistration(NoteViewModel *model) {
        if (!rubberBandSelector || !active || !model)
            return;
        const auto it = records.constFind(model);
        if (it == records.cend())
            return;
        const auto &record = it.value();
        rubberBandSelector->insertItem(model, QRectF(record.position, record.key, record.length, 1));
        selectorItems.insert(model);
    }

    void NoteEditLayerQuickItemPrivate::removeSelectorRegistration(NoteViewModel *model) {
        if (!selectorItems.remove(model) || !rubberBandSelector)
            return;
        rubberBandSelector->removeItem(model);
    }

    void NoteEditLayerQuickItemPrivate::resetSelectorRegistrations() {
        if (rubberBandSelector) {
            for (auto *model : std::as_const(selectorItems))
                rubberBandSelector->removeItem(model);
        }
        selectorItems.clear();
        if (!rubberBandSelector || !active)
            return;
        for (auto *model : records.keys())
            updateSelectorRegistration(model);
    }

    void NoteEditLayerQuickItemPrivate::bumpGeometryRevision() {
        Q_Q(NoteEditLayerQuickItem);
        if (geometryRevisionPending)
            return;
        geometryRevisionPending = true;
        QTimer::singleShot(0, q, [this] {
            Q_Q(NoteEditLayerQuickItem);
            geometryRevisionPending = false;
            ++geometryRevision;
            Q_EMIT q->geometryRevisionChanged();
        });
    }

    bool NoteEditLayerQuickItemPrivate::hasValidLayout() const {
        return timeViewModel && timeLayoutViewModel && clavierViewModel
            && timeLayoutViewModel->pixelDensity() > 0 && clavierViewModel->pixelDensity() > 0;
    }

    QRectF NoteEditLayerQuickItemPrivate::itemRect(const NoteEditLayerRecord &record) const {
        if (!hasValidLayout())
            return {};
        return {
            record.position * timeLayoutViewModel->pixelDensity(),
            (127 - record.key) * clavierViewModel->pixelDensity(),
            record.length * timeLayoutViewModel->pixelDensity(),
            clavierViewModel->pixelDensity(),
        };
    }

    QSizeF NoteEditLayerQuickItemPrivate::additionalTextSize(const NoteEditLayerRecord &record) const {
        if (record.additionalTextMetricsRevision != fontRevision) {
            record.additionalTextSize = measureSingleLineText(record.additionalText, font);
            record.additionalTextMetricsRevision = fontRevision;
        }
        return record.additionalTextSize;
    }

    qreal NoteEditLayerQuickItemPrivate::additionalTextHeight() const {
        if (additionalTextLineHeightRevision != fontRevision) {
            additionalTextLineHeight = measureSingleLineText({}, font).height();
            additionalTextLineHeightRevision = fontRevision;
        }
        return additionalTextLineHeight + 2 * AdditionalTextVerticalPadding;
    }

    QRectF NoteEditLayerQuickItemPrivate::additionalTextRect(const NoteEditLayerRecord &record, bool clipped) const {
        if (!hasValidLayout())
            return {};
        const QSizeF textSize = additionalTextSize(record);
        const qreal fullWidth = textSize.width() + 2 * AdditionalTextHorizontalPadding;
        const qreal height = textSize.height() + 2 * AdditionalTextVerticalPadding;
        const qreal noteX = record.position * timeLayoutViewModel->pixelDensity();
        const qreal noteY = (127 - record.key) * clavierViewModel->pixelDensity();
        const qreal noteWidth = std::max<qreal>(0, record.length * timeLayoutViewModel->pixelDensity());
        return {
            noteX,
            additionalTextAbove ? noteY - height : noteY + clavierViewModel->pixelDensity(),
            clipped ? std::min(noteWidth, fullWidth) : fullWidth,
            height,
        };
    }

    int NoteEditLayerQuickItemPrivate::visualLayer(const NoteEditLayerRecord &record) const {
        if (record.selected && selectionController && selectionController->currentItem() == record.model)
            return 2;
        return record.selected ? 1 : 0;
    }

    const NoteEditLayerRecord *NoteEditLayerQuickItemPrivate::hitTestRecord(const QPointF &point) const {
        if (!active || !hasValidLayout())
            return nullptr;

        const qreal timePixelDensity = timeLayoutViewModel->pixelDensity();
        const qreal clavierPixelDensity = clavierViewModel->pixelDensity();
        const int key = 127 - static_cast<int>(std::floor(point.y() / clavierPixelDensity));
        if (key < 0 || key > 127)
            return nullptr;

        const int position = static_cast<int>(std::floor(point.x() / timePixelDensity));
        const NoteEditLayerInterval query(position, 1, nullptr);
        const NoteEditLayerRecord *targetRecord = nullptr;
        intervalTrees[key].overlap_find_all(query, [this, point, &targetRecord](const auto &intervalIterator) {
            const auto it = records.constFind(intervalIterator.interval().model);
            if (it == records.cend())
                return true;
            const QRectF rect = itemRect(it.value());
            if (rect.width() <= 0 || point.x() < rect.left() || point.x() >= rect.right()
                || point.y() < rect.top() || point.y() >= rect.bottom()) {
                return true;
            }
            if (!targetRecord || visualLayer(it.value()) > visualLayer(*targetRecord)
                || (visualLayer(it.value()) == visualLayer(*targetRecord)
                    && it->insertionOrder > targetRecord->insertionOrder)) {
                targetRecord = &it.value();
            }
            return true;
        });
        return targetRecord;
    }

    const NoteEditLayerRecord *NoteEditLayerQuickItemPrivate::hitTestAdditionalTextRecord(const QPointF &point) const {
        if (!active || thumbnailDisplay || !hasValidLayout())
            return nullptr;

        const qreal timePixelDensity = timeLayoutViewModel->pixelDensity();
        const qreal clavierPixelDensity = clavierViewModel->pixelDensity();
        const qreal boxHeight = additionalTextHeight();
        const qreal minimumNoteY = additionalTextAbove
            ? point.y()
            : point.y() - clavierPixelDensity - boxHeight;
        const qreal maximumNoteY = additionalTextAbove
            ? point.y() + boxHeight
            : point.y() - clavierPixelDensity;
        const int minimumRow = std::max(0, static_cast<int>(std::floor(minimumNoteY / clavierPixelDensity)) - 1);
        const int maximumRow = std::min(127, static_cast<int>(std::ceil(maximumNoteY / clavierPixelDensity)) + 1);
        const int position = static_cast<int>(std::floor(point.x() / timePixelDensity));
        const NoteEditLayerInterval query(position, 1, nullptr);
        const NoteEditLayerRecord *targetRecord = nullptr;
        for (int row = minimumRow; row <= maximumRow; ++row) {
            const int key = 127 - row;
            intervalTrees[key].overlap_find_all(query, [this, point, &targetRecord](const auto &intervalIterator) {
                const auto it = records.constFind(intervalIterator.interval().model);
                if (it == records.cend() || it->model == additionalTextEditingItem.data())
                    return true;
                const QRectF rect = additionalTextRect(it.value(), true);
                if (rect.width() <= 0 || rect.height() <= 0
                    || point.x() < rect.left() || point.x() >= rect.right()
                    || point.y() < rect.top() || point.y() >= rect.bottom()) {
                    return true;
                }
                if (!targetRecord || visualLayer(it.value()) > visualLayer(*targetRecord)
                    || (visualLayer(it.value()) == visualLayer(*targetRecord)
                        && it->insertionOrder > targetRecord->insertionOrder)) {
                    targetRecord = &it.value();
                }
                return true;
            });
        }
        return targetRecord;
    }

    NoteEditLayerQuickItem::NoteEditLayerQuickItem(QQuickItem *parent)
        : QQuickItem(parent), d_ptr(new NoteEditLayerQuickItemPrivate(this)) {
        setFlag(ItemHasContents, true);
    }

    NoteEditLayerQuickItem::~NoteEditLayerQuickItem() = default;

    RangeSequenceViewModel *NoteEditLayerQuickItem::noteSequenceViewModel() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->noteSequenceViewModel;
    }

    void NoteEditLayerQuickItem::setNoteSequenceViewModel(RangeSequenceViewModel *noteSequenceViewModel) {
        Q_D(NoteEditLayerQuickItem);
        if (d->noteSequenceViewModel == noteSequenceViewModel)
            return;
        d->resetNoteSequenceViewModel(noteSequenceViewModel);
        Q_EMIT noteSequenceViewModelChanged();
    }

    TimeViewModel *NoteEditLayerQuickItem::timeViewModel() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->timeViewModel;
    }

    void NoteEditLayerQuickItem::setTimeViewModel(TimeViewModel *timeViewModel) {
        Q_D(NoteEditLayerQuickItem);
        if (d->timeViewModel == timeViewModel)
            return;
        if (d->timeViewModel)
            disconnect(d->timeViewModel, nullptr, this, nullptr);
        d->timeViewModel = timeViewModel;
        if (d->timeViewModel) {
            connect(d->timeViewModel, &TimeViewModel::startChanged, this, [d] { d->updateViewport(false, false); });
            connect(d->timeViewModel, &QObject::destroyed, this, [this] {
                Q_D(NoteEditLayerQuickItem);
                d->timeViewModel = nullptr;
                d->residentRangeValid = false;
                d->updateViewport(true, true);
                Q_EMIT timeViewModelChanged();
            });
        }
        d->residentRangeValid = false;
        d->updateViewport(true, true);
        Q_EMIT timeViewModelChanged();
    }

    TimeLayoutViewModel *NoteEditLayerQuickItem::timeLayoutViewModel() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->timeLayoutViewModel;
    }

    void NoteEditLayerQuickItem::setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel) {
        Q_D(NoteEditLayerQuickItem);
        if (d->timeLayoutViewModel == timeLayoutViewModel)
            return;
        if (d->timeLayoutViewModel)
            disconnect(d->timeLayoutViewModel, nullptr, this, nullptr);
        d->timeLayoutViewModel = timeLayoutViewModel;
        if (d->timeLayoutViewModel) {
            connect(d->timeLayoutViewModel, &TimeLayoutViewModel::pixelDensityChanged, this, [d] {
                d->updateViewport(false, true);
                d->scheduleResidentTrim();
            });
            connect(d->timeLayoutViewModel, &QObject::destroyed, this, [this] {
                Q_D(NoteEditLayerQuickItem);
                d->timeLayoutViewModel = nullptr;
                d->residentRangeValid = false;
                d->updateViewport(true, true);
                Q_EMIT timeLayoutViewModelChanged();
            });
        }
        d->residentRangeValid = false;
        d->updateViewport(true, true);
        Q_EMIT timeLayoutViewModelChanged();
    }

    ClavierViewModel *NoteEditLayerQuickItem::clavierViewModel() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->clavierViewModel;
    }

    void NoteEditLayerQuickItem::setClavierViewModel(ClavierViewModel *clavierViewModel) {
        Q_D(NoteEditLayerQuickItem);
        if (d->clavierViewModel == clavierViewModel)
            return;
        if (d->clavierViewModel)
            disconnect(d->clavierViewModel, nullptr, this, nullptr);
        d->clavierViewModel = clavierViewModel;
        if (d->clavierViewModel) {
            connect(d->clavierViewModel, &ClavierViewModel::startChanged, this, [d] { d->updateViewport(false, false); });
            connect(d->clavierViewModel, &ClavierViewModel::pixelDensityChanged, this, [d] {
                d->updateViewport(false, true);
                d->scheduleResidentTrim();
            });
            connect(d->clavierViewModel, &QObject::destroyed, this, [this] {
                Q_D(NoteEditLayerQuickItem);
                d->clavierViewModel = nullptr;
                d->residentRangeValid = false;
                d->updateViewport(true, true);
                Q_EMIT clavierViewModelChanged();
            });
        }
        d->residentRangeValid = false;
        d->updateViewport(true, true);
        Q_EMIT clavierViewModelChanged();
    }

    SelectionController *NoteEditLayerQuickItem::selectionController() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->selectionController;
    }

    void NoteEditLayerQuickItem::setSelectionController(SelectionController *selectionController) {
        Q_D(NoteEditLayerQuickItem);
        if (d->selectionController == selectionController)
            return;
        if (d->selectionController)
            disconnect(d->selectionController, nullptr, this, nullptr);
        d->selectionController = selectionController;
        if (d->selectionController) {
            connect(d->selectionController, &SelectionController::currentItemChanged, this, &QQuickItem::update);
            connect(d->selectionController, &QObject::destroyed, this, [this] {
                Q_D(NoteEditLayerQuickItem);
                d->selectionController = nullptr;
                update();
                Q_EMIT selectionControllerChanged();
            });
        }
        update();
        Q_EMIT selectionControllerChanged();
    }

    RubberBandSelector *NoteEditLayerQuickItem::rubberBandSelector() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->rubberBandSelector;
    }

    void NoteEditLayerQuickItem::setRubberBandSelector(RubberBandSelector *rubberBandSelector) {
        Q_D(NoteEditLayerQuickItem);
        if (d->rubberBandSelector == rubberBandSelector)
            return;
        if (d->rubberBandSelector) {
            for (auto *model : std::as_const(d->selectorItems))
                d->rubberBandSelector->removeItem(model);
            disconnect(d->rubberBandSelector, nullptr, this, nullptr);
        }
        d->selectorItems.clear();
        d->rubberBandSelector = rubberBandSelector;
        if (d->rubberBandSelector) {
            connect(d->rubberBandSelector, &QObject::destroyed, this, [this] {
                Q_D(NoteEditLayerQuickItem);
                d->rubberBandSelector = nullptr;
                d->selectorItems.clear();
                Q_EMIT rubberBandSelectorChanged();
            });
        }
        d->resetSelectorRegistrations();
        Q_EMIT rubberBandSelectorChanged();
    }

    QColor NoteEditLayerQuickItem::fillColor() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->fillColor;
    }

    void NoteEditLayerQuickItem::setFillColor(const QColor &fillColor) {
        Q_D(NoteEditLayerQuickItem);
        if (d->fillColor == fillColor)
            return;
        d->fillColor = fillColor;
        update();
        Q_EMIT fillColorChanged();
    }

    QColor NoteEditLayerQuickItem::selectedFillColor() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->selectedFillColor;
    }

    void NoteEditLayerQuickItem::setSelectedFillColor(const QColor &selectedFillColor) {
        Q_D(NoteEditLayerQuickItem);
        if (d->selectedFillColor == selectedFillColor)
            return;
        d->selectedFillColor = selectedFillColor;
        update();
        Q_EMIT selectedFillColorChanged();
    }

    QColor NoteEditLayerQuickItem::selectedBorderColor() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->selectedBorderColor;
    }

    void NoteEditLayerQuickItem::setSelectedBorderColor(const QColor &selectedBorderColor) {
        Q_D(NoteEditLayerQuickItem);
        if (d->selectedBorderColor == selectedBorderColor)
            return;
        d->selectedBorderColor = selectedBorderColor;
        update();
        Q_EMIT selectedBorderColorChanged();
    }

    QColor NoteEditLayerQuickItem::overlappedBorderColor() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->overlappedBorderColor;
    }

    void NoteEditLayerQuickItem::setOverlappedBorderColor(const QColor &overlappedBorderColor) {
        Q_D(NoteEditLayerQuickItem);
        if (d->overlappedBorderColor == overlappedBorderColor)
            return;
        d->overlappedBorderColor = overlappedBorderColor;
        update();
        Q_EMIT overlappedBorderColorChanged();
    }

    QColor NoteEditLayerQuickItem::textColor() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->textColor;
    }

    void NoteEditLayerQuickItem::setTextColor(const QColor &textColor) {
        Q_D(NoteEditLayerQuickItem);
        if (d->textColor == textColor)
            return;
        d->textColor = textColor;
        update();
        Q_EMIT textColorChanged();
    }

    QColor NoteEditLayerQuickItem::additionalTextColor() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->additionalTextColor;
    }

    void NoteEditLayerQuickItem::setAdditionalTextColor(const QColor &additionalTextColor) {
        Q_D(NoteEditLayerQuickItem);
        if (d->additionalTextColor == additionalTextColor)
            return;
        d->additionalTextColor = additionalTextColor;
        update();
        Q_EMIT additionalTextColorChanged();
    }

    QColor NoteEditLayerQuickItem::highlightedAdditionalTextColor() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->highlightedAdditionalTextColor;
    }

    void NoteEditLayerQuickItem::setHighlightedAdditionalTextColor(const QColor &highlightedAdditionalTextColor) {
        Q_D(NoteEditLayerQuickItem);
        if (d->highlightedAdditionalTextColor == highlightedAdditionalTextColor)
            return;
        d->highlightedAdditionalTextColor = highlightedAdditionalTextColor;
        update();
        Q_EMIT highlightedAdditionalTextColorChanged();
    }

    QFont NoteEditLayerQuickItem::font() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->font;
    }

    void NoteEditLayerQuickItem::setFont(const QFont &font) {
        Q_D(NoteEditLayerQuickItem);
        if (d->font == font)
            return;
        d->font = font;
        ++d->fontRevision;
        d->residentRangeValid = false;
        d->updateViewport(true, true);
        Q_EMIT fontChanged();
    }

    bool NoteEditLayerQuickItem::isActive() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->active;
    }

    void NoteEditLayerQuickItem::setActive(bool active) {
        Q_D(NoteEditLayerQuickItem);
        if (d->active == active)
            return;
        d->active = active;
        d->residentRangeValid = false;
        d->resetSelectorRegistrations();
        d->updateViewport(true, false);
        Q_EMIT activeChanged();
    }

    bool NoteEditLayerQuickItem::isTransparentDisplay() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->transparentDisplay;
    }

    void NoteEditLayerQuickItem::setTransparentDisplay(bool transparentDisplay) {
        Q_D(NoteEditLayerQuickItem);
        if (d->transparentDisplay == transparentDisplay)
            return;
        d->transparentDisplay = transparentDisplay;
        update();
        Q_EMIT transparentDisplayChanged();
    }

    bool NoteEditLayerQuickItem::isThumbnailDisplay() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->thumbnailDisplay;
    }

    void NoteEditLayerQuickItem::setThumbnailDisplay(bool thumbnailDisplay) {
        Q_D(NoteEditLayerQuickItem);
        if (d->thumbnailDisplay == thumbnailDisplay)
            return;
        d->thumbnailDisplay = thumbnailDisplay;
        d->residentRangeValid = false;
        d->updateViewport(true, false);
        Q_EMIT thumbnailDisplayChanged();
    }

    bool NoteEditLayerQuickItem::isEditScopeFocused() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->editScopeFocused;
    }

    void NoteEditLayerQuickItem::setEditScopeFocused(bool editScopeFocused) {
        Q_D(NoteEditLayerQuickItem);
        if (d->editScopeFocused == editScopeFocused)
            return;
        d->editScopeFocused = editScopeFocused;
        update();
        Q_EMIT editScopeFocusedChanged();
    }

    NoteViewModel *NoteEditLayerQuickItem::lyricEditingItem() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->lyricEditingItem;
    }

    void NoteEditLayerQuickItem::setLyricEditingItem(NoteViewModel *lyricEditingItem) {
        Q_D(NoteEditLayerQuickItem);
        if (d->lyricEditingItem == lyricEditingItem)
            return;
        disconnect(d->lyricEditingItemDestroyedConnection);
        d->lyricEditingItem = lyricEditingItem;
        if (d->lyricEditingItem) {
            d->lyricEditingItemDestroyedConnection = connect(d->lyricEditingItem, &QObject::destroyed, this, [this] {
                Q_D(NoteEditLayerQuickItem);
                d->lyricEditingItem = nullptr;
                d->lyricEditingItemDestroyedConnection = {};
                update();
                Q_EMIT lyricEditingItemChanged();
            });
        } else {
            d->lyricEditingItemDestroyedConnection = {};
        }
        update();
        Q_EMIT lyricEditingItemChanged();
    }

    NoteViewModel *NoteEditLayerQuickItem::additionalTextEditingItem() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->additionalTextEditingItem;
    }

    void NoteEditLayerQuickItem::setAdditionalTextEditingItem(NoteViewModel *additionalTextEditingItem) {
        Q_D(NoteEditLayerQuickItem);
        if (d->additionalTextEditingItem == additionalTextEditingItem)
            return;
        disconnect(d->additionalTextEditingItemDestroyedConnection);
        d->additionalTextEditingItem = additionalTextEditingItem;
        if (d->additionalTextEditingItem) {
            d->additionalTextEditingItemDestroyedConnection = connect(d->additionalTextEditingItem, &QObject::destroyed, this, [this] {
                Q_D(NoteEditLayerQuickItem);
                d->additionalTextEditingItem = nullptr;
                d->additionalTextEditingItemDestroyedConnection = {};
                update();
                Q_EMIT additionalTextEditingItemChanged();
            });
        } else {
            d->additionalTextEditingItemDestroyedConnection = {};
        }
        update();
        Q_EMIT additionalTextEditingItemChanged();
    }

    double NoteEditLayerQuickItem::viewportWidth() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->viewportWidth;
    }

    void NoteEditLayerQuickItem::setViewportWidth(double viewportWidth) {
        Q_D(NoteEditLayerQuickItem);
        if (qFuzzyCompare(d->viewportWidth, viewportWidth))
            return;
        d->viewportWidth = viewportWidth;
        d->updateViewport(false, false);
        Q_EMIT viewportWidthChanged();
    }

    double NoteEditLayerQuickItem::viewportHeight() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->viewportHeight;
    }

    void NoteEditLayerQuickItem::setViewportHeight(double viewportHeight) {
        Q_D(NoteEditLayerQuickItem);
        if (qFuzzyCompare(d->viewportHeight, viewportHeight))
            return;
        d->viewportHeight = viewportHeight;
        d->updateViewport(false, false);
        Q_EMIT viewportHeightChanged();
    }

    int NoteEditLayerQuickItem::shortNoteThreshold() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->shortNoteThreshold;
    }

    void NoteEditLayerQuickItem::setShortNoteThreshold(int shortNoteThreshold) {
        Q_D(NoteEditLayerQuickItem);
        if (d->shortNoteThreshold == shortNoteThreshold)
            return;
        d->shortNoteThreshold = shortNoteThreshold;
        d->rebuildMarkers();
        Q_EMIT shortNoteThresholdChanged();
    }

    bool NoteEditLayerQuickItem::isAdditionalTextAbove() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->additionalTextAbove;
    }

    void NoteEditLayerQuickItem::setAdditionalTextAbove(bool additionalTextAbove) {
        Q_D(NoteEditLayerQuickItem);
        if (d->additionalTextAbove == additionalTextAbove)
            return;
        d->additionalTextAbove = additionalTextAbove;
        d->residentRangeValid = false;
        d->updateViewport(true, true);
        Q_EMIT additionalTextAboveChanged();
    }

    quint64 NoteEditLayerQuickItem::geometryRevision() const {
        Q_D(const NoteEditLayerQuickItem);
        return d->geometryRevision;
    }

    QAbstractItemModel *NoteEditLayerQuickItem::shortMarkerModel() const {
        Q_D(const NoteEditLayerQuickItem);
        return const_cast<NoteEditLayerMarkerModel *>(&d->markerModel);
    }

    PointerHit NoteEditLayerQuickItem::hitTest(const QPointF &point, QQuickItem *coordinateSpace) const {
        Q_D(const NoteEditLayerQuickItem);
        PointerHit hit;
        const NoteEditLayerRecord *targetRecord = d->hitTestRecord(point);
        if (!targetRecord) {
            targetRecord = d->hitTestAdditionalTextRecord(point);
            if (!targetRecord)
                return hit;
            const QRectF localRect = d->additionalTextRect(*targetRecord, true);
            hit.valid = true;
            hit.targetRect = coordinateSpace ? mapRectToItem(coordinateSpace, localRect) : localRect;
            hit.hoverRegion = AdditionalTextHitRegion;
            hit.payload = QVariant::fromValue(static_cast<QObject *>(targetRecord->model));
            return hit;
        }

        const QRectF localRect = d->itemRect(*targetRecord);
        const qreal edgeWidth = std::min(EdgeHitMaximumWidth, localRect.width() * EdgeHitWidthFraction);
        int payload = -1;
        if (point.x() < localRect.left() + edgeWidth)
            payload = EdgeDragHandler::LeftEdge;
        else if (point.x() >= localRect.right() - edgeWidth)
            payload = EdgeDragHandler::RightEdge;

        hit.valid = true;
        hit.target = targetRecord->model;
        hit.targetRect = coordinateSpace ? mapRectToItem(coordinateSpace, localRect) : localRect;
        hit.hoverRegion = NoteHitRegion;
        hit.payload = payload;
        return hit;
    }

    QRectF NoteEditLayerQuickItem::itemRect(QObject *model) const {
        Q_D(const NoteEditLayerQuickItem);
        auto *note = qobject_cast<NoteViewModel *>(model);
        const auto it = d->records.constFind(note);
        return it == d->records.cend() ? QRectF() : d->itemRect(it.value());
    }

    QRectF NoteEditLayerQuickItem::additionalTextRect(QObject *model) const {
        Q_D(const NoteEditLayerQuickItem);
        auto *note = qobject_cast<NoteViewModel *>(model);
        const auto it = d->records.constFind(note);
        return it == d->records.cend() ? QRectF() : d->additionalTextRect(it.value(), false);
    }

    bool NoteEditLayerQuickItem::contains(const QPointF &point) const {
        Q_D(const NoteEditLayerQuickItem);
        return d->hitTestRecord(point) || d->hitTestAdditionalTextRecord(point);
    }

    QRectF NoteEditLayerQuickItem::mapToTickKeyRect(const QRectF &sourceRect) const {
        Q_D(const NoteEditLayerQuickItem);
        if (!d->hasValidLayout())
            return {};
        const QRectF rect = sourceRect.normalized();
        const qreal timePixelDensity = d->timeLayoutViewModel->pixelDensity();
        const qreal clavierPixelDensity = d->clavierViewModel->pixelDensity();
        return {
            rect.left() / timePixelDensity,
            128.0 - rect.bottom() / clavierPixelDensity,
            rect.width() / timePixelDensity,
            rect.height() / clavierPixelDensity,
        };
    }

    QSGNode *NoteEditLayerQuickItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
        Q_D(NoteEditLayerQuickItem);
        if (usesSoftwareRenderer(this)) {
            auto *rootNode = dynamic_cast<SoftwareNoteEditLayerRootNode *>(oldNode);
            if (!rootNode) {
                delete oldNode;
                rootNode = new SoftwareNoteEditLayerRootNode(this);
            }
            rootNode->synchronize(d);
            return rootNode;
        }
        auto *rootNode = dynamic_cast<HardwareNoteEditLayerRootNode *>(oldNode);
        if (!rootNode) {
            delete oldNode;
            rootNode = new HardwareNoteEditLayerRootNode(this);
        }
        rootNode->synchronize(d);
        return rootNode;
    }

}

#include "moc_NoteEditLayerQuickItem_p.cpp"
