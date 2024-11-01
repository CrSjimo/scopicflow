#include "TimelineQuickItem_p.h"
#include "TimelineQuickItem_p_p.h"

#include <cmath>

#include <QSGSimpleRectNode>
#include <QQuickWindow>
#include <QSGTextNode>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimeAlignmentViewModel.h>

namespace sflow {

    TimelineQuickItemPrivate::~TimelineQuickItemPrivate() {
        for (auto p : barNumberTextLayouts)
            delete p;
        for (auto p : timeSignatureTextLayouts)
            delete p;
    }
    QTextLayout *TimelineQuickItemPrivate::createTextLayoutForBarNumber(int bar) {
        auto layout = barNumberTextLayouts.value(bar);
        if (layout)
            return layout;
        layout = new QTextLayout(QLocale().toString(bar + 1));
        layout->beginLayout();
        layout->createLine();
        layout->endLayout();
        barNumberTextLayouts.insert(bar, layout);
        return layout;
    }
    QTextLayout *TimelineQuickItemPrivate::createTextLayoutForTimeSignature(int numerator, int denominator) {
        qint64 k = denominator;
        k = k << 32 | numerator;
        auto layout = timeSignatureTextLayouts.value(k);
        if (layout)
            return layout;
        layout =
            new QTextLayout(QLocale().toString(numerator) + "/" + QLocale().toString(denominator));
        layout->beginLayout();
        layout->createLine();
        layout->endLayout();
        timeSignatureTextLayouts.insert(k, layout);
        return layout;
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

    TimelinePalette::TimelinePalette(QObject *parent) : QObject(parent) {
    }
    TimelinePalette::~TimelinePalette() = default;

    QColor TimelinePalette::backgroundColor() const {
        return m_backgroundColor;
    }
    void TimelinePalette::setBackgroundColor(const QColor &color) {
        if (m_backgroundColor != color) {
            m_backgroundColor = color;
            emit backgroundColorChanged(color);
        }
    }
    QColor TimelinePalette::foregroundColor() const {
        return m_foregroundColor;
    }
    void TimelinePalette::setForegroundColor(const QColor &color) {
        if (m_foregroundColor != color) {
            m_foregroundColor = color;
            emit foregroundColorChanged(color);
        }
    }
    QColor TimelinePalette::positionIndicatorColor() const {
        return m_positionIndicatorColor;
    }
    void TimelinePalette::setPositionIndicatorColor(const QColor &color) {
        if (m_positionIndicatorColor != color) {
            m_positionIndicatorColor = color;
            emit positionIndicatorColorChanged(color);
        }
    }
    QColor TimelinePalette::cursorIndicatorColor() const {
        return m_cursorIndicatorColor;
    }
    void TimelinePalette::setCursorIndicatorColor(const QColor &color) {
        if (m_cursorIndicatorColor != color) {
            m_cursorIndicatorColor = color;
            emit cursorIndicatorColorChanged(color);
        }
    }

    TimelineQuickItem::TimelineQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TimelineQuickItemPrivate) {
        Q_D(TimelineQuickItem);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
        auto defaultPalette = new TimelinePalette(this);
        defaultPalette->setBackgroundColor(Qt::black);
        defaultPalette->setForegroundColor(Qt::white);
        defaultPalette->setPositionIndicatorColor(Qt::cyan);
        defaultPalette->setCursorIndicatorColor(Qt::red);
        setPalette(defaultPalette);
    }
    TimelineQuickItem::~TimelineQuickItem() = default;

