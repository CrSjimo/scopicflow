#include "TimeManipulator.h"
#include "TimeManipulator_p.h"

#include <QQuickItem>

#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>

namespace sflow {

    void TimeManipulatorPrivate::setViewSize(double size) {
        Q_Q(TimeManipulator);
        if (viewSize == size)
            return;
        viewSize = size;
        startAnimation.stop();
        pixelDensityAnimation.stop();
        emit q->viewSizeChanged();
    }
    void TimeManipulatorPrivate::updateParent() {
        Q_Q(TimeManipulator);
        if (auto item = qobject_cast<QQuickItem *>(q->parent())) {
            QObject::connect(item, &QQuickItem::widthChanged, q, [=, this] {
                if (!isViewSizeExplicitSet)
                    setViewSize(item->width());
            });
            setViewSize(item->width());
        }
    }

    TimeManipulator::TimeManipulator(QObject *parent) : QObject(parent), d_ptr(new TimeManipulatorPrivate) {
        Q_D(TimeManipulator);
        d->q_ptr = this;
        d->startAnimation.setEasingCurve(QEasingCurve::OutCubic);
        d->pixelDensityAnimation.setEasingCurve(QEasingCurve::OutCubic);
        connect(&d->startAnimation, &QVariantAnimation::valueChanged, this, [d](const QVariant& value) {
            if (!d->timeViewModel)
                return;
            d->timeViewModel->setProperty("start", value.toDouble());
        });
        connect(&d->pixelDensityAnimation, &QVariantAnimation::valueChanged, this, [d](const QVariant& value) {
            if (!d->timeViewModel || !d->timeLayoutViewModel)
                return;
            d->timeViewModel->setProperty("start", qFuzzyIsNull(d->timeViewModel->start()) && d->currentAnimationFixStartToZero ? 0.0 : qMax(0.0, d->timeViewModel->start() + d->animationCenter / d->timeLayoutViewModel->pixelDensity() - d->animationCenter / value.toDouble()));
            d->timeLayoutViewModel->setProperty("pixelDensity", value);
        });
        d->updateParent();
    }
    TimeManipulator::~TimeManipulator() = default;

