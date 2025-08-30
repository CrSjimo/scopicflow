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
        if (auto item = qobject_cast<QQuickItem *>(q->parent())) {
            QObject::connect(item, &QQuickItem::heightChanged, q, [=, this] {
                if (!isViewSizeExplicitSet)
                    setViewSize(item->height());
            });
            setViewSize(item->height());
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
            auto newPixelDensity = value.toDouble();
            auto newStart = d->currentAnimationFixStartToZero ? 0.0 : 
                           d->currentAnimationEnsureEnd ? 128.0 - (d->viewSize - d->topExpansion - d->bottomExpansion) / newPixelDensity :
                           qMax(0.0, d->clavierViewModel->start() + (d->viewSize - d->animationCenterY - d->topExpansion - d->bottomExpansion) / d->clavierViewModel->pixelDensity() - (d->viewSize - d->animationCenterY - d->topExpansion - d->bottomExpansion) / newPixelDensity);
            newStart = qMin(newStart, 128.0 - (d->viewSize - d->topExpansion - d->bottomExpansion) / newPixelDensity);
            d->clavierViewModel->setStart(newStart);
            d->clavierViewModel->setPixelDensity(newPixelDensity);
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
    
    QObject *ClavierManipulator::parent() const {
        return QObject::parent();
    }
    
    void ClavierManipulator::setParent(QObject *parent) {
        Q_D(ClavierManipulator);
        if (parent != QObject::parent()) {
            if (QObject::parent()) {
                disconnect(QObject::parent(), nullptr, this, nullptr);
            }
            QObject::setParent(parent);
            d->updateParent();
            emit parentChanged();
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
    
    double ClavierManipulator::bottomExpansion() const {
        Q_D(const ClavierManipulator);
        return d->bottomExpansion;
    }
    
    void ClavierManipulator::setBottomExpansion(double bottomExpansion) {
        Q_D(ClavierManipulator);
        if (d->bottomExpansion == bottomExpansion)
            return;
        d->bottomExpansion = bottomExpansion;
        emit bottomExpansionChanged();
    }

    double ClavierManipulator::topExpansion() const {
        Q_D(const ClavierManipulator);
        return d->topExpansion;
    }
    
    void ClavierManipulator::setTopExpansion(double topExpansion) {
        Q_D(ClavierManipulator);
        if (d->topExpansion == topExpansion)
            return;
        d->topExpansion = topExpansion;
        emit topExpansionChanged();
    }

    void ClavierManipulator::moveViewBy(double deltaY, bool animated) {
        Q_D(ClavierManipulator);
        if (!d->clavierViewModel)
            return;
        auto newStart = qMax(0.0, d->clavierViewModel->start() - deltaY / d->clavierViewModel->pixelDensity());
        newStart = qMin(newStart, 128.0 - (d->viewSize - d->topExpansion - d->bottomExpansion) / d->clavierViewModel->pixelDensity());
        
        if (!animated) {
            d->clavierViewModel->setStart(newStart);
        } else {
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            d->startAnimation.setStartValue(d->clavierViewModel->start());
            d->startAnimation.setEndValue(newStart);
            d->startAnimation.setDuration(250);
            d->startAnimation.start();
        }
    }
    
    void ClavierManipulator::zoomViewBy(double ratio, double centerY, bool animated) {
        Q_D(ClavierManipulator);
        if (!d->clavierViewModel)
            return;
        auto newPixelDensity = qMin(qMax(d->clavierViewModel->minimumPixelDensity(), d->clavierViewModel->pixelDensity() * ratio), d->clavierViewModel->maximumPixelDensity());
        auto newStart = qMax(0.0, d->clavierViewModel->start() + (d->viewSize - centerY - d->topExpansion - d->bottomExpansion) / d->clavierViewModel->pixelDensity() - (d->viewSize - centerY - d->topExpansion - d->bottomExpansion) / newPixelDensity);
        newStart = qMin(newStart, 128.0 - (d->viewSize - d->topExpansion - d->bottomExpansion) / newPixelDensity);
        
        if (!animated) {
            d->clavierViewModel->setStart(newStart);
            d->clavierViewModel->setPixelDensity(newPixelDensity);
        } else {
            d->currentAnimationFixStartToZero = ratio < 1 && qFuzzyIsNull(d->clavierViewModel->start());
            d->currentAnimationEnsureEnd = ratio < 1 && qFuzzyIsNull(d->clavierViewModel->start() - (128.0 - (d->viewSize - d->topExpansion - d->bottomExpansion) / d->clavierViewModel->pixelDensity()));
            d->animationCenterY = centerY;
            d->startAnimation.stop();
            d->pixelDensityAnimation.stop();
            d->pixelDensityAnimation.setStartValue(d->clavierViewModel->pixelDensity());
            d->pixelDensityAnimation.setEndValue(newPixelDensity);
            d->pixelDensityAnimation.setDuration(250);
            d->pixelDensityAnimation.start();
        }
    }

    int ClavierManipulator::mapToKey(double y) const {
        Q_D(const ClavierManipulator);
        if (!d->clavierViewModel)
            return 0;
        return qFloor(d->clavierViewModel->start() + (d->viewSize - d->bottomExpansion - y) / d->clavierViewModel->pixelDensity());
    }
}

#include "moc_ClavierManipulator.cpp"
