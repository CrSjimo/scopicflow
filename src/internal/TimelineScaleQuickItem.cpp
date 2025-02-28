#include "TimelineScaleQuickItem_p.h"
#include "TimelineScaleQuickItem_p_p.h"

#include <cmath>

#include <QSGSimpleRectNode>
#include <QSGTextNode>
#include <QTextLayout>

#include <SVSCraftCore/musictimeline.h>
#include <SVSCraftCore/musictime.h>
#include <SVSCraftCore/musictimesignature.h>

#include <ScopicFlow/TimeViewModel.h>
#include <ScopicFlow/TimeLayoutViewModel.h>

namespace sflow {

    ScaleSGNode::~ScaleSGNode() {
        for (auto p : barNumberTextLayouts)
            delete p;
        barNumberTextLayouts.clear();
        for (auto p : barNumberTextNodes)
            delete p;
        barNumberTextNodes.clear();
        for (auto p : timeSignatureTextLayouts)
            delete p;
        timeSignatureTextLayouts.clear();
        for (auto p : timeSignatureTextNodes)
            delete p;
        timeSignatureTextNodes.clear();
    }
    QTextLayout *ScaleSGNode::createTextLayoutForBarNumber(int bar) {
        auto layout = barNumberTextLayouts.value(bar);
        if (layout)
            return layout;
        if (barNumberTextLayouts.size() > 4096) {
            delete barNumberTextLayouts.cbegin().value();
            barNumberTextLayouts.erase(barNumberTextLayouts.cbegin());
        }
        layout = new QTextLayout(QString::number(bar + 1));
        layout->beginLayout();
        layout->createLine();
        layout->endLayout();
        barNumberTextLayouts.insert(bar, layout);
        return layout;
    }
    QSGTextNode *ScaleSGNode::createTextNodeForBarNumber(int bar, const QColor &color) {
        auto textNode = barNumberTextNodes.value(bar);
        if (textNode) {
            if (textNode->color() == color)
                return textNode;
            delete textNode;
        }
        if (barNumberTextNodes.size() > 1024) {
            delete barNumberTextNodes.cbegin().value();
            barNumberTextNodes.erase(barNumberTextNodes.cbegin());
        }
        textNode = d->q_ptr->window()->createTextNode();
        textNode->setColor(color);
        auto barNumberLayout = createTextLayoutForBarNumber(bar);
        textNode->addTextLayout({0, 0}, barNumberLayout);
        textNode->setFlag(QSGNode::OwnedByParent, false);
        barNumberTextNodes.insert(bar, textNode);
        return textNode;
    }
    QTextLayout *ScaleSGNode::createTextLayoutForTimeSignature(int numerator, int denominator) {
        qint64 k = denominator;
        k = k << 32 | numerator;
        auto layout = timeSignatureTextLayouts.value(k);
        if (layout)
            return layout;
        if (timeSignatureTextLayouts.size() > 4096) {
            delete timeSignatureTextLayouts.cbegin().value();
            timeSignatureTextLayouts.erase(timeSignatureTextLayouts.cbegin());
        }
        layout = new QTextLayout(QString::number(numerator) + "/" + QString::number(denominator));
        layout->beginLayout();
        layout->createLine();
        layout->endLayout();
        timeSignatureTextLayouts.insert(k, layout);
        return layout;
    }
    QSGTextNode *ScaleSGNode::createTextNodeForTimeSignature(int numerator, int denominator, const QColor &color) {
        qint64 k = denominator;
        k = k << 32 | numerator;
        auto textNode = timeSignatureTextNodes.value(k);
        if (textNode) {
            if (textNode->color() == color)
                return textNode;
            delete textNode;
        }
        if (timeSignatureTextNodes.size() > 1024) {
            delete timeSignatureTextNodes.cbegin().value();
            timeSignatureTextNodes.erase(timeSignatureTextNodes.cbegin());
        }
        textNode = d->q_ptr->window()->createTextNode();
        textNode->setColor(color);
        textNode->addTextLayout({}, createTextLayoutForTimeSignature(numerator, denominator));
        textNode->setFlag(QSGNode::OwnedByParent, false);
        timeSignatureTextNodes.insert(k, textNode);
        return textNode;
    }

