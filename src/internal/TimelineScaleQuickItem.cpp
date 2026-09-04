#include "TimelineScaleQuickItem_p.h"
#include "TimelineScaleQuickItem_p_p.h"

#include <cmath>

#include <QQuickWindow>
#include <QSGRectangleNode>
#include <QSGRendererInterface>
#include <QSGSimpleRectNode>
#include <QSGTextNode>
#include <QSGTransformNode>
#include <QTextLayout>

#include <SVSCraftCore/MusicTimeline.h>
#include <SVSCraftCore/MusicTime.h>
#include <SVSCraftCore/MusicTimeSignature.h>

#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>

namespace sflow {

    namespace {

        class TimelineScaleRootNode : public QSGTransformNode {
        public:
            explicit TimelineScaleRootNode(bool software) : software(software) {
            }

            bool software = false;
        };

        bool usesSoftwareRenderer(const QQuickItem *item) {
            return item->window()
                && item->window()->rendererInterface()->graphicsApi() == QSGRendererInterface::Software;
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

        template<typename Key>
        void trimDetachedTextNodes(QHash<Key, QSharedPointer<QSGTextNode>> &nodes, qsizetype maximumSize) {
            if (nodes.size() <= maximumSize) {
                return;
            }
            for (auto it = nodes.begin(); it != nodes.end() && nodes.size() > maximumSize;) {
                if (!it.value()->parent()) {
                    it = nodes.erase(it);
                } else {
                    ++it;
                }
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

    ScaleSGNode::~ScaleSGNode() = default;
    QTextLayout *ScaleSGNode::createTextLayoutForBarNumber(int bar) {
        auto layout = barNumberTextLayouts.value(bar);
        if (layout && layout->font() == d->font) {
            return layout.get();
        }
        if (barNumberTextLayouts.size() > 4096) {
            barNumberTextLayouts.clear();
        }
        layout = QSharedPointer<QTextLayout>::create(QString::number(bar + 1));
        layout->setFont(d->font);
        layout->beginLayout();
        layout->createLine();
        layout->endLayout();
        barNumberTextLayouts.insert(bar, layout);
        return layout.get();
    }
    QSGTextNode *ScaleSGNode::createTextNodeForBarNumber(int bar) {
        auto textNode = barNumberTextNodes.value(bar);
        auto layout = barNumberTextLayouts.value(bar);
        if (layout && layout->font() == d->font && textNode && d->q_ptr->window() == window && textNode->color() == d->color) {
            return textNode.get();
        }
        trimDetachedTextNodes(barNumberTextNodes, 1024);
        if (textNode && textNode->parent()) {
            textNode->parent()->removeChildNode(textNode.get());
        }
        window = d->q_ptr->window();
        textNode.reset(window->createTextNode());
        textNode->setColor(d->color);
        auto barNumberLayout = createTextLayoutForBarNumber(bar);
        textNode->addTextLayout({0, 0}, barNumberLayout);
        textNode->setFlag(QSGNode::OwnedByParent, false);
        barNumberTextNodes.insert(bar, textNode);
        return textNode.get();
    }
    QTextLayout *ScaleSGNode::createTextLayoutForTimeSignature(int numerator, int denominator) {
        qint64 k = denominator;
        k = k << 32 | numerator;
        auto layout = timeSignatureTextLayouts.value(k);
        if (layout && layout->font() == d->font)
            return layout.get();
        if (timeSignatureTextLayouts.size() > 4096) {
            timeSignatureTextLayouts.clear();
        }
        layout = QSharedPointer<QTextLayout>::create(QString::number(numerator) + "/" + QString::number(denominator));
        layout->beginLayout();
        layout->createLine();
        layout->endLayout();
        timeSignatureTextLayouts.insert(k, layout);
        return layout.get();
    }
    QSGTextNode *ScaleSGNode::createTextNodeForTimeSignature(int bar, int numerator, int denominator) {
        const QString key = QStringLiteral("%1/%2/%3").arg(bar).arg(numerator).arg(denominator);
        auto textNode = timeSignatureTextNodes.value(key);
        if (textNode && d->q_ptr->window() == window && textNode->color() == d->color) {
            return textNode.get();
        }
        trimDetachedTextNodes(timeSignatureTextNodes, 1024);
        if (textNode && textNode->parent()) {
            textNode->parent()->removeChildNode(textNode.get());
        }
        window = d->q_ptr->window();
        textNode.reset(window->createTextNode());
        textNode->setColor(d->color);
        textNode->addTextLayout({}, createTextLayoutForTimeSignature(numerator, denominator));
        textNode->setFlag(QSGNode::OwnedByParent, false);
        timeSignatureTextNodes.insert(key, textNode);
        return textNode.get();
    }
    void TimelineScaleQuickItemPrivate::updateTimeline() {
        Q_Q(TimelineScaleQuickItem);
        if (timeline) {
            QObject::disconnect(timeline, nullptr, q, nullptr);
        }
        timeline = timeViewModel->timeline();
        if (timeline) {
            QObject::connect(timeline, &SVS::MusicTimeline::changed, q, &QQuickItem::update);
        }
    }

    TimelineScaleQuickItem::TimelineScaleQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TimelineScaleQuickItemPrivate) {
        Q_D(TimelineScaleQuickItem);
        d->q_ptr = this;
        d->color = Qt::white;
        setFlag(ItemHasContents, true);
    }
    TimelineScaleQuickItem::~TimelineScaleQuickItem() = default;

    TimeViewModel *TimelineScaleQuickItem::timeViewModel() const {
        Q_D(const TimelineScaleQuickItem);
        return d->timeViewModel;
    }
    void TimelineScaleQuickItem::setTimeViewModel(TimeViewModel *viewModel) {
        Q_D(TimelineScaleQuickItem);
        if (d->timeViewModel == viewModel) {
            return;
        }
        if (d->timeViewModel) {
            disconnect(d->timeViewModel, nullptr, this, nullptr);
        }
        if (d->timeline) {
            disconnect(d->timeline, nullptr, this, nullptr);
        }
        d->timeViewModel = viewModel;
        d->timeline = nullptr;
        if (viewModel) {
            d->timeline = viewModel->timeline();
            connect(viewModel, SIGNAL(startChanged()), this, SLOT(update()));
            connect(viewModel, SIGNAL(timelineChanged()), this, SLOT(updateTimeline()));
            if (d->timeline) {
                connect(d->timeline, &SVS::MusicTimeline::changed, this, &QQuickItem::update);
            }
        }
        emit timeViewModelChanged();
        update();
    }
    TimeLayoutViewModel *TimelineScaleQuickItem::timeLayoutViewModel() const {
        Q_D(const TimelineScaleQuickItem);
        return d->timeLayoutViewModel;
    }
    void TimelineScaleQuickItem::setTimeLayoutViewModel(TimeLayoutViewModel *viewModel) {
        Q_D(TimelineScaleQuickItem);
        if (d->timeLayoutViewModel == viewModel) {
            return;
        }
        if (d->timeLayoutViewModel) {
            disconnect(d->timeLayoutViewModel, nullptr, this, nullptr);
        }
        d->timeLayoutViewModel = viewModel;
        if (viewModel) {
            connect(viewModel, SIGNAL(pixelDensityChanged()), this, SLOT(update()));
        }
        emit timeLayoutViewModelChanged();
        update();
    }
    QColor TimelineScaleQuickItem::color() const {
        Q_D(const TimelineScaleQuickItem);
        return d->color;
    }
    void TimelineScaleQuickItem::setColor(const QColor &foregroundColor) {
        Q_D(TimelineScaleQuickItem);
        if (d->color == foregroundColor) {
            return;
        }
        d->color = foregroundColor;
        update();
        emit colorChanged();
    }
    QFont TimelineScaleQuickItem::font() const {
        Q_D(const TimelineScaleQuickItem);
        return d->font;
    }
    void TimelineScaleQuickItem::setFont(const QFont &font) {
        Q_D(TimelineScaleQuickItem);
        if (d->font == font) {
            return;
        }
        d->font = font;
        update();
        emit fontChanged();
    }

    static inline bool isOnScale(const SVS::PersistentMusicTime &time, int barScaleIntervalExp2, bool doDrawBeatScale) {
        if (doDrawBeatScale)
            return time.tick() == 0;
        return time.tick() == 0 && time.beat() == 0 && time.measure() % (1 << barScaleIntervalExp2) == 0;
    }

    static inline void moveBackward(SVS::PersistentMusicTime &time, int barScaleIntervalExp2, bool doDrawBeatScale) {
        if (doDrawBeatScale) {
            time = time.timeline()->create(time.measure(), time.beat() - 1, 0);
            return;
        }
        int interval = 1 << barScaleIntervalExp2;
        time = time.timeline()->create(time.measure() - interval, 0, 0);
    }

    static inline void moveForward(SVS::PersistentMusicTime &time, int barScaleIntervalExp2, bool doDrawBeatScale) {
        if (doDrawBeatScale) {
            time = time.timeline()->create(time.measure(), time.beat() + 1, 0);
            return;
        }
        int interval = 1 << barScaleIntervalExp2;
        time = time.timeline()->create((time.measure() / interval + 1) * interval, 0, 0);
    }

    QSGNode *TimelineScaleQuickItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
        Q_D(TimelineScaleQuickItem);
        const bool software = usesSoftwareRenderer(this);
        ScaleSGNode *scaleNode;
        auto *root = static_cast<TimelineScaleRootNode *>(node);
        if (!root || root->software != software) {
            delete root;
            root = new TimelineScaleRootNode(software);
            node = root;
            node->appendChildNode(scaleNode = new ScaleSGNode(d));
            scaleNode->setFlag(QSGNode::OwnedByParent);
            if (software) {
                auto *lineContainer = new QSGTransformNode;
                lineContainer->setFlag(QSGNode::OwnedByParent);
                node->appendChildNode(lineContainer);
            } else {
                auto *lineNode = new QSGGeometryNode;
                lineNode->setFlag(QSGNode::OwnedByParent);
                auto *material = new QSGFlatColorMaterial;
                lineNode->setMaterial(material);
                lineNode->setFlag(QSGNode::OwnsMaterial);
                lineNode->setGeometry(new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2));
                lineNode->setFlag(QSGNode::OwnsGeometry);
                node->appendChildNode(lineNode);
            }
        } else {
            scaleNode = static_cast<ScaleSGNode *>(node->childAtIndex(0));
        }

        if (!d->timeViewModel || !d->timeLayoutViewModel || !d->timeViewModel->timeline())
            return node;
        double minimumScaleDistance = 48;
        bool doDrawBeatScale = d->timeLayoutViewModel->pixelDensity() * 480 > minimumScaleDistance;

        int barScaleIntervalExp2 = std::ceil(std::log2(minimumScaleDistance / (d->timeLayoutViewModel->pixelDensity() * 480 * 4))); // TODO consider variable time signature
        barScaleIntervalExp2 = std::max(0, barScaleIntervalExp2);

        auto musicTime = d->timeViewModel->timeline()->create(0, 0, static_cast<int>(d->timeViewModel->start()));
        if (!isOnScale(musicTime, barScaleIntervalExp2, doDrawBeatScale)) {
            moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale);
            moveBackward(musicTime, barScaleIntervalExp2, doDrawBeatScale);
        }

