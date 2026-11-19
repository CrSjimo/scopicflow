#include "AbsoluteTimelineScaleQuickItem_p.h"
#include "AbsoluteTimelineScaleQuickItem_p_p.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>

#include <QHash>
#include <QList>
#include <QMap>
#include <QMatrix4x4>
#include <QPair>
#include <QQuickWindow>
#include <QRectF>
#include <QSGFlatColorMaterial>
#include <QSGGeometryNode>
#include <QSGRectangleNode>
#include <QSGRendererInterface>
#include <QSGTextNode>
#include <QSGTransformNode>
#include <QSharedPointer>
#include <QString>
#include <QTextLayout>

#include <SVSCraftCore/MusicTime.h>
#include <SVSCraftCore/MusicTimeline.h>

#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeViewModel.h>

namespace sflow {

    namespace {

        constexpr double minimumMajorScaleDistance = 64;
        constexpr double minimumMinorScaleDistance = 16;

        struct ScaleInterval {
            qint64 major;
            qint64 minor;
            int fractionalDigits;
        };

        constexpr ScaleInterval scaleIntervals[] = {
            {1, 0, 3},
            {2, 1, 3},
            {5, 1, 3},
            {10, 5, 2},
            {20, 10, 2},
            {50, 10, 2},
            {100, 50, 1},
            {200, 100, 1},
            {500, 100, 1},
            {1000, 500, 0},
            {2000, 1000, 0},
            {5000, 1000, 0},
            {10000, 5000, 0},
            {20000, 10000, 0},
            {30000, 10000, 0},
        };

        ScaleInterval scaleIntervalFor(double pixelsPerMillisecond) {
            for (const auto &interval : scaleIntervals) {
                if (interval.major * pixelsPerMillisecond >= minimumMajorScaleDistance
                    && (interval.minor == 0 || interval.minor * pixelsPerMillisecond >= minimumMinorScaleDistance)) {
                    return interval;
                }
            }

            qint64 major = 60000;
            while (major * pixelsPerMillisecond < minimumMajorScaleDistance) {
                if (major > std::numeric_limits<qint64>::max() / 2) {
                    return {};
                }
                major *= 2;
            }
            return {major, major / 2, 0};
        }

        using TimecodeKey = QPair<qint64, int>;

        class AbsoluteTimelineScaleRootNode : public QSGTransformNode {
        public:
            AbsoluteTimelineScaleRootNode(QQuickWindow *window, bool software)
                : m_window(window), m_software(software) {
            }

            QQuickWindow *const m_window;
            const bool m_software;
        };

        class AbsoluteScaleSGNode : public QSGTransformNode {
        public:
            QTextLayout *createTextLayoutForTimecode(const TimecodeKey &key, const QFont &font);
            QSGTextNode *createTextNodeForTimecode(const TimecodeKey &key, const QFont &font, const QColor &color, QQuickWindow *window);

        private:
            QHash<TimecodeKey, QSharedPointer<QTextLayout>> m_timecodeTextLayouts;
            QHash<TimecodeKey, QSharedPointer<QSGTextNode>> m_timecodeTextNodes;
        };

        QTextLayout *AbsoluteScaleSGNode::createTextLayoutForTimecode(const TimecodeKey &key, const QFont &font) {
            auto layout = m_timecodeTextLayouts.value(key);
            if (layout && layout->font() == font) {
                return layout.get();
            }
            if (m_timecodeTextLayouts.size() > 4096) {
                m_timecodeTextLayouts.clear();
            }
            const auto [millisecond, fractionalDigits] = key;
            QString timecode = QStringLiteral("%1:%2").arg(millisecond / 60000).arg(millisecond / 1000 % 60, 2, 10, u'0');
            if (fractionalDigits > 0) {
                timecode += QStringLiteral(".%1").arg(millisecond % 1000, 3, 10, u'0').left(fractionalDigits + 1);
            }
            layout = QSharedPointer<QTextLayout>::create(timecode, font);
            layout->beginLayout();
            layout->createLine();
            layout->endLayout();
            m_timecodeTextLayouts.insert(key, layout);
            return layout.get();
        }

