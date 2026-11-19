#ifndef SCOPIC_FLOW_ABSOLUTETIMELINESCALEQUICKITEM_P_P_H
#define SCOPIC_FLOW_ABSOLUTETIMELINESCALEQUICKITEM_P_P_H

#include <ScopicFlowInternal/private/AbsoluteTimelineScaleQuickItem_p.h>

#include <QColor>
#include <QFont>
#include <QPointer>

namespace SVS {
    class MusicTimeline;
}

namespace sflow {

    class AbsoluteTimelineScaleQuickItemPrivate {
        Q_DECLARE_PUBLIC(AbsoluteTimelineScaleQuickItem)
    public:
        AbsoluteTimelineScaleQuickItem *q_ptr = nullptr;

        QPointer<TimeViewModel> timeViewModel;
        QPointer<TimeLayoutViewModel> timeLayoutViewModel;
        QPointer<const SVS::MusicTimeline> timeline;

        QColor color = Qt::white;
        QFont font;

        void updateTimeline();
    };

}

#endif // SCOPIC_FLOW_ABSOLUTETIMELINESCALEQUICKITEM_P_P_H
