#include "TrackListManipulator.h"
#include "TrackListManipulator_p.h"

#include <QQuickItem>

#include <ScopicFlowCore/TrackListLayoutViewModel.h>
#include <ScopicFlowCore/document/ListViewModel.h>

namespace sflow {

    void TrackListManipulatorPrivate::setViewSize(double size) {
        Q_Q(TrackListManipulator);
        if (viewSize == size)
            return;
        viewSize = size;
        emit q->viewSizeChanged();
    }

    void TrackListManipulatorPrivate::updateParent() {
        Q_Q(TrackListManipulator);
        if (target) {
            QObject::connect(target, &QQuickItem::heightChanged, q, [=, this] {
                if (!isViewSizeExplicitSet)
                    setViewSize(target->height());
            });
            setViewSize(target->height());
        }
    }

    TrackListManipulator::TrackListManipulator(QObject *parent) : QObject(parent), d_ptr(new TrackListManipulatorPrivate) {
        Q_D(TrackListManipulator);
        d->q_ptr = this;
        d->viewportOffsetAnimation.setEasingCurve(QEasingCurve::OutCubic);
        d->viewportOffsetAnimation.setDuration(250);
        connect(&d->viewportOffsetAnimation, &QVariantAnimation::valueChanged, this, [d](const QVariant& value) {
            if (!d->trackListLayoutViewModel)
                return;
            d->trackListLayoutViewModel->setViewportOffset(value.toDouble());
        });
        d->updateParent();
    }

    TrackListManipulator::~TrackListManipulator() = default;

    TrackListLayoutViewModel *TrackListManipulator::trackListLayoutViewModel() const {
        Q_D(const TrackListManipulator);
        return d->trackListLayoutViewModel;
    }

    void TrackListManipulator::setTrackListLayoutViewModel(TrackListLayoutViewModel *trackListLayoutViewModel) {
        Q_D(TrackListManipulator);
        if (d->trackListLayoutViewModel != trackListLayoutViewModel) {
            d->trackListLayoutViewModel = trackListLayoutViewModel;
            emit trackListLayoutViewModelChanged();
        }
    }

    ListViewModel *TrackListManipulator::trackListViewModel() const {
        Q_D(const TrackListManipulator);
        return d->trackListViewModel;
    }

    void TrackListManipulator::setTrackListViewModel(ListViewModel *trackListViewModel) {
        Q_D(TrackListManipulator);
        if (d->trackListViewModel != trackListViewModel) {
            d->trackListViewModel = trackListViewModel;
            emit trackListViewModelChanged();
        }
    }

    QQuickItem *TrackListManipulator::target() const {
        Q_D(const TrackListManipulator);
        return d->target;
    }

    void TrackListManipulator::setTarget(QQuickItem *target) {
        Q_D(TrackListManipulator);
        if (target != d->target) {
            if (d->target) {
                disconnect(d->target, nullptr, this, nullptr);
            }
            d->target = target;
            d->updateParent();
            emit targetChanged();
        }
    }

    double TrackListManipulator::viewSize() const {
        Q_D(const TrackListManipulator);
        return d->viewSize;
    }

    void TrackListManipulator::setViewSize(double viewSize) {
        Q_D(TrackListManipulator);
        d->isViewSizeExplicitSet = true;
        d->setViewSize(viewSize);
    }

    void TrackListManipulator::resetViewSize() {
        Q_D(TrackListManipulator);
        d->isViewSizeExplicitSet = false;
        if (auto item = qobject_cast<QQuickItem *>(parent())) {
            d->setViewSize(item->height());
        }
    }

    double TrackListManipulator::viewportHeight() const {
        Q_D(const TrackListManipulator);
        return d->viewportHeight;
    }

    void TrackListManipulator::setViewportHeight(double viewportHeight) {
        Q_D(TrackListManipulator);
        if (qFuzzyCompare(d->viewportHeight, viewportHeight))
            return;
        d->viewportHeight = viewportHeight;
        emit viewportHeightChanged();
    }

    void TrackListManipulator::moveViewBy(double deltaY, bool animated) {
        Q_D(TrackListManipulator);
        if (!d->trackListLayoutViewModel)
            return;

        auto currentOffset = d->trackListLayoutViewModel->viewportOffset();
        auto maxOffset = qMax(0.0, d->viewportHeight - d->viewSize);
        auto newOffset = qBound(0.0, currentOffset + deltaY, maxOffset);
        
        if (!animated) {
            d->trackListLayoutViewModel->setViewportOffset(newOffset);
        } else {
            d->viewportOffsetAnimation.stop();
            d->viewportOffsetAnimation.setStartValue(currentOffset);
            d->viewportOffsetAnimation.setEndValue(newOffset);
            d->viewportOffsetAnimation.start();
        }
    }

    int TrackListManipulator::mapToPosition(double y) const {
        // TODO: Implementation to be added
        return 0;
    }

    double TrackListManipulator::mapToY(int position) const {
        // TODO: Implementation to be added
        return 0.0;
    }

}