        QSGTextNode *AbsoluteScaleSGNode::createTextNodeForTimecode(const TimecodeKey &key, const QFont &font, const QColor &color, QQuickWindow *window) {
            auto textNode = m_timecodeTextNodes.value(key);
            auto layout = m_timecodeTextLayouts.value(key);
            if (layout && layout->font() == font && textNode && textNode->color() == color) {
                return textNode.get();
            }
            for (auto it = m_timecodeTextNodes.begin(); it != m_timecodeTextNodes.end() && m_timecodeTextNodes.size() > 1024;) {
                if (!it.value()->parent()) {
                    it = m_timecodeTextNodes.erase(it);
                } else {
                    ++it;
                }
            }
            if (textNode && textNode->parent()) {
                textNode->parent()->removeChildNode(textNode.get());
            }
            textNode.reset(window->createTextNode());
            textNode->setColor(color);
            textNode->addTextLayout({}, createTextLayoutForTimecode(key, font));
            textNode->setFlag(QSGNode::OwnedByParent, false);
            m_timecodeTextNodes.insert(key, textNode);
            return textNode.get();
        }

        void ensureRectangleNodeCount(QSGNode *container, int count, QQuickWindow *window) {
            while (container->childCount() < count) {
                auto *node = window->createRectangleNode();
                node->setFlag(QSGNode::OwnedByParent);
                container->appendChildNode(node);
            }
            while (container->childCount() > count) {
                auto *node = container->lastChild();
                container->removeChildNode(node);
                delete node;
            }
        }

        void synchronizeChildOrder(QSGNode *container, const QList<QSGNode *> &desiredNodes) {
            for (int index = 0; index < desiredNodes.size(); ++index) {
                auto *desiredNode = desiredNodes.at(index);
                auto *currentNode = index < container->childCount()
                    ? container->childAtIndex(index)
                    : nullptr;
                if (currentNode == desiredNode) {
                    continue;
                }
                if (desiredNode->parent()) {
                    desiredNode->parent()->removeChildNode(desiredNode);
                }
                if (currentNode) {
                    container->insertChildNodeBefore(desiredNode, currentNode);
                } else {
                    container->appendChildNode(desiredNode);
                }
            }
            while (container->childCount() > desiredNodes.size()) {
                auto *staleNode = container->lastChild();
                container->removeChildNode(staleNode);
                if (staleNode->flags().testFlag(QSGNode::OwnedByParent)) {
                    delete staleNode;
                }
            }
        }

    }

    void AbsoluteTimelineScaleQuickItemPrivate::updateTimeline() {
        Q_Q(AbsoluteTimelineScaleQuickItem);
        if (timeline) {
            QObject::disconnect(timeline, nullptr, q, nullptr);
        }
        timeline = timeViewModel ? timeViewModel->timeline() : nullptr;
        if (timeline) {
            QObject::connect(timeline, &SVS::MusicTimeline::changed, q, &QQuickItem::update);
            QObject::connect(timeline, &QObject::destroyed, q, &QQuickItem::update);
        }
        q->update();
    }

