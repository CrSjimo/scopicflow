#ifndef SCOPIC_FLOW_TIMELINE_P_H
#define SCOPIC_FLOW_TIMELINE_P_H

#include <ScopicFlow/Timeline.h>

namespace sflow {

    class TimelinePrivate {
        Q_DECLARE_PUBLIC(Timeline)
    public:
        Timeline *q_ptr;

        TimelinePalette *palette = nullptr;
        TimeViewModel *timeViewModel = nullptr;
    };

}

#endif //SCOPIC_FLOW_TIMELINE_P_H
