#include "TrackListManipulator.h"
#include "TrackListManipulator_p.h"

#include <algorithm>

#include <QQuickItem>

#include <ScopicFlowCore/TrackListLayoutViewModel.h>
#include <ScopicFlowCore/ListViewModel.h>
#include <ScopicFlowCore/TrackViewModel.h>

namespace sflow {

    void TrackListManipulatorPrivate::setViewportHeight(double height) {
        Q_Q(TrackListManipulator);
        if (qFuzzyCompare(viewportHeight, height))
            return;
        viewportHeight = height;
        emit q->viewportHeightChanged();
    }

    double TrackListManipulatorPrivate::itemHeight(QObject *object) const {
        if (auto track = qobject_cast<TrackViewModel *>(object))
            return qMax(0.0, track->rowHeight());
        return 0.0;
    }

    void TrackListManipulatorPrivate::clearItemConnections() {
        for (const auto &connection : itemConnections) {
            QObject::disconnect(connection);
        }
        itemConnections.clear();
    }

    void TrackListManipulatorPrivate::rebuildFromModel() {
        clearItemConnections();
        itemHeights.clear();
        prefixDirty = true;

        if (!trackListViewModel) {
            setViewportHeight(0.0);
            prefixSums = {0.0};
            return;
        }

        const auto items = trackListViewModel->items();
        double newHeight = 0.0;
        itemHeights.reserve(items.size());
        for (auto *item : items) {
            const double height = itemHeight(item);
            itemHeights.insert(item, height);
            newHeight += height;

            if (auto track = qobject_cast<TrackViewModel *>(item)) {
                itemConnections << QObject::connect(track, &TrackViewModel::rowHeightChanged, q_ptr, [this, track] {
                    handleRowHeightChanged(track);
                });
            }
        }

        setViewportHeight(newHeight);
        prefixSums.clear();
    }

    void TrackListManipulatorPrivate::handleRowHeightChanged(QObject *object) {
        const double oldHeight = itemHeights.value(object, 0.0);
        const double newHeight = itemHeight(object);

        if (qFuzzyCompare(oldHeight, newHeight))
            return;

        itemHeights.insert(object, newHeight);
        setViewportHeight(viewportHeight + newHeight - oldHeight);
        prefixDirty = true;
    }

    void TrackListManipulatorPrivate::ensurePrefixReady() const {
        if (!prefixDirty)
            return;

        prefixSums.clear();
        prefixSums.reserve(itemHeights.size() + 1);
        prefixSums.push_back(0.0);

        if (trackListViewModel) {
            const auto items = trackListViewModel->items();
            for (auto *item : items) {
                const double height = itemHeights.value(item, itemHeight(item));
                prefixSums.push_back(prefixSums.back() + height);
                itemHeights.insert(item, height);
            }
        }

        prefixDirty = false;
    }

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
            if (d->itemsChangedConnection)
                disconnect(d->itemsChangedConnection);
            d->clearItemConnections();
            d->trackListViewModel = trackListViewModel;
            if (d->trackListViewModel) {
                d->itemsChangedConnection = connect(d->trackListViewModel, &ListViewModel::itemsChanged, this, [d] {
                    d->rebuildFromModel();
                });
            } else {
                d->itemsChangedConnection = {};
            }
            d->rebuildFromModel();
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
        Q_D(const TrackListManipulator);
        if (!d->trackListViewModel || d->trackListViewModel->count() == 0)
            return 0;

        d->ensurePrefixReady();
        const auto &prefix = d->prefixSums;
        const int count = d->trackListViewModel->count();
        const double totalHeight = prefix.isEmpty() ? 0.0 : prefix.constLast();

        if (y < 0.0)
            return -1;
        if (y >= totalHeight)
            return count;

        auto it = std::upper_bound(prefix.begin(), prefix.end(), y);
        const auto index = static_cast<int>(std::max<std::ptrdiff_t>(0, std::distance(prefix.begin(), it) - 1));
        return index;
    }

    double TrackListManipulator::mapToY(int position) const {
        Q_D(const TrackListManipulator);
        if (!d->trackListViewModel || d->trackListViewModel->count() == 0)
            return 0.0;

        d->ensurePrefixReady();
        const int count = d->trackListViewModel->count();
        const int clamped = qBound(0, position, count);
        return d->prefixSums.value(clamped, 0.0);
    }

}

#include "moc_TrackListManipulator.cpp"