        QList<QPair<float, bool>> xList;

        QList<QSGNode *> desiredTextNodes;

        for (;; moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale)) {
            double deltaTick = musicTime.totalTick() - d->timeViewModel->start();
            double x = deltaTick * d->timeLayoutViewModel->pixelDensity();
            if (x > width())
                break;
            bool isEmphasized = musicTime.beat() == 0;
            xList.emplace_back(x, isEmphasized);

            if (!isEmphasized)
                continue;

            auto textNode = scaleNode->createTextNodeForBarNumber(musicTime.measure());
            auto barNumberLayout = scaleNode->createTextLayoutForBarNumber(musicTime.measure());
            QMatrix4x4 transform;
            transform.translate(x + 2, 6);
            textNode->setMatrix(transform);
            desiredTextNodes.append(textNode);

            if (d->timeViewModel->timeline()->nearestBarWithTimeSignatureTo(musicTime.measure()) != musicTime.measure())
                continue;

            auto timeSignature = d->timeViewModel->timeline()->timeSignatureAt(musicTime.measure());
            textNode = scaleNode->createTextNodeForTimeSignature(musicTime.measure(), timeSignature.numerator(), timeSignature.denominator());
            transform.translate(8 + barNumberLayout->maximumWidth(), 0);
            textNode->setMatrix(transform);
            desiredTextNodes.append(textNode);
        }
        synchronizeChildOrder(scaleNode, desiredTextNodes);
        if (software) {
            auto *lineContainer = node->childAtIndex(1);
            ensureRectangleNodeCount(lineContainer, xList.size(), window());
            for (int i = 0; i < xList.size(); ++i) {
                const auto &[x, isEmphasized] = xList.at(i);
                const qreal top = isEmphasized ? 8 : 16;
                auto *lineNode = static_cast<QSGRectangleNode *>(lineContainer->childAtIndex(i));
                const QRectF rect(x - 0.5, top, 1, std::max<qreal>(0, height() - top));
                if (lineNode->rect() != rect) {
                    lineNode->setRect(rect);
                }
                if (lineNode->color() != d->color) {
                    lineNode->setColor(d->color);
                }
            }
        } else {
            auto *lineNode = static_cast<QSGGeometryNode *>(node->childAtIndex(1));
            auto *material = static_cast<QSGFlatColorMaterial *>(lineNode->material());
            auto lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), xList.size() * 2);
            lineGeometry->setDrawingMode(QSGGeometry::DrawLines);
            lineGeometry->setLineWidth(1);
            for (int i = 0; i < xList.size(); i++) {
                const auto &[x, isEmphasized] = xList[i];
                lineGeometry->vertexDataAsPoint2D()[i * 2].set(x, height());
                lineGeometry->vertexDataAsPoint2D()[i * 2 + 1].set(x, isEmphasized ? 8 : 16);
            }
            lineNode->setGeometry(lineGeometry);
            material->setColor(d->color);
        }
        return node;

    }

}

#include "moc_TimelineScaleQuickItem_p.cpp"