    TimelineScaleQuickItem::TimelineScaleQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TimelineScaleQuickItemPrivate) {
        Q_D(TimelineScaleQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
        connect(this, &TimelineScaleQuickItem::colorChanged, this, &QQuickItem::update);
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
            connect(viewModel, &TimeViewModel::startChanged, this, &QQuickItem::update);
            connect(viewModel, &TimeViewModel::timelineChanged, this, [=] {
                if (d->timeline) {
                    disconnect(d->timeline, nullptr, this, nullptr);
                }
                d->timeline = viewModel->timeline();
                if (d->timeline) {
                    connect(d->timeline, &SVS::MusicTimeline::changed, this, &QQuickItem::update);
                }
            });
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
            connect(viewModel, &TimeLayoutViewModel::pixelDensityChanged, this, &QQuickItem::update);
            connect(viewModel, &TimeLayoutViewModel::positionAlignmentChanged, this, &QQuickItem::update);
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
        d->color = foregroundColor;
        update();
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
        ScaleSGNode *scaleNode;
        if (!node) {
            node = new QSGNode;
        } else {
            auto oldScaleNode = node->childAtIndex(0);
            delete oldScaleNode;
        }
        node->appendChildNode(scaleNode = new ScaleSGNode(d));
        scaleNode->setFlag(QSGNode::OwnedByParent);

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
        auto foregroundColor = d->color.isValid() ? d->color : Qt::white;

        for (;; moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale)) {
            double deltaTick = musicTime.totalTick() - d->timeViewModel->start();
            double x = deltaTick * d->timeLayoutViewModel->pixelDensity();
            if (x > width())
                break;
            bool isEmphasized = musicTime.beat() == 0;
            xList.emplace_back(x, isEmphasized);

            if (!isEmphasized)
                continue;

            auto barNumberLayout = scaleNode->createTextLayoutForBarNumber(musicTime.measure());
            auto textNode = scaleNode->createTextNodeForBarNumber(musicTime.measure(), foregroundColor);
            QMatrix4x4 transform;
            transform.translate(x + 2, height() - 16);
            textNode->setMatrix(transform);
            scaleNode->appendChildNode(textNode);

            if (d->timeViewModel->timeline()->nearestBarWithTimeSignatureTo(musicTime.measure()) != musicTime.measure())
                continue;

            auto timeSignature = d->timeViewModel->timeline()->timeSignatureAt(musicTime.measure());
            textNode = scaleNode->createTextNodeForTimeSignature(timeSignature.numerator(), timeSignature.denominator(), foregroundColor);
            transform.translate(8 + barNumberLayout->maximumWidth(), 0);
            textNode->setMatrix(transform);
            scaleNode->appendChildNode(textNode);
        }
        auto lineNode = new QSGGeometryNode;
        auto lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), xList.size() * 2);
        lineGeometry->setDrawingMode(QSGGeometry::DrawLines);
        lineGeometry->setLineWidth(1);
        for (int i = 0; i < xList.size(); i++) {
            const auto &[x, isEmphasized] = xList[i];
            lineGeometry->vertexDataAsPoint2D()[i * 2].set(x, height());
            lineGeometry->vertexDataAsPoint2D()[i * 2 + 1].set(x, height() - 32 * (isEmphasized ? 0.5 : 0.25));
        }
        lineNode->setGeometry(lineGeometry);
        lineNode->setFlag(QSGNode::OwnsGeometry);
        auto material = new QSGFlatColorMaterial;
        material->setColor(foregroundColor);
        lineNode->setMaterial(material);
        lineNode->setFlag(QSGNode::OwnsMaterial);
        lineNode->setFlag(QSGNode::OwnedByParent);
        scaleNode->appendChildNode(lineNode);
        return node;

    }

}

#include "moc_TimelineScaleQuickItem_p.cpp"