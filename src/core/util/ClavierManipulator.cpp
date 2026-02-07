#include "ClavierManipulator.h"
#include "ClavierManipulator_p.h"

#include <QQuickItem>

#include <ScopicFlowCore/ClavierViewModel.h>

namespace sflow {

    void ClavierManipulatorPrivate::setViewSize(double size) {
        Q_Q(ClavierManipulator);
        if (viewSize == size)
            return;
        viewSize = size;
        startAnimation.stop();
        pixelDensityAnimation.stop();
        emit q->viewSizeChanged();
    }
    void ClavierManipulatorPrivate::updateParent() {
        Q_Q(ClavierManipulator);
        if (target) {
            QObject::connect(target, &QQuickItem::heightChanged, q, [=, this] {
                if (!isViewSizeExplicitSet)
                    setViewSize(target->height());
            });
            setViewSize(target->height());
        }
    }

    ClavierManipulator::ClavierManipulator(QObject *parent) : QObject(parent), d_ptr(new ClavierManipulatorPrivate) {
        Q_D(ClavierManipulator);
        d->q_ptr = this;
        d->startAnimation.setEasingCurve(QEasingCurve::OutCubic);
        d->pixelDensityAnimation.setEasingCurve(QEasingCurve::OutCubic);
        connect(&d->startAnimation, &QVariantAnimation::valueChanged, this, [d](const QVariant& value) {
            if (!d->clavierViewModel)
                return;
            d->clavierViewModel->setStart(value.toDouble());
        });
        connect(&d->pixelDensityAnimation, &QVariantAnimation::valueChanged, this, [d](const QVariant& value) {
            if (!d->clavierViewModel)
                return;
            // Note: Inverted direction - negative pixelDensity effect
            d->clavierViewModel->setStart(d->clavierViewModel->start() - d->animationCenter / d->clavierViewModel->pixelDensity() + d->animationCenter / value.toDouble());
            d->clavierViewModel->setPixelDensity(value.toDouble());
        });
        d->updateParent();
    }
    ClavierManipulator::~ClavierManipulator() = default;