    TimeViewModel *TimeManipulator::timeViewModel() const {
        Q_D(const TimeManipulator);
        return d->timeViewModel;
    }
    void TimeManipulator::setTimeViewModel(TimeViewModel *timeViewModel) {
        Q_D(TimeManipulator);
        if (d->timeViewModel != timeViewModel) {
            d->timeViewModel = timeViewModel;
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            emit timeViewModelChanged();
        }
    }
    TimeLayoutViewModel *TimeManipulator::timeLayoutViewModel() const {
        Q_D(const TimeManipulator);
        return d->timeLayoutViewModel;
    }
    void TimeManipulator::setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel) {
        Q_D(TimeManipulator);
        if (d->timeLayoutViewModel != timeLayoutViewModel) {
            d->timeLayoutViewModel = timeLayoutViewModel;
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            emit timeLayoutViewModelChanged();
        }
    }
    QObject *TimeManipulator::parent() const {
        return QObject::parent();
    }
    void TimeManipulator::setParent(QObject *parent) {
        Q_D(TimeManipulator);
        if (parent != QObject::parent()) {
            if (QObject::parent()) {
                disconnect(QObject::parent(), nullptr, this, nullptr);
            }
            QObject::setParent(parent);
            d->updateParent();
            emit parentChanged();
        }
    }
    double TimeManipulator::viewSize() const {
        Q_D(const TimeManipulator);
        return d->viewSize;
    }
    void TimeManipulator::setViewSize(double viewSize) {
        Q_D(TimeManipulator);
        d->isViewSizeExplicitSet = true;
        d->setViewSize(viewSize);
    }
    void TimeManipulator::resetViewSize() {
        Q_D(TimeManipulator);
        d->isViewSizeExplicitSet = false;
        if (auto item = qobject_cast<QQuickItem *>(parent())) {
            d->setViewSize(item->width());
        }
    }
    void TimeManipulator::moveViewBy(double delta, bool animated, bool restrictEnd) {
        Q_D(TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return;
        auto newStart = qMax(0.0, d->timeViewModel->start() + delta / d->timeLayoutViewModel->pixelDensity());
        auto newEnd = newStart + d->viewSize / d->timeLayoutViewModel->pixelDensity();
        if (newEnd > d->timeViewModel->end()) {
            if (restrictEnd) {
                newStart += d->timeViewModel->end() - newEnd;
                if (newStart < 0)
                    return;
            } else {
                d->timeViewModel->setProperty("end", newEnd);
            }
        }
        if (!animated) {
            d->timeViewModel->setProperty("start", newStart);
        } else {
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            d->startAnimation.setStartValue(d->timeViewModel->start());
            d->startAnimation.setEndValue(newStart);
            d->startAnimation.start();
        }
    }
    void TimeManipulator::zoomViewBy(double ratio, double center, bool animated, bool restrictEnd) {
        Q_D(TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return;
        auto newPixelDensity = qMin(qMax(d->timeLayoutViewModel->minimumPixelDensity(), d->timeLayoutViewModel->pixelDensity() * ratio), d->timeLayoutViewModel->maximumPixelDensity());
        auto newStart = qMax(0.0, d->timeViewModel->start() + center / d->timeLayoutViewModel->pixelDensity() - center / newPixelDensity);
        auto newEnd = newStart + d->viewSize / newPixelDensity;
        if (newEnd > d->timeViewModel->end()) {
            if (restrictEnd) {
                newStart += d->timeViewModel->end() - newEnd;
                if (newStart < 0)
                    return;
            } else {
                d->timeViewModel->setProperty("end", newEnd);
            }
        }
        if (!animated) {
            d->timeViewModel->setProperty("start", newStart);
            d->timeLayoutViewModel->setProperty("pixelDensity", newPixelDensity);
        } else {
            d->currentAnimationFixStartToZero = ratio < 1 && qFuzzyIsNull(d->timeViewModel->start());
            d->animationCenter = center;
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            d->pixelDensityAnimation.setStartValue(d->timeLayoutViewModel->pixelDensity());
            d->pixelDensityAnimation.setEndValue(newPixelDensity);
            d->pixelDensityAnimation.start();
        }
    }
    void TimeManipulator::ensureVisible(int position, int length, double leftPadding,
                                        double rightPadding, bool animated, bool restrictEnd) {
        Q_D(TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return;
        auto itemRangeStart = position - leftPadding / d->timeLayoutViewModel->pixelDensity();
        auto itemRangeEnd =
            position + length + rightPadding / d->timeLayoutViewModel->pixelDensity();
        auto currentRangeStart = d->timeViewModel->start();
        auto currentRangeEnd =
            d->timeViewModel->start() + d->viewSize / d->timeLayoutViewModel->pixelDensity();
        if (itemRangeStart < currentRangeStart) {
            moveViewBy((itemRangeStart - currentRangeStart) *
                           d->timeLayoutViewModel->pixelDensity(),
                       animated, restrictEnd);
        } else if (itemRangeEnd > currentRangeEnd) {
            moveViewBy((itemRangeEnd - currentRangeEnd) * d->timeLayoutViewModel->pixelDensity(),
                       animated, restrictEnd);
        }
    }
    int TimeManipulator::alignTick(int tick, ScopicFlow::AlignOption alignOption) const {
        Q_D(const TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return tick;
        auto align = d->timeLayoutViewModel->positionAlignment();
        switch (alignOption) {
            case ScopicFlow::AO_Round:
                return (tick + align / 2) / align * align;
            case ScopicFlow::AO_Floor:
                return tick / align * align;
            case ScopicFlow::AO_Ceil:
                return (tick + align - 1) / align * align;
            case ScopicFlow::AO_Visible: {
                tick = alignTick(qMax(0, tick), ScopicFlow::AO_Round);
                auto pos =  mapToPosition(tick);
                if (pos < 0)
                    tick += align;
                else if (pos > viewSize())
                    tick -= align;
                return tick;
            }
            default:
                return tick;
        }
    }
    int TimeManipulator::mapToTick(double position) const {
        Q_D(const TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return 0;
        return qRound(d->timeViewModel->start() + position / d->timeLayoutViewModel->pixelDensity());
    }
    double TimeManipulator::mapToPosition(int tick) const {
        Q_D(const TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return 0;
        return (tick - d->timeViewModel->start()) * d->timeLayoutViewModel->pixelDensity();
    }
}

#include "moc_TimeManipulator.cpp"