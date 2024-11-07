#include "TrackListQuickItem_p.h"
#include "TrackListQuickItem_p_p.h"

#include <ScopicFlow/TrackViewModel.h>

namespace sflow {
    TrackListQuickItem::TrackListQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TrackListQuickItemPrivate) {
        Q_D(TrackListQuickItem);
        d->q_ptr = this;
    }
    TrackListQuickItem::~TrackListQuickItem() {
    }
    TrackListViewModel *TrackListQuickItem::trackListViewModel() const {
        Q_D(const TrackListQuickItem);
        return d->trackListViewModel;
    }
    void TrackListQuickItem::setTrackListViewModel(TrackListViewModel *trackListViewModel) {
        Q_D(TrackListQuickItem);
        if (d->trackListViewModel == trackListViewModel)
            return;
        d->trackListViewModel = trackListViewModel;
        emit trackListViewModelChanged();
    }
    QObject *TrackListQuickItem::trackAt(int index) const {
        Q_D(const TrackListQuickItem);
        if (!d->trackListViewModel)
            return nullptr;
        return d->trackListViewModel->trackAt(index);
    }
} // sflow