    AbsoluteTimelineScaleQuickItem::AbsoluteTimelineScaleQuickItem(QQuickItem *parent)
        : QQuickItem(parent), d_ptr(new AbsoluteTimelineScaleQuickItemPrivate) {
        Q_D(AbsoluteTimelineScaleQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
    }

    AbsoluteTimelineScaleQuickItem::~AbsoluteTimelineScaleQuickItem() = default;

    TimeViewModel *AbsoluteTimelineScaleQuickItem::timeViewModel() const {
        Q_D(const AbsoluteTimelineScaleQuickItem);
        return d->timeViewModel;
    }

    void AbsoluteTimelineScaleQuickItem::setTimeViewModel(TimeViewModel *viewModel) {
        Q_D(AbsoluteTimelineScaleQuickItem);
        if (d->timeViewModel == viewModel) {
            return;
        }
        if (d->timeViewModel) {
            disconnect(d->timeViewModel, nullptr, this, nullptr);
        }
        d->timeViewModel = viewModel;
        if (viewModel) {
            connect(viewModel, &TimeViewModel::startChanged, this, &QQuickItem::update);
            connect(viewModel, SIGNAL(timelineChanged()), this, SLOT(updateTimeline()));
            connect(viewModel, &QObject::destroyed, this, &QQuickItem::update);
        }
        d->updateTimeline();
        Q_EMIT timeViewModelChanged();
    }

    TimeLayoutViewModel *AbsoluteTimelineScaleQuickItem::timeLayoutViewModel() const {
        Q_D(const AbsoluteTimelineScaleQuickItem);
        return d->timeLayoutViewModel;
    }

    void AbsoluteTimelineScaleQuickItem::setTimeLayoutViewModel(TimeLayoutViewModel *viewModel) {
        Q_D(AbsoluteTimelineScaleQuickItem);
        if (d->timeLayoutViewModel == viewModel) {
            return;
        }
        if (d->timeLayoutViewModel) {
            disconnect(d->timeLayoutViewModel, nullptr, this, nullptr);
        }
        d->timeLayoutViewModel = viewModel;
        if (viewModel) {
            connect(viewModel, &TimeLayoutViewModel::pixelDensityChanged, this, &QQuickItem::update);
            connect(viewModel, &QObject::destroyed, this, &QQuickItem::update);
        }
        Q_EMIT timeLayoutViewModelChanged();
        update();
    }

    QColor AbsoluteTimelineScaleQuickItem::color() const {
        Q_D(const AbsoluteTimelineScaleQuickItem);
        return d->color;
    }

    void AbsoluteTimelineScaleQuickItem::setColor(const QColor &color) {
        Q_D(AbsoluteTimelineScaleQuickItem);
        if (d->color == color) {
            return;
        }
        d->color = color;
        update();
        Q_EMIT colorChanged();
    }

    QFont AbsoluteTimelineScaleQuickItem::font() const {
        Q_D(const AbsoluteTimelineScaleQuickItem);
        return d->font;
    }

    void AbsoluteTimelineScaleQuickItem::setFont(const QFont &font) {
        Q_D(AbsoluteTimelineScaleQuickItem);
        if (d->font == font) {
            return;
        }
        d->font = font;
        update();
        Q_EMIT fontChanged();
    }

    QSGNode *AbsoluteTimelineScaleQuickItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
        Q_D(AbsoluteTimelineScaleQuickItem);
        if (!d->timeViewModel || !d->timeLayoutViewModel || !d->timeline || width() <= 0 || height() <= 0) {
            delete node;
            return nullptr;
        }

        const double startTick = d->timeViewModel->start();
        const double pixelDensity = d->timeLayoutViewModel->pixelDensity();
        if (!std::isfinite(startTick) || !std::isfinite(pixelDensity) || pixelDensity <= 0) {
            delete node;
            return nullptr;
        }

        const double leftTick = std::max(0.0, startTick - minimumMajorScaleDistance / pixelDensity);
        const double rightTick = startTick + (width() + minimumMajorScaleDistance) / pixelDensity;
        if (rightTick < leftTick) {
            delete node;
            return nullptr;
        }

        const auto tempi = d->timeline->tempi();
        const double maximumTick = std::numeric_limits<int>::max();
        auto tempo = std::prev(tempi.upperBound(static_cast<int>(std::min(leftTick, maximumTick))));
        const auto tempoEnd = tempi.upperBound(static_cast<int>(std::min(rightTick, maximumTick)));
        double minimumTempo = tempo.value();
        for (auto it = std::next(tempo); it != tempoEnd; ++it) {
            minimumTempo = std::min(minimumTempo, it.value());
        }

        const int resolution = d->timeline->ticksPerQuarterNote();
        const auto tickRate = [resolution](double bpm) { return resolution * bpm / 60000.0; };
        const auto interval = scaleIntervalFor(pixelDensity * tickRate(minimumTempo));
        const qint64 step = interval.minor == 0 ? interval.major : interval.minor;
        if (step == 0) {
            delete node;
            return nullptr;
        }

        const auto millisecondAtTempo = [d](int tick) { return d->timeline->create(0, 0, tick).millisecond(); };
        const auto lastTempo = std::prev(tempoEnd);
        const double endMillisecond = millisecondAtTempo(lastTempo.key()) + (rightTick - lastTempo.key()) / tickRate(lastTempo.value());
        if (!(endMillisecond < static_cast<double>(std::numeric_limits<qint64>::max() - step))) {
            delete node;
            return nullptr;
        }

        auto *root = static_cast<AbsoluteTimelineScaleRootNode *>(node);
        auto *quickWindow = window();
        const bool software = quickWindow->rendererInterface()->graphicsApi() == QSGRendererInterface::Software;
        if (!root || root->m_window != quickWindow || root->m_software != software) {
            delete root;
            root = new AbsoluteTimelineScaleRootNode(quickWindow, software);
            root->appendChildNode(new AbsoluteScaleSGNode);
            if (software) {
                root->appendChildNode(new QSGTransformNode);
            } else {
                auto *lineNode = new QSGGeometryNode;
                auto *geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 0);
                geometry->setDrawingMode(QSGGeometry::DrawLines);
                geometry->setLineWidth(1);
                lineNode->setGeometry(geometry);
                lineNode->setFlag(QSGNode::OwnsGeometry);
                lineNode->setMaterial(new QSGFlatColorMaterial);
                lineNode->setFlag(QSGNode::OwnsMaterial);
                root->appendChildNode(lineNode);
            }
        }
        auto *scaleNode = static_cast<AbsoluteScaleSGNode *>(root->childAtIndex(0));

