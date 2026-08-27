#include "DragScroller_p.h"
#include "DragScroller_p_p.h"

#include <algorithm>

#include <QJSValueList>

namespace sflow {

    constexpr static int DEFAULT_INTERVAL = 50 / 3;

    DragScrollerPrivate::DragScrollerPrivate(DragScroller *q) : q_ptr(q) {
        tickingTimer = new QTimer(q);
        tickingTimer->setInterval(DEFAULT_INTERVAL);
    }

    void DragScrollerPrivate::onTimeout() {
        Q_Q(DragScroller);
        const double interval = tickingTimer->interval();
        Q_EMIT q->moved(std::clamp(distanceX / 256, -1.0, 1.0) * interval,
                        std::clamp(distanceY / 256, -1.0, 1.0) * interval);
    }

    DragScroller::DragScroller(QObject *parent) : QObject(parent), d_ptr(new DragScrollerPrivate(this)) {
        Q_D(DragScroller);
        connect(d->tickingTimer, &QTimer::timeout, this, [d] {
            d->onTimeout();
        });
    }

    DragScroller::~DragScroller() = default;

    double DragScroller::distanceX() const {
        Q_D(const DragScroller);
        return d->distanceX;
    }

    void DragScroller::setDistanceX(double distanceX) {
        Q_D(DragScroller);
        if (d->distanceX == distanceX)
            return;
        d->distanceX = distanceX;
        Q_EMIT distanceXChanged();
    }

    double DragScroller::distanceY() const {
        Q_D(const DragScroller);
        return d->distanceY;
    }

    void DragScroller::setDistanceY(double distanceY) {
        Q_D(DragScroller);
        if (d->distanceY == distanceY)
            return;
        d->distanceY = distanceY;
        Q_EMIT distanceYChanged();
    }

    bool DragScroller::isRunning() const {
        Q_D(const DragScroller);
        return d->running;
    }

    void DragScroller::setRunning(bool running) {
        Q_D(DragScroller);
        if (d->running == running)
            return;
        d->running = running;
        if (running) {
            d->tickingTimer->start();
        } else {
            d->tickingTimer->stop();
        }
        Q_EMIT runningChanged();
    }

    QTimer *DragScroller::tickingTimer() const {
        Q_D(const DragScroller);
        return d->tickingTimer;
    }

    void DragScroller::determine(double x, double width, double y, double height, const std::function<void(bool, bool)> &callback) {
        bool xTriggered = true;
        if (x < 0)
            setDistanceX(x);
        else if (x > width)
            setDistanceX(x - width);
        else {
            setDistanceX(0);
            xTriggered = false;
        }
        bool yTriggered = true;
        if (y < 0)
            setDistanceY(y);
        else if (y > height)
            setDistanceY(y - height);
        else {
            setDistanceY(0);
            yTriggered = false;
        }
        setRunning(xTriggered || yTriggered);
        callback(xTriggered, yTriggered);
    }

    void DragScroller::determine(double x, double width, double y, double height, const QJSValue &callback) {
        determine(x, width, y, height, [callback](bool xTriggered, bool yTriggered) {
            if (callback.isCallable()) {
                QJSValueList args{QJSValue(xTriggered), QJSValue(yTriggered)};
                callback.call(args);
            }
        });
    }

}

#include "moc_DragScroller_p.cpp"
