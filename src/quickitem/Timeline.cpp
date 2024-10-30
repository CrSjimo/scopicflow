#include "Timeline.h"
#include "Timeline_p.h"

#include <cmath>

#include <QSGSimpleRectNode>
#include <QQuickWindow>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlow/TimeViewModel.h>

namespace sflow {

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

    Timeline::Timeline(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TimelinePrivate) {
        Q_D(Timeline);
        d->q_ptr = this;
        setFlag(ItemHasContents, true);
    }
    Timeline::~Timeline() = default;

    TimelinePalette *Timeline::palette() const {
        Q_D(const Timeline);
        return d->palette;
    }
    void Timeline::setPalette(TimelinePalette *palette) {
        Q_D(Timeline);
        if (d->palette == palette)
            return;
        if (d->palette)
            disconnect(d->palette, nullptr, this, nullptr);
        d->palette = palette;
        if (d->palette) {
            connect(d->palette, &TimelinePalette::backgroundColorChanged, this, &QQuickItem::update);
            connect(d->palette, &TimelinePalette::foregroundColorChanged, this, &QQuickItem::update);
            connect(d->palette, &TimelinePalette::positionIndicatorColorChanged, this, &QQuickItem::update);
            connect(d->palette, &TimelinePalette::cursorIndicatorColorChanged, this, &QQuickItem::update);
        }
        emit paletteChanged();
        update();

    }
    TimeViewModel *Timeline::timeViewModel() const {
        Q_D(const Timeline);
        return d->timeViewModel;
    }
    void Timeline::setTimeViewModel(TimeViewModel *timeViewModel) {
        Q_D(Timeline);
        if (d->timeViewModel == timeViewModel)
            return;
        if (d->timeViewModel)
            disconnect(d->timeViewModel, nullptr, this, nullptr);
        d->timeViewModel = timeViewModel;
        if (d->timeViewModel) {
            connect(d->timeViewModel, &TimeViewModel::startChanged, this, &QQuickItem::update);
            connect(d->timeViewModel, &TimeViewModel::pixelDensityChanged, this, &QQuickItem::update);
            connect(d->timeViewModel, &TimeViewModel::primaryPositionChanged, this, &QQuickItem::update);
            connect(d->timeViewModel, &TimeViewModel::secondaryPositionChanged, this, &QQuickItem::update);
            connect(d->timeViewModel, &TimeViewModel::cursorPositionChanged, this, &QQuickItem::update);
        }
    }

    static inline bool isOnScale(const SVS::PersistentMusicTime &time, int barScaleIntervalExp2, bool doDrawBeatScale) {
        if (doDrawBeatScale)
            return time.beat() == 0;
        return time.measure() % (1 << barScaleIntervalExp2) == 0;
    }

    static inline void moveForward(SVS::PersistentMusicTime &time, int barScaleIntervalExp2, bool doDrawBeatScale) {
        if (doDrawBeatScale) {
            time = time.timeline()->create(time.measure(), time.beat() + 1, 0);
            return;
        }
        int interval = 1 << barScaleIntervalExp2;
        time = time.timeline()->create((time.measure() / interval + 1) * interval, 0, 0);
    }

    QSGNode *Timeline::updatePaintNode(QSGNode *node, UpdatePaintNodeData *) {
        Q_D(Timeline);
        QSGSimpleRectNode *rectNode;
        QSGNode *scaleNode;
        QSGNode *barLabelNode;
        QSGNode *indicatorNode;
        if (!node) {
            node = new QSGNode;
            node->appendChildNode(rectNode = new QSGSimpleRectNode);
            node->appendChildNode(scaleNode = new QSGNode);
            node->appendChildNode(barLabelNode = new QSGNode);
            node->appendChildNode(indicatorNode = new QSGNode);
        } else {
            rectNode = static_cast<QSGSimpleRectNode *>(node->childAtIndex(0));
            scaleNode = static_cast<QSGNode *>(node->childAtIndex(1));
            barLabelNode = static_cast<QSGNode*>(node->childAtIndex(2));
            indicatorNode = static_cast<QSGNode*>(node->childAtIndex(3));
        }
        if (d->palette && d->palette->backgroundColor().isValid())
            rectNode->setColor(d->palette->backgroundColor());
        else
            rectNode->setColor(Qt::black);
        rectNode->setRect(boundingRect());

        scaleNode->removeAllChildNodes();
        barLabelNode->removeAllChildNodes();
        indicatorNode->removeAllChildNodes();

        if (!d->timeViewModel || !d->timeViewModel->timeline())
            return node;
        double minimumScaleDistance = 32;
        bool doDrawBeatScale = d->timeViewModel->pixelDensity() * 480 > minimumScaleDistance;

        int barScaleIntervalExp2 = std::ceil(std::log2(minimumScaleDistance / 480 / 4)); // TODO consider variable time signature

        auto musicTime = d->timeViewModel->timeline()->create(0, 0, static_cast<int>(d->timeViewModel->start()));
        if (!isOnScale(musicTime, barScaleIntervalExp2, doDrawBeatScale))
            moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale);

        for (;; moveForward(musicTime, barScaleIntervalExp2, doDrawBeatScale)) {
            double deltaTick = musicTime.totalTick() - d->timeViewModel->start();
            double x = deltaTick * d->timeViewModel->pixelDensity();
            if (x > width())
                break;
            bool isEmphasized = musicTime.beat() == 0;
            auto lineNode = new QSGGeometryNode;
            auto lineGeometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 2);
            lineGeometry->setDrawingMode(QSGGeometry::DrawLines);
            lineGeometry->setLineWidth(2);
            lineGeometry->vertexDataAsPoint2D()[0].set(x, height());
            lineGeometry->vertexDataAsPoint2D()[1].set(x, height() - std::min(height(), 32.0) * (isEmphasized ? 0.5 : 0.25));
            lineNode->setGeometry(lineGeometry);
            lineNode->setFlag(QSGNode::OwnsGeometry);
            auto material = new QSGFlatColorMaterial;
            if (d->palette && d->palette->foregroundColor().isValid())
                material->setColor(d->palette->foregroundColor());
            else
                material->setColor(Qt::white);
            lineNode->setMaterial(material);
            lineNode->setFlag(QSGNode::OwnsMaterial);
            scaleNode->appendChildNode(lineNode);

        }


        return node;

    }

}