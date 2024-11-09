#ifndef SCOPIC_FLOW_TRACKLISTQUICKITEM_P_P_H
#define SCOPIC_FLOW_TRACKLISTQUICKITEM_P_P_H

#include <ScopicFlow/private/TrackListQuickItem_p.h>

namespace sflow {
    class TrackListQuickItemPrivate {
        Q_DECLARE_PUBLIC(TrackListQuickItem)
    public:
        TrackListQuickItem *q_ptr;

        TrackListPalette *palette;
        TrackListViewModel *trackListViewModel = nullptr;

        ScrollBehaviorViewModel *scrollBehaviorViewModel = nullptr;
        AnimationViewModel *animationViewModel = nullptr;

        double totalHeight = 0.0;
        QList<double> handlePositionList;
        void calculateTotalHeight();
    };
}

#endif //SCOPIC_FLOW_TRACKLISTQUICKITEM_P_P_H