        auto font = d->font;
        if (font.pixelSize() > 1) {
            font.setPixelSize(std::max(1, font.pixelSize() - 1));
        } else if (font.pointSizeF() > 1) {
            font.setPointSizeF(std::max<qreal>(1, font.pointSizeF() - 1));
        }

        QList<QPair<float, bool>> xList;
        QList<QSGNode *> desiredTextNodes;

        double tempoMillisecond = millisecondAtTempo(tempo.key());
        double ticksPerMillisecond = tickRate(tempo.value());
        const double startMillisecond = tempoMillisecond + (leftTick - tempo.key()) / ticksPerMillisecond;
        auto nextTempo = std::next(tempo);
        double nextTempoMillisecond = nextTempo == tempoEnd ? std::numeric_limits<double>::infinity() : millisecondAtTempo(nextTempo.key());
        for (qint64 millisecond = static_cast<qint64>(std::ceil(startMillisecond / step)) * step; millisecond <= endMillisecond; millisecond += step) {
            while (millisecond >= nextTempoMillisecond) {
                tempo = nextTempo++;
                tempoMillisecond = nextTempoMillisecond;
                ticksPerMillisecond = tickRate(tempo.value());
                nextTempoMillisecond = nextTempo == tempoEnd ? std::numeric_limits<double>::infinity() : millisecondAtTempo(nextTempo.key());
            }

            const double tick = tempo.key() + (millisecond - tempoMillisecond) * ticksPerMillisecond;
            const double x = (tick - startTick) * pixelDensity;
            const bool isEmphasized = millisecond % interval.major == 0;
            if (x >= -0.5 && x <= width() + 0.5) {
                xList.emplace_back(x, isEmphasized);
            }
            if (!isEmphasized) {
                continue;
            }

            const TimecodeKey key{millisecond, interval.fractionalDigits};
            auto *textNode = scaleNode->createTextNodeForTimecode(key, font, d->color, quickWindow);
            auto *layout = scaleNode->createTextLayoutForTimecode(key, font);
            QMatrix4x4 transform;
            transform.translate(x - layout->maximumWidth() / 2, 0);
            textNode->setMatrix(transform);
            desiredTextNodes.append(textNode);
        }

        synchronizeChildOrder(scaleNode, desiredTextNodes);
        const auto scaleTop = [this](bool isEmphasized) { return std::max<qreal>(0, height() - (isEmphasized ? 6 : 4)); };
        if (software) {
            auto *lineContainer = root->childAtIndex(1);
            ensureRectangleNodeCount(lineContainer, xList.size(), quickWindow);
            for (int i = 0; i < xList.size(); ++i) {
                const auto &[x, isEmphasized] = xList.at(i);
                const qreal top = scaleTop(isEmphasized);
                auto *lineNode = static_cast<QSGRectangleNode *>(lineContainer->childAtIndex(i));
                const QRectF rect(x - 0.5, top, 1, height() - top);
                if (lineNode->rect() != rect) {
                    lineNode->setRect(rect);
                }
                if (lineNode->color() != d->color) {
                    lineNode->setColor(d->color);
                }
            }
        } else {
            auto *lineNode = static_cast<QSGGeometryNode *>(root->childAtIndex(1));
            auto *material = static_cast<QSGFlatColorMaterial *>(lineNode->material());
            auto *geometry = lineNode->geometry();
            geometry->allocate(xList.size() * 2);
            auto *vertices = geometry->vertexDataAsPoint2D();
            for (int i = 0; i < xList.size(); ++i) {
                const auto &[x, isEmphasized] = xList.at(i);
                vertices[i * 2].set(x, height());
                vertices[i * 2 + 1].set(x, scaleTop(isEmphasized));
            }
            material->setColor(d->color);
            lineNode->markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);
        }
        return root;
    }

}

#include "moc_AbsoluteTimelineScaleQuickItem_p.cpp"
