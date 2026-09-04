#include "ParameterEditorQuickItem_p.h"
#include "ParameterEditorQuickItem_p_p.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>
#include <numbers>

#include <QPainter>
#include <QPainterPath>
#include <QQuickWindow>
#include <QSGGeometry>
#include <QSGGeometryNode>
#include <QSGRendererInterface>
#include <QSGVertexColorMaterial>
#include <QSet>

#include <SVSCraftQuick/SoftwarePainterNode.h>

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

        struct RasterMetrics {
            double devicePixelRatio = 1.0;
            double xScale = 1.0;
            double xPhase = 0.0;
            double antialiasWidth = 1.0;
        };

        RasterMetrics rasterMetricsForItem(const QQuickItem *item) {
            RasterMetrics metrics;
            if (item->window()) {
                metrics.devicePixelRatio = item->window()->effectiveDevicePixelRatio();
            }
            if (!std::isfinite(metrics.devicePixelRatio) || metrics.devicePixelRatio <= 0.0) {
                metrics.devicePixelRatio = 1.0;
            }

            const QPointF sceneOrigin = item->mapToScene(QPointF());
            const QPointF sceneX = item->mapToScene(QPointF(1.0, 0.0)) - sceneOrigin;
            const QPointF sceneY = item->mapToScene(QPointF(0.0, 1.0)) - sceneOrigin;
            metrics.xScale = std::hypot(sceneX.x(), sceneX.y());
            const double yScale = std::hypot(sceneY.x(), sceneY.y());
            if (!std::isfinite(metrics.xScale) || metrics.xScale <= 0.0) {
                metrics.xScale = 1.0;
            }
            const double validYScale = std::isfinite(yScale) && yScale > 0.0 ? yScale : 1.0;
            const QPointF sceneXAxis = sceneX / metrics.xScale;
            metrics.xPhase = QPointF::dotProduct(sceneOrigin, sceneXAxis) * metrics.devicePixelRatio;
            if (!std::isfinite(metrics.xPhase)) {
                metrics.xPhase = 0.0;
            } else {
                metrics.xPhase -= std::floor(metrics.xPhase);
            }
            metrics.antialiasWidth = 1.0 / (metrics.devicePixelRatio * std::sqrt(metrics.xScale * validYScale));
            return metrics;
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

        quint32 appendVertex(ParameterEditorGeometry &geometry,
                             const QPointF &point,
                             float coverage) {
            const auto index = static_cast<quint32>(geometry.vertices.size());
            geometry.vertices.append({point, coverage});
            return index;
        }

        void appendQuadIndices(ParameterEditorGeometry &geometry,
                               quint32 a,
                               quint32 b,
                               quint32 c,
                               quint32 d) {
            geometry.indices.append(a);
            geometry.indices.append(b);
            geometry.indices.append(c);
            geometry.indices.append(c);
            geometry.indices.append(b);
            geometry.indices.append(d);
        }

        void appendLineQuad(ParameterEditorGeometry &geometry,
                            const QPointF &a,
                            const QPointF &b,
                            double width,
                            double antialiasWidth) {
            const QPointF direction = b - a;
            const double length = std::hypot(direction.x(), direction.y());
            if (qFuzzyIsNull(length)) {
                return;
            }
            const QPointF unitNormal(-direction.y() / length, direction.x() / length);
            const double fringeHalfWidth = qMax(0.0, antialiasWidth * 0.5);
            const double innerHalfWidth = qMax(0.0, width * 0.5 - fringeHalfWidth);
            const double outerHalfWidth = width * 0.5 + fringeHalfWidth;
            const QPointF innerNormal = unitNormal * innerHalfWidth;
            const QPointF outerNormal = unitNormal * outerHalfWidth;

            const QPointF innerPositiveA = a + innerNormal;
            const QPointF innerNegativeA = a - innerNormal;
            const QPointF innerPositiveB = b + innerNormal;
            const QPointF innerNegativeB = b - innerNormal;
            const quint32 outerPositiveA = appendVertex(geometry, a + outerNormal, 0.0f);
            const quint32 innerPositiveAIndex = appendVertex(geometry, innerPositiveA, 1.0f);
            const quint32 innerNegativeAIndex = appendVertex(geometry, innerNegativeA, 1.0f);
            const quint32 outerNegativeA = appendVertex(geometry, a - outerNormal, 0.0f);
            const quint32 outerPositiveB = appendVertex(geometry, b + outerNormal, 0.0f);
            const quint32 innerPositiveBIndex = appendVertex(geometry, innerPositiveB, 1.0f);
            const quint32 innerNegativeBIndex = appendVertex(geometry, innerNegativeB, 1.0f);
            const quint32 outerNegativeB = appendVertex(geometry, b - outerNormal, 0.0f);
            appendQuadIndices(geometry,
                              outerPositiveA,
                              innerPositiveAIndex,
                              outerPositiveB,
                              innerPositiveBIndex);
            appendQuadIndices(geometry,
                              innerPositiveAIndex,
                              innerNegativeAIndex,
                              innerPositiveBIndex,
                              innerNegativeBIndex);
            appendQuadIndices(geometry,
                              innerNegativeAIndex,
                              outerNegativeA,
                              innerNegativeBIndex,
                              outerNegativeB);
        }

        void appendDashedLine(ParameterEditorGeometry &geometry,
                              const QPointF &a,
                              const QPointF &b,
                              double width,
                              double antialiasWidth,
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
                                   width,
                                   antialiasWidth);
                }
                offset = endOffset;
            }
            phase = std::fmod(phase + length, patternLength);
        }

        void appendFillQuad(ParameterEditorGeometry &geometry,
                            const QPointF &a,
                            const QPointF &b,
                            double baseline,
                            double antialiasWidth) {
            if (qFuzzyCompare(a.y() + 1.0, baseline + 1.0)
                && qFuzzyCompare(b.y() + 1.0, baseline + 1.0)) {
                return;
            }
            const QPointF bottomA(a.x(), baseline);
            const QPointF bottomB(b.x(), baseline);
            const QPointF direction = b - a;
            const double length = std::hypot(direction.x(), direction.y());
            if (qFuzzyIsNull(length)) {
                return;
            }
            QPointF inwardNormal(-direction.y() / length, direction.x() / length);
            const QPointF curveCenter = (a + b) * 0.5;
            const QPointF baselineCenter(curveCenter.x(), baseline);
            if (QPointF::dotProduct(baselineCenter - curveCenter, inwardNormal) < 0.0) {
                inwardNormal = -inwardNormal;
            }
            const QPointF fringeNormal = inwardNormal * qMax(0.0, antialiasWidth * 0.5);
            const QPointF innerA = a + fringeNormal;
            const QPointF innerB = b + fringeNormal;
            const QPointF outerA = a - fringeNormal;
            const QPointF outerB = b - fringeNormal;
            const quint32 outerAIndex = appendVertex(geometry, outerA, 0.0f);
            const quint32 innerAIndex = appendVertex(geometry, innerA, 1.0f);
            const quint32 bottomAIndex = appendVertex(geometry, bottomA, 1.0f);
            const quint32 outerBIndex = appendVertex(geometry, outerB, 0.0f);
            const quint32 innerBIndex = appendVertex(geometry, innerB, 1.0f);
            const quint32 bottomBIndex = appendVertex(geometry, bottomB, 1.0f);
            appendQuadIndices(geometry, outerAIndex, innerAIndex, outerBIndex, innerBIndex);
            appendQuadIndices(geometry, innerAIndex, bottomAIndex, innerBIndex, bottomBIndex);
        }

        void appendAntialiasedPolygon(ParameterEditorGeometry &geometry,
                                      const QPointF &center,
                                      const QVector<QPointF> &unitPerimeter,
                                      double radius,
                                      double antialiasWidth) {
            const double innerRadius = qMax(0.0, radius - antialiasWidth * 0.5);
            const double outerRadius = radius + antialiasWidth * 0.5;
            const quint32 centerIndex = appendVertex(geometry, center, 1.0f);
            QVector<quint32> innerIndices;
            QVector<quint32> outerIndices;
            innerIndices.reserve(unitPerimeter.size());
            outerIndices.reserve(unitPerimeter.size());
            for (const auto &point : unitPerimeter) {
                innerIndices.append(appendVertex(geometry, center + point * innerRadius, 1.0f));
                outerIndices.append(appendVertex(geometry, center + point * outerRadius, 0.0f));
            }
            for (qsizetype i = 0; i < unitPerimeter.size(); ++i) {
                const qsizetype next = (i + 1) % unitPerimeter.size();
                geometry.indices.append(centerIndex);
                geometry.indices.append(innerIndices.at(i));
                geometry.indices.append(innerIndices.at(next));
                appendQuadIndices(geometry,
                                  innerIndices.at(i),
                                  outerIndices.at(i),
                                  innerIndices.at(next),
                                  outerIndices.at(next));
            }
        }

        QVector<QPointF> anchorPerimeter(ParameterAnchorViewModel::InterpolationMode interpolationMode) {
            QVector<QPointF> perimeter;
            switch (interpolationMode) {
                case ParameterAnchorViewModel::Hermite:
                    perimeter.reserve(circleSegments);
                    for (int i = 0; i < circleSegments; ++i) {
                        const double angle = 2.0 * std::numbers::pi * i / circleSegments;
                        perimeter.append(QPointF(std::cos(angle), std::sin(angle)));
                    }
                    break;
                case ParameterAnchorViewModel::Linear:
                    perimeter = {
                        QPointF(0.0, -std::numbers::sqrt2),
                        QPointF(std::numbers::sqrt2, 0.0),
                        QPointF(0.0, std::numbers::sqrt2),
                        QPointF(-std::numbers::sqrt2, 0.0),
                    };
                    break;
                case ParameterAnchorViewModel::None:
                    perimeter = {
                        QPointF(-1.0, -1.0),
                        QPointF(1.0, -1.0),
                        QPointF(1.0, 1.0),
                        QPointF(-1.0, 1.0),
                    };
                    break;
            }
            return perimeter;
        }

        void appendAnchor(ParameterEditorGeometry &geometry,
                          const QPointF &center,
                          double radius,
                          ParameterAnchorViewModel::InterpolationMode interpolationMode,
                          double antialiasWidth) {
            const auto perimeter = anchorPerimeter(interpolationMode);
            appendAntialiasedPolygon(geometry, center, perimeter, radius, antialiasWidth);
        }

        enum SoftwarePrimitive : quint32 {
            SoftwareFill = 1 << 0,
            SoftwareCurve = 1 << 1,
            SoftwareAccent = 1 << 2,
            SoftwareSelectedAnchor = 1 << 3,
            SoftwareReference = 1 << 4,
        };

        struct ParameterEditorSoftwareSnapshot {
            QPainterPath fill;
            QPainterPath solid;
            QPainterPath dashed;
            QPainterPath accent;
            QPainterPath anchors;
            QPainterPath selectedAnchors;
            QPainterPath reference;
        };

        void appendPathLine(QPainterPath &path, const QPointF &a, const QPointF &b) {
            if (qFuzzyIsNull(std::hypot(b.x() - a.x(), b.y() - a.y()))) {
                return;
            }
            path.moveTo(a);
            path.lineTo(b);
        }

        void appendDashedPath(QPainterPath &path,
                              const QPointF &a,
                              const QPointF &b,
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
                    appendPathLine(path,
                                   a + delta * (offset / length),
                                   a + delta * (endOffset / length));
                }
                offset = endOffset;
            }
            phase = std::fmod(phase + length, patternLength);
        }

        void appendAnchorPath(QPainterPath &path,
                              const QPointF &center,
                              double radius,
                              ParameterAnchorViewModel::InterpolationMode interpolationMode) {
            const auto perimeter = anchorPerimeter(interpolationMode);
            if (perimeter.isEmpty()) {
                return;
            }
            path.moveTo(center + perimeter.first() * radius);
            for (qsizetype i = 1; i < perimeter.size(); ++i) {
                path.lineTo(center + perimeter.at(i) * radius);
            }
            path.closeSubpath();
        }

        void appendFillRun(QPainterPath &path,
                           const QVector<ParameterEditorSemanticSample> &samples,
                           qsizetype first,
                           qsizetype last,
                           double baseline) {
            if (last <= first) {
                return;
            }
            bool hasArea = false;
            for (qsizetype i = first; i <= last; ++i) {
                if (!qFuzzyCompare(samples.at(i).finalPoint.y() + 1.0, baseline + 1.0)) {
                    hasArea = true;
                    break;
                }
            }
            if (!hasArea) {
                return;
            }
            path.moveTo(samples.at(first).finalPoint);
            for (qsizetype i = first + 1; i <= last; ++i) {
                path.lineTo(samples.at(i).finalPoint);
            }
            path.lineTo(samples.at(last).finalPoint.x(), baseline);
            path.lineTo(samples.at(first).finalPoint.x(), baseline);
            path.closeSubpath();
        }

        ParameterEditorGeometrySnapshot buildHardwareSnapshot(const ParameterEditorSemanticSnapshot &source) {
            ParameterEditorGeometrySnapshot result;
            ParameterEditorSemanticSample previous;
            bool hasPrevious = false;
            double dashedPhase = 0.0;
            bool previousSegmentDashed = false;
            for (const auto &current : source.samples) {
                bool currentSegmentDashed = false;
                if (hasPrevious && previous.finalValid && current.finalValid) {
                    if (source.fillMode != ParameterEditorQuickItem::NoFill) {
                        appendFillQuad(result.fill,
                                       previous.finalPoint,
                                       current.finalPoint,
                                       source.fillY,
                                       source.antialiasWidth);
                    }
                    const bool fallbackSegment = previous.finalSource != ParameterEditorQuickItemPrivate::EditedSource
                        && current.finalSource != ParameterEditorQuickItemPrivate::EditedSource;
                    if (!fallbackSegment || source.fallbackDisplayMode == ParameterEditorQuickItem::CurveSolid) {
                        appendLineQuad(result.solid,
                                       previous.finalPoint,
                                       current.finalPoint,
                                       lineWidth,
                                       source.antialiasWidth);
                    } else if (source.fallbackDisplayMode == ParameterEditorQuickItem::CurveDashed) {
                        if (!previousSegmentDashed) {
                            dashedPhase = 0.0;
                        }
                        appendDashedLine(result.dashed,
                                         previous.finalPoint,
                                         current.finalPoint,
                                         lineWidth,
                                         source.antialiasWidth,
                                         dashedPhase);
                        currentSegmentDashed = true;
                    }
                }
                if (hasPrevious && previous.overlayValid && current.overlayValid) {
                    appendLineQuad(result.accent,
                                   previous.overlayPoint,
                                   current.overlayPoint,
                                   accentLineWidth,
                                   source.antialiasWidth);
                }
                previousSegmentDashed = currentSegmentDashed;
                previous = current;
                hasPrevious = true;
            }
            if (source.referenceVisible) {
                appendLineQuad(result.reference,
                               QPointF(0.0, source.referenceY),
                               QPointF(source.samples.isEmpty() ? 0.0 : source.samples.constLast().finalPoint.x(), source.referenceY),
                               lineWidth,
                               source.antialiasWidth);
            }
            for (const auto &anchor : source.anchors) {
                if (anchor.selected) {
                    appendAnchor(result.selectedAnchors,
                                 anchor.center,
                                 selectedAnchorRadius,
                                 anchor.interpolationMode,
                                 source.antialiasWidth);
                }
                appendAnchor(result.anchors,
                             anchor.center,
                             anchorRadius,
                             anchor.interpolationMode,
                             source.antialiasWidth);
            }
            return result;
        }

        ParameterEditorSoftwareSnapshot buildSoftwareSnapshot(const ParameterEditorSemanticSnapshot &source,
                                                               quint32 drawMask) {
            ParameterEditorSoftwareSnapshot result;
            qsizetype fillStart = -1;
            ParameterEditorSemanticSample previous;
            bool hasPrevious = false;
            double dashedPhase = 0.0;
            bool previousSegmentDashed = false;
            for (qsizetype index = 0; index < source.samples.size(); ++index) {
                const auto &current = source.samples.at(index);
                if ((drawMask & SoftwareFill) && source.fillMode != ParameterEditorQuickItem::NoFill) {
                    if (current.finalValid) {
                        if (fillStart < 0) {
                            fillStart = index;
                        }
                    } else if (fillStart >= 0) {
                        appendFillRun(result.fill, source.samples, fillStart, index - 1, source.fillY);
                        fillStart = -1;
                    }
                }
                bool currentSegmentDashed = false;
                if (hasPrevious && previous.finalValid && current.finalValid && (drawMask & SoftwareCurve)) {
                    const bool fallbackSegment = previous.finalSource != ParameterEditorQuickItemPrivate::EditedSource
                        && current.finalSource != ParameterEditorQuickItemPrivate::EditedSource;
                    if (!fallbackSegment || source.fallbackDisplayMode == ParameterEditorQuickItem::CurveSolid) {
                        appendPathLine(result.solid, previous.finalPoint, current.finalPoint);
                    } else if (source.fallbackDisplayMode == ParameterEditorQuickItem::CurveDashed) {
                        if (!previousSegmentDashed) {
                            dashedPhase = 0.0;
                        }
                        appendDashedPath(result.dashed, previous.finalPoint, current.finalPoint, dashedPhase);
                        currentSegmentDashed = true;
                    }
                }
                if (hasPrevious && previous.overlayValid && current.overlayValid && (drawMask & SoftwareAccent)) {
                    appendPathLine(result.accent, previous.overlayPoint, current.overlayPoint);
                }
                previousSegmentDashed = currentSegmentDashed;
                previous = current;
                hasPrevious = true;
            }
            if (fillStart >= 0) {
                appendFillRun(result.fill, source.samples, fillStart, source.samples.size() - 1, source.fillY);
            }
            if ((drawMask & SoftwareReference) && source.referenceVisible && !source.samples.isEmpty()) {
                appendPathLine(result.reference,
                               QPointF(0.0, source.referenceY),
                               QPointF(source.samples.constLast().finalPoint.x(), source.referenceY));
            }
            for (const auto &anchor : source.anchors) {
                if (anchor.selected && (drawMask & SoftwareSelectedAnchor)) {
                    appendAnchorPath(result.selectedAnchors,
                                     anchor.center,
                                     selectedAnchorRadius,
                                     anchor.interpolationMode);
                }
                if (drawMask & SoftwareAccent) {
                    appendAnchorPath(result.anchors,
                                     anchor.center,
                                     anchorRadius,
                                     anchor.interpolationMode);
                }
            }
            return result;
        }

        QSGGeometryNode *createGeometryNode() {
            auto *node = new QSGGeometryNode;
            auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(),
                                             0,
                                             0,
                                             QSGGeometry::UnsignedIntType);
            geometry->setDrawingMode(QSGGeometry::DrawTriangles);
            geometry->setVertexDataPattern(QSGGeometry::DynamicPattern);
            geometry->setIndexDataPattern(QSGGeometry::DynamicPattern);
            node->setGeometry(geometry);
            node->setFlag(QSGNode::OwnsGeometry);
            auto *material = new QSGVertexColorMaterial;
            node->setMaterial(material);
            node->setFlag(QSGNode::OwnsMaterial);
            return node;
        }

        void updateGeometryNode(QSGGeometryNode *node,
                                const ParameterEditorGeometry &source,
                                const QColor &color) {
            auto *geometry = node->geometry();
            geometry->allocate(static_cast<int>(source.vertices.size()),
                               static_cast<int>(source.indices.size()));
            auto *vertices = geometry->vertexDataAsColoredPoint2D();
            for (int i = 0; i < source.vertices.size(); ++i) {
                const auto &point = source.vertices.at(i);
                const int alpha = qRound(color.alpha() * std::clamp(point.coverage, 0.0f, 1.0f));
                const int red = qRound(color.red() * alpha / 255.0);
                const int green = qRound(color.green() * alpha / 255.0);
                const int blue = qRound(color.blue() * alpha / 255.0);
                vertices[i].set(static_cast<float>(point.point.x()),
                                static_cast<float>(point.point.y()),
                                red,
                                green,
                                blue,
                                alpha);
            }
            std::copy(source.indices.cbegin(), source.indices.cend(), geometry->indexDataAsUInt());
            node->markDirty(QSGNode::DirtyGeometry);
        }

        class ParameterEditorHardwareNode : public QSGNode {
        public:
            ParameterEditorHardwareNode() {
                for (int i = 0; i < NodeCount; ++i) {
                    appendChildNode(createGeometryNode());
                }
            }

            void synchronize(const ParameterEditorSemanticSnapshot &snapshot,
                             quint64 revision,
                             const std::array<QColor, NodeCount> &colors) {
                const bool geometryChanged = m_revision != revision;
                if (geometryChanged) {
                    m_snapshot = buildHardwareSnapshot(snapshot);
                    m_revision = revision;
                }
                const std::array<const ParameterEditorGeometry *, NodeCount> geometries {
                    &m_snapshot.fill,
                    &m_snapshot.solid,
                    &m_snapshot.dashed,
                    &m_snapshot.accent,
                    &m_snapshot.selectedAnchors,
                    &m_snapshot.anchors,
                    &m_snapshot.reference,
                };
                for (int i = 0; i < NodeCount; ++i) {
                    if (geometryChanged || m_colors.at(i) != colors.at(i)) {
                        updateGeometryNode(static_cast<QSGGeometryNode *>(childAtIndex(i)),
                                           *geometries.at(i),
                                           colors.at(i));
                    }
                }
                m_colors = colors;
            }

        private:
            ParameterEditorGeometrySnapshot m_snapshot;
            std::array<QColor, NodeCount> m_colors{};
            quint64 m_revision = std::numeric_limits<quint64>::max();
        };

        class ParameterEditorSoftwareNode : public SVS::SoftwarePainterNode {
        public:
            explicit ParameterEditorSoftwareNode(QQuickItem *item) : SoftwarePainterNode(item) {
            }

            void synchronize(const ParameterEditorSemanticSnapshot &snapshot,
                             quint64 revision,
                             const QColor &fillColor,
                             const QColor &curveColor,
                             const QColor &accentColor,
                             const QColor &selectedAnchorColor,
                             const QColor &referenceColor,
                             const QSizeF &size) {
                quint32 drawMask = 0;
                if (fillColor.alpha() > 0 && snapshot.fillMode != ParameterEditorQuickItem::NoFill) {
                    drawMask |= SoftwareFill;
                }
                if (curveColor.alpha() > 0) {
                    drawMask |= SoftwareCurve;
                }
                if (accentColor.alpha() > 0) {
                    drawMask |= SoftwareAccent;
                }
                if (selectedAnchorColor.alpha() > 0) {
                    drawMask |= SoftwareSelectedAnchor;
                }
                if (referenceColor.alpha() > 0 && snapshot.referenceVisible) {
                    drawMask |= SoftwareReference;
                }
                if (m_revision != revision || m_drawMask != drawMask) {
                    m_snapshot = buildSoftwareSnapshot(snapshot, drawMask);
                    m_revision = revision;
                    m_drawMask = drawMask;
                    markDirty(QSGNode::DirtyGeometry);
                }
                if (m_fillColor != fillColor
                    || m_curveColor != curveColor
                    || m_accentColor != accentColor
                    || m_selectedAnchorColor != selectedAnchorColor
                    || m_referenceColor != referenceColor) {
                    m_fillColor = fillColor;
                    m_curveColor = curveColor;
                    m_accentColor = accentColor;
                    m_selectedAnchorColor = selectedAnchorColor;
                    m_referenceColor = referenceColor;
                    markDirty(QSGNode::DirtyMaterial);
                }
                constexpr double antialiasMargin = 1.0;
                const double margin = selectedAnchorRadius + antialiasMargin;
                setBoundingRect(QRectF(QPointF(), size).adjusted(-margin,
                                                                 -margin,
                                                                 margin,
                                                                 margin));
            }

        protected:
            void paint(QPainter *painter) override {
                painter->setRenderHint(QPainter::Antialiasing);
                if (!m_snapshot.fill.isEmpty() && m_fillColor.alpha() > 0) {
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(m_fillColor);
                    painter->drawPath(m_snapshot.fill);
                }

                drawLinePath(painter, m_snapshot.solid, m_curveColor, lineWidth);
                drawLinePath(painter, m_snapshot.dashed, m_curveColor, lineWidth);
                drawLinePath(painter, m_snapshot.accent, m_accentColor, accentLineWidth);

                if (!m_snapshot.selectedAnchors.isEmpty() && m_selectedAnchorColor.alpha() > 0) {
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(m_selectedAnchorColor);
                    painter->drawPath(m_snapshot.selectedAnchors);
                }
                if (!m_snapshot.anchors.isEmpty() && m_accentColor.alpha() > 0) {
                    painter->setPen(Qt::NoPen);
                    painter->setBrush(m_accentColor);
                    painter->drawPath(m_snapshot.anchors);
                }
                drawLinePath(painter, m_snapshot.reference, m_referenceColor, lineWidth);
            }

        private:
            static void drawLinePath(QPainter *painter,
                                     const QPainterPath &path,
                                     const QColor &color,
                                     double width) {
                if (path.isEmpty() || color.alpha() == 0) {
                    return;
                }
                painter->setPen(QPen(color, width, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin));
                painter->setBrush(Qt::NoBrush);
                painter->drawPath(path);
            }

            ParameterEditorSoftwareSnapshot m_snapshot;
            QColor m_fillColor;
            QColor m_curveColor;
            QColor m_accentColor;
            QColor m_selectedAnchorColor;
            QColor m_referenceColor;
            quint32 m_drawMask = 0;
            quint64 m_revision = std::numeric_limits<quint64>::max();
        };

        bool usesSoftwareRenderer(const QQuickItem *item) {
            return item->window()
                && item->window()->rendererInterface()->graphicsApi() == QSGRendererInterface::Software;
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
        geometryDirty = true;
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
        geometryDirty = false;
        semanticSnapshot = {};
        ++snapshotRevision;
        if (!timeViewModel || !timeLayoutViewModel || timeLayoutViewModel->pixelDensity() <= 0.0
            || q->width() <= 0.0 || q->height() <= 0.0) {
            return;
        }

        const RasterMetrics rasterMetrics = rasterMetricsForItem(q);
        QVector<double> sampleXs;
        const double physicalWidth = q->width() * rasterMetrics.devicePixelRatio * rasterMetrics.xScale;
        if (!std::isfinite(physicalWidth) || physicalWidth > std::numeric_limits<int>::max() - 1.0) {
            return;
        }
        const int rasterPointCount = static_cast<int>(std::ceil(physicalWidth));
        sampleXs.reserve(rasterPointCount + 16);
        sampleXs.append(0.0);
        const double firstDeviceX = std::ceil(rasterMetrics.xPhase);
        const double localUnitsPerDevicePixel = 1.0 / (rasterMetrics.devicePixelRatio * rasterMetrics.xScale);
        for (int i = 0; i <= rasterPointCount; ++i) {
            const double x = (firstDeviceX + i - rasterMetrics.xPhase) * localUnitsPerDevicePixel;
            if (x > 0.0 && x < q->width()) {
                sampleXs.append(x);
            }
        }
        sampleXs.append(q->width());
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
        const auto addFreeBreakpoints = [&] {
            if ((!freeParameterViewModel && !originalParameterViewModel && !freeTransformParameterViewModel)
                || visibleEnd < 0.0) {
                return;
            }
            const double projectedStep = FreeParameterViewModel::step()
                * timeLayoutViewModel->pixelDensity()
                * rasterMetrics.devicePixelRatio
                * rasterMetrics.xScale;
            if (!std::isfinite(projectedStep) || projectedStep < 0.5) {
                return;
            }
            const double firstScaledPosition = std::ceil(std::max(0.0, visibleStart) / FreeParameterViewModel::step());
            const double lastScaledPosition = std::floor(std::max(0.0, visibleEnd) / FreeParameterViewModel::step());
            if (firstScaledPosition > lastScaledPosition
                || firstScaledPosition > std::numeric_limits<int>::max()) {
                return;
            }
            const int firstIndex = static_cast<int>(firstScaledPosition);
            const int lastIndex = static_cast<int>(std::min<double>(lastScaledPosition,
                                                                    std::numeric_limits<int>::max()));
            const qsizetype mergeStart = sampleXs.size();
            for (int index = firstIndex;; ++index) {
                const double x = xForPosition(static_cast<double>(index) * FreeParameterViewModel::step());
                if (x > 0.0 && x < q->width()) {
                    sampleXs.append(x);
                }
                if (index == lastIndex) {
                    break;
                }
            }
            std::inplace_merge(sampleXs.begin(), sampleXs.begin() + mergeStart, sampleXs.end());
        };
        addFreeBreakpoints();
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
        semanticSnapshot.fillMode = fillMode;
        semanticSnapshot.fillY = fillY;
        semanticSnapshot.fallbackDisplayMode = fallbackDisplayMode;
        semanticSnapshot.referenceVisible = referenceVisible;
        semanticSnapshot.referenceY = yForValue(referenceBaseline);
        semanticSnapshot.antialiasWidth = rasterMetrics.antialiasWidth;
        semanticSnapshot.samples.reserve(sampleXs.size());
        for (qsizetype index = 0; index < sampleXs.size(); ++index) {
            const double x = sampleXs.at(index);
            auto currentFinal = evaluateFinal(index);
            currentFinal.point.setX(x);
            const auto overlayValue = evaluateOverlay(index);
            const bool overlayValid = overlayValue.isValid();
            const QPointF currentOverlay(x, overlayValid ? yForValue(overlayValue.toDouble()) : 0.0);
            semanticSnapshot.samples.append({
                .finalPoint = currentFinal.point,
                .overlayPoint = currentOverlay,
                .finalSource = currentFinal.source,
                .finalValid = currentFinal.valid,
                .overlayValid = overlayValid,
            });
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
                semanticSnapshot.anchors.append({
                    .center = center,
                    .interpolationMode = item->interpolationMode(),
                    .selected = item->isSelected(),
                });
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
        connect(this, &QQuickItem::visibleChanged, this, [this, d] {
            if (isVisible()) {
                d->invalidate();
            }
        });
    }

    ParameterEditorQuickItem::~ParameterEditorQuickItem() = default;

    void ParameterEditorQuickItem::itemChange(ItemChange change, const ItemChangeData &value) {
        QQuickItem::itemChange(change, value);
        if ((change == ItemTransformHasChanged || change == ItemDevicePixelRatioHasChanged) && !d_ptr.isNull()) {
            Q_D(ParameterEditorQuickItem);
            d->invalidate();
        }
    }

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
        if (!d->geometryDirty || !isVisible()) {
            return;
        }
        d->rebuildGeometry();
    }

    QSGNode *ParameterEditorQuickItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) {
        Q_D(ParameterEditorQuickItem);
        const QColor finalCurveColor = d->editLayer == FinalLayer ? d->curveColor : d->dimmedCurveColor;
        const QColor finalFillColor = d->editLayer == FinalLayer ? d->fillColor : d->dimmedFillColor;
        if (usesSoftwareRenderer(this)) {
            auto *node = dynamic_cast<ParameterEditorSoftwareNode *>(oldNode);
            if (!node) {
                delete oldNode;
                node = new ParameterEditorSoftwareNode(this);
            }
            node->synchronize(d->semanticSnapshot,
                              d->snapshotRevision,
                              finalFillColor,
                              finalCurveColor,
                              d->accentColor,
                              d->selectedAnchorColor,
                              d->referenceColor,
                              size());
            return node;
        }
        auto *root = dynamic_cast<ParameterEditorHardwareNode *>(oldNode);
        if (!root) {
            delete oldNode;
            root = new ParameterEditorHardwareNode;
        }
        root->synchronize(d->semanticSnapshot,
                          d->snapshotRevision,
                          {
                              finalFillColor,
                              finalCurveColor,
                              finalCurveColor,
                              d->accentColor,
                              d->selectedAnchorColor,
                              d->accentColor,
                              d->referenceColor,
                          });
        return root;
    }

}

#include "moc_ParameterEditorQuickItem_p.cpp"
