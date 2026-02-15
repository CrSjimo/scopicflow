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
        if (target) {
            QObject::connect(target, &QQuickItem::widthChanged, q, [=, this] {
                if (!isViewSizeExplicitSet)
                    setViewSize(target->width());
            });
            setViewSize(target->width());
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
            d->timeViewModel->setStart(value.toDouble());
        });
        connect(&d->pixelDensityAnimation, &QVariantAnimation::valueChanged, this, [d](const QVariant& value) {
            if (!d->timeViewModel || !d->timeLayoutViewModel)
                return;
            d->timeViewModel->setStart(qFuzzyIsNull(d->timeViewModel->start() - d->startOffset) && d->currentAnimationFixStartToZero ? d->startOffset : qMax(d->startOffset, d->timeViewModel->start() + d->animationCenter / d->timeLayoutViewModel->pixelDensity() - d->animationCenter / value.toDouble()));
            d->timeLayoutViewModel->setPixelDensity(value.toDouble());
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
    QQuickItem *TimeManipulator::target() const {
        Q_D(const TimeManipulator);
        return d->target;
    }
    void TimeManipulator::setTarget(QQuickItem *target) {
        Q_D(TimeManipulator);
        if (target != d->target) {
            if (d->target) {
                disconnect(d->target, nullptr, this, nullptr);
            }
            d->target = target;
            d->updateParent();
            emit targetChanged();
        }
    }
    double TimeManipulator::startOffset() const {
        Q_D(const TimeManipulator);
        return d->startOffset;
    }
    void TimeManipulator::setStartOffset(double startOffset) {
        Q_D(TimeManipulator);
        if (d->startOffset != startOffset) {
            d->startOffset = startOffset;
            emit startOffsetChanged();
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
        auto newStart = qMax(d->startOffset, d->timeViewModel->start() + delta / d->timeLayoutViewModel->pixelDensity());
        auto newEnd = newStart + d->viewSize / d->timeLayoutViewModel->pixelDensity();
        if (newEnd > d->timeViewModel->end()) {
            if (restrictEnd) {
                newStart += d->timeViewModel->end() - newEnd;
                if (newStart < d->startOffset)
                    return;
            } else {
                d->timeViewModel->setEnd(newEnd);
            }
        }
        if (!animated) {
            d->timeViewModel->setStart(newStart);
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
        auto newStart = qMax(d->startOffset, d->timeViewModel->start() + center / d->timeLayoutViewModel->pixelDensity() - center / newPixelDensity);
        auto newEnd = newStart + d->viewSize / newPixelDensity;
        if (newEnd > d->timeViewModel->end()) {
            if (restrictEnd) {
                newStart += d->timeViewModel->end() - newEnd;
                if (newStart < d->startOffset)
                    return;
            } else {
                d->timeViewModel->setEnd(newEnd);
            }
        }
        if (!animated) {
            d->timeViewModel->setStart(newStart);
            d->timeLayoutViewModel->setPixelDensity(newPixelDensity);
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
    int TimeManipulator::alignPosition(int tick, ScopicFlow::AlignOption alignOption) const {
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
                tick = alignPosition(qMax(0, tick), ScopicFlow::AO_Round);
                auto pos =  mapToX(tick);
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
    int TimeManipulator::mapToPosition(double position) const {
        Q_D(const TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return 0;
        return qRound(d->timeViewModel->start() + position / d->timeLayoutViewModel->pixelDensity());
    }
    double TimeManipulator::mapToX(int position) const {
        Q_D(const TimeManipulator);
        if (!d->timeViewModel || !d->timeLayoutViewModel)
            return 0;
        return (position - d->timeViewModel->start()) * d->timeLayoutViewModel->pixelDensity();
    }
}

#include "moc_TimeManipulator.cpp"
