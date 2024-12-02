#ifndef SCOPIC_FLOW_TRACKLISTVIEWMODEL_P_H
#define SCOPIC_FLOW_TRACKLISTVIEWMODEL_P_H

#include <ScopicFlow/TrackListViewModel.h>

namespace sflow {
    class TrackListViewModelPrivate {
        Q_DECLARE_PUBLIC(TrackListViewModel)
    public:
        TrackListViewModel *q_ptr;
        QList<TrackViewModel *> tracks;
        int currentIndex = 0;
        bool intermediate = false;
        double viewportOffset = 0;

        QObjectList indexObjects;
    };
}

#endif //SCOPIC_FLOW_TRACKLISTVIEWMODEL_P_H
