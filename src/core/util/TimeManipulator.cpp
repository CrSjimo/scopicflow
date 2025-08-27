#include "TimeManipulator.h"
#include "TimeManipulator_p.h"

#include <ScopicFlowCore/TimeViewModel.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>

namespace sflow {
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
    double TimeManipulator::viewSize() const {
        Q_D(const TimeManipulator);
        return d->viewSize;
    }
    void TimeManipulator::setViewSize(double viewSize) {
        Q_D(TimeManipulator);
        if (d->viewSize != viewSize) {
            d->viewSize = viewSize;
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            emit viewSizeChanged();
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
    void TimeManipulator::ensureVisible(int position, int length, double leftPadding, double rightPadding, bool animated, bool restrictEnd) {
        Q_D(TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return;
        auto itemRangeStart = position - leftPadding / d->timeLayoutViewModel->pixelDensity();
        auto itemRangeEnd = position + length + rightPadding / d->timeLayoutViewModel->pixelDensity();
        auto currentRangeStart = d->timeViewModel->start();
        auto currentRangeEnd = d->timeViewModel->start() + d->viewSize / d->timeLayoutViewModel->pixelDensity();
        if (itemRangeStart < currentRangeStart) {
            moveViewBy((itemRangeStart - currentRangeStart) * d->timeLayoutViewModel->pixelDensity(), animated, restrictEnd);
        } else if (itemRangeEnd > currentRangeEnd) {
            moveViewBy((itemRangeEnd - currentRangeEnd) * d->timeLayoutViewModel->pixelDensity(), animated, restrictEnd);
        }
    }
}

#include "moc_TimeManipulator.cpp"