    ClavierViewModel *ClavierManipulator::clavierViewModel() const {
        Q_D(const ClavierManipulator);
        return d->clavierViewModel;
    }
    void ClavierManipulator::setClavierViewModel(ClavierViewModel *clavierViewModel) {
        Q_D(ClavierManipulator);
        if (d->clavierViewModel != clavierViewModel) {
            d->clavierViewModel = clavierViewModel;
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            emit clavierViewModelChanged();
        }
    }
    QQuickItem *ClavierManipulator::target() const {
        Q_D(const ClavierManipulator);
        return d->target;
    }
    void ClavierManipulator::setTarget(QQuickItem *target) {
        Q_D(ClavierManipulator);
        if (target != d->target) {
            if (d->target) {
                disconnect(d->target, nullptr, this, nullptr);
            }
            d->target = target;
            d->updateParent();
            emit targetChanged();
        }
    }
    double ClavierManipulator::viewSize() const {
        Q_D(const ClavierManipulator);
        return d->viewSize;
    }
    void ClavierManipulator::setViewSize(double viewSize) {
        Q_D(ClavierManipulator);
        d->isViewSizeExplicitSet = true;
        d->setViewSize(viewSize);
    }
    void ClavierManipulator::resetViewSize() {
        Q_D(ClavierManipulator);
        d->isViewSizeExplicitSet = false;
        if (auto item = qobject_cast<QQuickItem *>(parent())) {
            d->setViewSize(item->height());
        }
    }
    void ClavierManipulator::moveViewBy(double delta, bool animated) {
        Q_D(ClavierManipulator);
        if (!d->clavierViewModel)
            return;
        // Note: Inverted direction - positive delta moves view down (decreases start position)
        // Because pixelDensity is implicitly negative
        auto newStart = d->clavierViewModel->start() - delta / d->clavierViewModel->pixelDensity();
        auto newEnd = newStart - d->viewSize / d->clavierViewModel->pixelDensity();
        
        // Clamp to [0, 128] range - note inverted: start > end
        if (newStart > 128) {
            newStart = 128;
        }
        if (newEnd < 0) {
            newStart = d->viewSize / d->clavierViewModel->pixelDensity();
        }
        if (newStart < 0) {
            newStart = 0;
        }
        
        if (!animated) {
            d->clavierViewModel->setStart(newStart);
        } else {
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            d->startAnimation.setStartValue(d->clavierViewModel->start());
            d->startAnimation.setEndValue(newStart);
            d->startAnimation.start();
        }
    }
    void ClavierManipulator::zoomViewBy(double ratio, double center, bool animated) {
        Q_D(ClavierManipulator);
        if (!d->clavierViewModel)
            return;
        auto newPixelDensity = qMin(qMax(d->clavierViewModel->minimumPixelDensity(), d->clavierViewModel->pixelDensity() * ratio), d->clavierViewModel->maximumPixelDensity());
        // Note: Inverted direction - maintain center point with negative pixelDensity
        auto newStart = d->clavierViewModel->start() - center / d->clavierViewModel->pixelDensity() + center / newPixelDensity;
        auto newEnd = newStart - d->viewSize / newPixelDensity;
        
        // Clamp to [0, 128] range - note inverted: start > end
        if (newStart > 128) {
            newStart = 128;
        }
        if (newEnd < 0) {
            newStart = d->viewSize / newPixelDensity;
        }
        if (newStart < 0) {
            newStart = 0;
        }
        
        if (!animated) {
            d->clavierViewModel->setStart(newStart);
            d->clavierViewModel->setPixelDensity(newPixelDensity);
        } else {
            d->animationCenter = center;
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            d->pixelDensityAnimation.setStartValue(d->clavierViewModel->pixelDensity());
            d->pixelDensityAnimation.setEndValue(newPixelDensity);
            d->pixelDensityAnimation.start();
        }
    }
    void ClavierManipulator::ensureVisible(int position, int length, double topPadding,
                                           double bottomPadding, bool animated) {
        Q_D(ClavierManipulator);
        if (!d->clavierViewModel)
            return;
        // Note: Inverted direction - larger positions have smaller Y coordinates
        // position is at the "top" (smaller Y), position + length is at the "bottom" (larger Y)
        auto itemRangeTop = position + length + topPadding / d->clavierViewModel->pixelDensity();
        auto itemRangeBottom = position - bottomPadding / d->clavierViewModel->pixelDensity();
        auto currentRangeTop = d->clavierViewModel->start();
        auto currentRangeBottom = d->clavierViewModel->start() - d->viewSize / d->clavierViewModel->pixelDensity();
        
        if (itemRangeTop > currentRangeTop) {
            // Item extends beyond top of view - need to move view up (negative delta)
            moveViewBy(-(itemRangeTop - currentRangeTop) * d->clavierViewModel->pixelDensity(), animated);
        } else if (itemRangeBottom < currentRangeBottom) {
            // Item extends beyond bottom of view - need to move view down (positive delta)
            moveViewBy(-(itemRangeBottom - currentRangeBottom) * d->clavierViewModel->pixelDensity(), animated);
        }
    }
    double ClavierManipulator::mapToPosition(double y) const {
        Q_D(const ClavierManipulator);
        if (!d->clavierViewModel)
            return 0;
        // Note: Inverted direction - larger Y means smaller position
        return d->clavierViewModel->start() - y / d->clavierViewModel->pixelDensity();
    }
    double ClavierManipulator::mapToY(int position) const {
        Q_D(const ClavierManipulator);
        if (!d->clavierViewModel)
            return 0;
        // Note: Inverted direction - larger position means smaller Y
        return (d->clavierViewModel->start() - position) * d->clavierViewModel->pixelDensity();
    }
}

#include "moc_ClavierManipulator.cpp"