    TimelinePalette *TimelineQuickItem::palette() const {
        Q_D(const TimelineQuickItem);
        return d->palette;
    }
    void TimelineQuickItem::setPalette(TimelinePalette *palette) {
        Q_D(TimelineQuickItem);
        if (d->palette == palette)
            return;
        if (d->palette)
            disconnect(d->palette, nullptr, this, nullptr);
        d->palette = palette;
        if (d->palette) {
            connect(d->palette, &TimelinePalette::backgroundColorChanged, this, &QQuickItem::update);
            connect(d->palette, &TimelinePalette::foregroundColorChanged, this, &QQuickItem::update);
        }
        emit paletteChanged(palette);
        update();

    }
    TimeAlignmentViewModel *TimelineQuickItem::timeAlignmentViewModel() const {
        Q_D(const TimelineQuickItem);
        return d->timeAlignmentViewModel;
    }
    void TimelineQuickItem::setTimeAlignmentViewModel(TimeAlignmentViewModel *timeAlignmentViewModel) {
        Q_D(TimelineQuickItem);
        if (d->timeAlignmentViewModel == timeAlignmentViewModel)
            return;
        if (d->timeAlignmentViewModel)
            disconnect(d->timeAlignmentViewModel, nullptr, this, nullptr);
        d->timeAlignmentViewModel = timeAlignmentViewModel;
        if (d->timeAlignmentViewModel) {
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
            connect(d->timeAlignmentViewModel, &TimeViewModel::primaryPositionChanged, this,
                    [=](int tick) { emit primaryIndicatorXChanged(d->tickToX(tick)); });
            connect(d->timeAlignmentViewModel, &TimeViewModel::secondaryPositionChanged, this,
                    [=](int tick) { emit secondaryIndicatorXChanged(d->tickToX(tick)); });
            connect(d->timeAlignmentViewModel, &TimeViewModel::cursorPositionChanged, this,
                    [=](int tick) { emit cursorIndicatorXChanged(d->tickToX(tick)); });
        }
        emit primaryIndicatorXChanged(primaryIndicatorX());
        emit secondaryIndicatorXChanged(secondaryIndicatorX());
        emit cursorIndicatorXChanged(cursorIndicatorX());
        emit timeAlignmentViewModelChanged();
        update();
    }
    double TimelineQuickItem::primaryIndicatorX() const {
        Q_D(const TimelineQuickItem);
        if (!d->timeAlignmentViewModel)
            return 0;
        return d->tickToX(d->timeAlignmentViewModel->primaryPosition());
    }
    void TimelineQuickItem::setPrimaryIndicatorX(double primaryIndicatorX) {
        Q_D(TimelineQuickItem);
        if (!d->timeAlignmentViewModel)
            return;
        int tick = d->alignTick(std::max(0, d->xToTick(primaryIndicatorX)));
        d->timeAlignmentViewModel->setPrimaryPosition(tick);
        d->timeAlignmentViewModel->setSecondaryPosition(tick);
    }
    double TimelineQuickItem::secondaryIndicatorX() const {
        Q_D(const TimelineQuickItem);
        if (!d->timeAlignmentViewModel)
            return 0;
        return d->tickToX(d->timeAlignmentViewModel->secondaryPosition());
    }
    double TimelineQuickItem::cursorIndicatorX() const {
        Q_D(const TimelineQuickItem);
        if (!d->timeAlignmentViewModel)
            return -1;
        return d->tickToX(d->timeAlignmentViewModel->cursorPosition());
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
        QSGNode *scaleNode;
        QSGNode *indicatorNode;
        if (!node) {
            node = new QSGNode;
            node->appendChildNode(rectNode = new QSGSimpleRectNode);
            rectNode->setFlag(QSGNode::OwnedByParent);
        } else {
            rectNode = static_cast<QSGSimpleRectNode *>(node->childAtIndex(0));
            auto oldScaleNode = node->childAtIndex(1);
            auto oldIndicatorNode = node->childAtIndex(2);
            node->removeChildNode(oldScaleNode);
            delete oldScaleNode;
            node->removeChildNode(oldIndicatorNode);
            delete oldIndicatorNode;
        }
        node->appendChildNode(scaleNode = new QSGNode);
        scaleNode->setFlag(QSGNode::OwnedByParent);
        node->appendChildNode(indicatorNode = new QSGNode);
        indicatorNode->setFlag(QSGNode::OwnedByParent);
        if (d->palette && d->palette->backgroundColor().isValid())
            rectNode->setColor(d->palette->backgroundColor());
        else
            rectNode->setColor(Qt::black);
        rectNode->setRect(boundingRect());

        scaleNode->removeAllChildNodes();
        indicatorNode->removeAllChildNodes();

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

        for (;; moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale)) {
            double deltaTick = musicTime.totalTick() - d->timeAlignmentViewModel->start();
            double x = deltaTick * d->timeAlignmentViewModel->pixelDensity();
            if (x > width())
                break;
            bool isEmphasized = musicTime.beat() == 0;
            auto lineNode = new QSGGeometryNode;
            auto lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2);
            lineGeometry->setDrawingMode(QSGGeometry::DrawLines);
            lineGeometry->setLineWidth(1);
            lineGeometry->vertexDataAsPoint2D()[0].set(x, height());
            lineGeometry->vertexDataAsPoint2D()[1].set(x, height() - 32 * (isEmphasized ? 0.5 : 0.25));
            lineNode->setGeometry(lineGeometry);
            lineNode->setFlag(QSGNode::OwnsGeometry);
            auto material = new QSGFlatColorMaterial;
            if (d->palette && d->palette->foregroundColor().isValid())
                material->setColor(d->palette->foregroundColor());
            else
                material->setColor(Qt::white);
            lineNode->setMaterial(material);
            lineNode->setFlag(QSGNode::OwnsMaterial);
            lineNode->setFlag(QSGNode::OwnedByParent);
            scaleNode->appendChildNode(lineNode);
            if (isEmphasized) {
                auto textNode = window()->createTextNode();
                textNode->setColor(material->color());
                auto barNumberLayout = d->createTextLayoutForBarNumber(musicTime.measure());
                textNode->addTextLayout({x + 2, height() - 16}, barNumberLayout);
                textNode->setFlag(QSGNode::OwnedByParent);
                scaleNode->appendChildNode(textNode);

                if (d->timeAlignmentViewModel->timeline()->nearestTimeSignatureTo(musicTime.measure()) == musicTime.measure()) {
                    auto timeSignature = d->timeAlignmentViewModel->timeline()->timeSignatureAt(musicTime.measure());
                    textNode = window()->createTextNode();
                    textNode->setColor(material->color());
                    textNode->addTextLayout({x + 10 + barNumberLayout->maximumWidth(), height() - 16}, d->createTextLayoutForTimeSignature(timeSignature.numerator(), timeSignature.denominator()));
                    textNode->setFlag(QSGNode::OwnedByParent);
                    scaleNode->appendChildNode(textNode);
                }
            }
        }
        return node;

    }

}