#include "TimelineQuickItem_p.h"
#include "TimelineQuickItem_p_p.h"

#include <cmath>

#include <QSGSimpleRectNode>
#include <QQuickWindow>
#include <QSGTextNode>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/PlaybackViewModel.h>

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
        layout = new QTextLayout(QLocale().toString(bar + 1));
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
        layout = new QTextLayout(QLocale().toString(numerator) + "/" + QLocale().toString(denominator));
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
    double TimelineQuickItemPrivate::tickToX(int tick) const {
        if (!timeAlignmentViewModel)
            return 0;
        auto deltaTick = tick - timeAlignmentViewModel->start();
        return deltaTick * timeAlignmentViewModel->pixelDensity();
    }
    int TimelineQuickItemPrivate::xToTick(double x) const {
        if (!timeAlignmentViewModel)
            return 0;
        auto deltaTick = x / timeAlignmentViewModel->pixelDensity();
        return static_cast<int>(std::round(timeAlignmentViewModel->start() + deltaTick));
    }
    int TimelineQuickItemPrivate::alignTick(int tick) const {
        if (!timeAlignmentViewModel)
            return tick;
        int align = timeAlignmentViewModel->positionAlignment();
        return (tick + align / 2) / align * align;
    }
    int TimelineQuickItemPrivate::alignTickCeil(int tick) const {
        if (!timeAlignmentViewModel)
            return tick;
        int align = timeAlignmentViewModel->positionAlignment();
        return (tick + align - 1) / align * align;
    }
    int TimelineQuickItemPrivate::alignTickFloor(int tick) const {
        if (!timeAlignmentViewModel)
            return tick;
        int align = timeAlignmentViewModel->positionAlignment();
        return (tick) / align * align;
    }

    TimelineQuickItem::TimelineQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TimelineQuickItemPrivate) {
        Q_D(TimelineQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
        connect(this, &TimelineQuickItem::backgroundColorChanged, this, &QQuickItem::update);
        connect(this, &TimelineQuickItem::foregroundColorChanged, this, &QQuickItem::update);
    }
    TimelineQuickItem::~TimelineQuickItem() = default;

    TimeAlignmentViewModel *TimelineQuickItem::timeAlignmentViewModel() const {
        Q_D(const TimelineQuickItem);
        return d->timeAlignmentViewModel;
    }
    void TimelineQuickItem::setTimeAlignmentViewModel(TimeAlignmentViewModel *timeAlignmentViewModel) {
        Q_D(TimelineQuickItem);
        if (d->timeAlignmentViewModel == timeAlignmentViewModel)
            return;
        if (d->timeAlignmentViewModel) {
            disconnect(d->timeAlignmentViewModel, nullptr, this, nullptr);
        }
        if (d->timeline) {
            disconnect(d->timeline, nullptr, this, nullptr);
        }
        d->timeAlignmentViewModel = timeAlignmentViewModel;
        d->timeline = nullptr;
        if (d->timeAlignmentViewModel) {
            d->timeline = d->timeAlignmentViewModel->timeline();
            connect(d->timeAlignmentViewModel, &TimeViewModel::startChanged, this, [=] {
                emit primaryIndicatorXChanged(primaryIndicatorX());
                emit secondaryIndicatorXChanged(secondaryIndicatorX());
                emit cursorIndicatorXChanged(cursorIndicatorX());
                update();
            });
            connect(d->timeAlignmentViewModel, &TimeViewModel::pixelDensityChanged, this, [=] {
                emit primaryIndicatorXChanged(primaryIndicatorX());
                emit secondaryIndicatorXChanged(secondaryIndicatorX());
                emit cursorIndicatorXChanged(cursorIndicatorX());
                update();
            });
            connect(d->timeAlignmentViewModel, &TimeViewModel::timelineChanged, this, [=] {
                if (d->timeline) {
                   disconnect(d->timeline, nullptr, this, nullptr);
                }
                d->timeline = d->timeAlignmentViewModel->timeline();
                if (d->timeline) {
                    connect(d->timeline, &SVS::MusicTimeline::changed, this, &QQuickItem::update);
                }
            });
            if (d->timeline) {
                connect(d->timeline, &SVS::MusicTimeline::changed, this, &QQuickItem::update);
            }
        }
        emit timeAlignmentViewModelChanged();
        update();
    }
    PlaybackViewModel *TimelineQuickItem::playbackViewModel() const {
        Q_D(const TimelineQuickItem);
        return d->playbackViewModel;
    }
    void TimelineQuickItem::setPlaybackViewModel(PlaybackViewModel *playbackViewModel) {
        Q_D(TimelineQuickItem);
        if (d->playbackViewModel == playbackViewModel)
            return;
        if (d->playbackViewModel) {
            disconnect(d->playbackViewModel, nullptr, this, nullptr);
        }
        d->playbackViewModel = playbackViewModel;
        if (d->playbackViewModel) {
            connect(d->playbackViewModel, &PlaybackViewModel::primaryPositionChanged, this, [=](int tick) { emit primaryIndicatorXChanged(d->tickToX(tick)); });
            connect(d->playbackViewModel, &PlaybackViewModel::secondaryPositionChanged, this, [=](int tick) { emit secondaryIndicatorXChanged(d->tickToX(tick)); });
            connect(d->playbackViewModel, &PlaybackViewModel::cursorPositionChanged, this, [=](int tick) { emit cursorIndicatorXChanged(d->tickToX(tick)); });
        }
        emit primaryIndicatorXChanged(primaryIndicatorX());
        emit secondaryIndicatorXChanged(secondaryIndicatorX());
        emit cursorIndicatorXChanged(cursorIndicatorX());
    }
    ScrollBehaviorViewModel *TimelineQuickItem::scrollBehaviorViewModel() const {
        Q_D(const TimelineQuickItem);
        return d->scrollBehaviorViewModel;
    }
    void TimelineQuickItem::setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel) {
        Q_D(TimelineQuickItem);
        if (d->scrollBehaviorViewModel != scrollBehaviorViewModel) {
            d->scrollBehaviorViewModel = scrollBehaviorViewModel;
            emit scrollBehaviorViewModelChanged(scrollBehaviorViewModel);
        }
    }
    AnimationViewModel *TimelineQuickItem::animationViewModel() const {
        Q_D(const TimelineQuickItem);
        return d->animationViewModel;
    }
    void TimelineQuickItem::setAnimationViewModel(AnimationViewModel *animationViewModel) {
        Q_D(TimelineQuickItem);
        if (d->animationViewModel == animationViewModel)
            return;
        d->animationViewModel = animationViewModel;
        emit animationViewModelChanged(animationViewModel);
    }
    PaletteViewModel *TimelineQuickItem::paletteViewModel() const {
        Q_D(const TimelineQuickItem);
        return d->paletteViewModel;
    }
    void TimelineQuickItem::setPaletteViewModel(PaletteViewModel *paletteViewModel) {
        Q_D(TimelineQuickItem);
        if (d->paletteViewModel == paletteViewModel)
            return;
        d->paletteViewModel = paletteViewModel;
        emit paletteViewModelChanged(paletteViewModel);
        
    }
    double TimelineQuickItem::primaryIndicatorX() const {
        Q_D(const TimelineQuickItem);
        if (!d->playbackViewModel)
            return 0;
        return d->tickToX(d->playbackViewModel->primaryPosition());
    }
    void TimelineQuickItem::setPrimaryIndicatorX(double primaryIndicatorX) {
        Q_D(TimelineQuickItem);
        if (!d->timeAlignmentViewModel || !d->playbackViewModel)
            return;
        int tick = d->alignTick(std::max(0, d->xToTick(primaryIndicatorX)));
        if (d->tickToX(tick) < 0)
            tick += d->timeAlignmentViewModel->positionAlignment();
        else if (d->tickToX(tick) > width())
            tick -= d->timeAlignmentViewModel->positionAlignment();
        d->playbackViewModel->setPrimaryPosition(tick);
        d->playbackViewModel->setSecondaryPosition(tick);
    }
    double TimelineQuickItem::secondaryIndicatorX() const {
        Q_D(const TimelineQuickItem);
        if (!d->playbackViewModel)
            return 0;
        return d->tickToX(d->playbackViewModel->secondaryPosition());
    }
    double TimelineQuickItem::cursorIndicatorX() const {
        Q_D(const TimelineQuickItem);
        if (!d->playbackViewModel)
            return -1;
        return d->tickToX(d->playbackViewModel->cursorPosition());
    }
    QColor TimelineQuickItem::backgroundColor() const {
        Q_D(const TimelineQuickItem);
        return d->backgroundColor;
    }
    void TimelineQuickItem::setBackgroundColor(const QColor &backgroundColor) {
        Q_D(TimelineQuickItem);
        d->backgroundColor = backgroundColor;
        update();
    }
    QColor TimelineQuickItem::foregroundColor() const {
        Q_D(const TimelineQuickItem);
        return d->foregroundColor;
    }
    void TimelineQuickItem::setForegroundColor(const QColor &foregroundColor) {
        Q_D(TimelineQuickItem);
        d->foregroundColor = foregroundColor;
        update();
    }
    int TimelineQuickItem::mapToTick(double x) const {
        Q_D(const TimelineQuickItem);
        return d->xToTick(x);
    }
    double TimelineQuickItem::mapToX(int tick) const {
        Q_D(const TimelineQuickItem);
        return d->tickToX(tick);
    }
    void TimelineQuickItem::handleContextMenuRequest(double x) {
        Q_D(TimelineQuickItem);
        emit contextMenuRequestedForTimeline(d->alignTick(d->xToTick(x)));
    }
    double TimelineQuickItem::getAlignedX(double x) const {
        Q_D(const TimelineQuickItem);
        return d->tickToX(d->alignTick(d->xToTick(x)));
    }
    void TimelineQuickItem::setZoomedRange(double selectionX, double selectionWidth) {
        Q_D(TimelineQuickItem);
        if (!d->timeAlignmentViewModel)
            return;
        int start = d->xToTick(selectionX);
        int end = d->xToTick(selectionX + selectionWidth);
        if (end - start < d->timeAlignmentViewModel->positionAlignment())
            return;
        d->timeAlignmentViewModel->setStart(start);
        d->timeAlignmentViewModel->setPixelDensity(
            qBound(d->timeAlignmentViewModel->minimumPixelDensity(), width() / (end - start),
                   d->timeAlignmentViewModel->maximumPixelDensity()));
    }
    void TimelineQuickItem::moveViewOnDraggingPositionIndicator(double deltaX) {
        Q_D(TimelineQuickItem);
        if (!d->playbackViewModel)
            return;
        auto newStart = std::max(0.0, d->timeAlignmentViewModel->start() +
                                          deltaX / d->timeAlignmentViewModel->pixelDensity());
        auto newEnd = newStart + width() / d->timeAlignmentViewModel->pixelDensity();
        if (newEnd > d->timeAlignmentViewModel->end())
            d->timeAlignmentViewModel->setEnd(newEnd);
        d->timeAlignmentViewModel->setStart(newStart);
        if (deltaX < 0) {
            int tick = d->alignTickCeil(std::max(0, d->xToTick(0)));
            d->playbackViewModel->setPrimaryPosition(tick);
            d->playbackViewModel->setSecondaryPosition(tick);
        } else {
            int tick = d->alignTickFloor(std::max(0, d->xToTick(width())));
            d->playbackViewModel->setPrimaryPosition(tick);
            d->playbackViewModel->setSecondaryPosition(tick);
        }
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

    QSGNode *TimelineQuickItem::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
        Q_D(TimelineQuickItem);
        QSGSimpleRectNode *rectNode;
        ScaleSGNode *scaleNode;
        if (!node) {
            node = new QSGNode;
            node->appendChildNode(rectNode = new QSGSimpleRectNode);
            rectNode->setFlag(QSGNode::OwnedByParent);
        } else {
            rectNode = static_cast<QSGSimpleRectNode *>(node->childAtIndex(0));
            auto oldScaleNode = node->childAtIndex(1);
            delete oldScaleNode;
        }
        node->appendChildNode(scaleNode = new ScaleSGNode(d));
        scaleNode->setFlag(QSGNode::OwnedByParent);
        if (d->backgroundColor.isValid())
            rectNode->setColor(d->backgroundColor);
        else
            rectNode->setColor(Qt::black);
        rectNode->setRect(boundingRect());

        if (!d->timeAlignmentViewModel || !d->timeAlignmentViewModel->timeline())
            return node;
        double minimumScaleDistance = 48;
        bool doDrawBeatScale = d->timeAlignmentViewModel->pixelDensity() * 480 > minimumScaleDistance;

        int barScaleIntervalExp2 = std::ceil(std::log2(minimumScaleDistance / (d->timeAlignmentViewModel->pixelDensity() * 480 * 4))); // TODO consider variable time signature
        barScaleIntervalExp2 = std::max(0, barScaleIntervalExp2);

        auto musicTime = d->timeAlignmentViewModel->timeline()->create(0, 0, static_cast<int>(d->timeAlignmentViewModel->start()));
        if (!isOnScale(musicTime, barScaleIntervalExp2, doDrawBeatScale)) {
            moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale);
            moveBackward(musicTime, barScaleIntervalExp2, doDrawBeatScale);
        }

        QList<QPair<float, bool>> xList;
        auto foregroundColor = d->foregroundColor.isValid() ? d->foregroundColor : Qt::white;

        for (;; moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale)) {
            double deltaTick = musicTime.totalTick() - d->timeAlignmentViewModel->start();
            double x = deltaTick * d->timeAlignmentViewModel->pixelDensity();
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

            if (d->timeAlignmentViewModel->timeline()->nearestTimeSignatureTo(musicTime.measure()) != musicTime.measure())
                continue;

            auto timeSignature = d->timeAlignmentViewModel->timeline()->timeSignatureAt(musicTime.measure());
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