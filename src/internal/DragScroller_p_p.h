#ifndef SCOPIC_FLOW_DRAGSCROLLER_P_P_H
#define SCOPIC_FLOW_DRAGSCROLLER_P_P_H

#include <ScopicFlowInternal/private/DragScroller_p.h>

#include <QTimer>

namespace sflow {

    class DragScrollerPrivate {
        Q_DECLARE_PUBLIC(DragScroller)

    public:
        explicit DragScrollerPrivate(DragScroller *q);

        void onTimeout();

        DragScroller *q_ptr = nullptr;
        double distanceX = 0;
        double distanceY = 0;
        bool running = false;
        QTimer *tickingTimer{};
    };

}

#endif // SCOPIC_FLOW_DRAGSCROLLER_P_P_H
