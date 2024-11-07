#ifndef SCOPIC_FLOW_TRACKLISTQUICKITEM_P_P_H
#define SCOPIC_FLOW_TRACKLISTQUICKITEM_P_P_H

#include <ScopicFlow/private/TrackListQuickItem_p.h>

namespace sflow {
    class TrackListQuickItemPrivate {
        Q_DECLARE_PUBLIC(TrackListQuickItem)
    public:
        TrackListQuickItem *q_ptr;

        TrackListViewModel *trackListViewModel = nullptr;
    };
}

#endif //SCOPIC_FLOW_TRACKLISTQUICKITEM_P_P_H
