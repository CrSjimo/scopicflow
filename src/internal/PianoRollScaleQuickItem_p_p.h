#ifndef SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_P_H
#define SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_P_H

#include <ScopicFlowInternal/private/PianoRollScaleQuickItem_p.h>

#include <SVSCraftCore/MusicTimeline.h>

namespace sflow {
    class PianoRollScaleQuickItemPrivate {
        Q_DECLARE_PUBLIC(PianoRollScaleQuickItem)
    public:
        PianoRollScaleQuickItem *q_ptr;

        QPointer<TimeViewModel> timeViewModel;
        QPointer<TimeLayoutViewModel> timeLayoutViewModel;
        QPointer<SVS::MusicTimeline> timeline;

        QColor beatScaleColor;
        QColor barScaleColor;
        QColor segmentScaleColor;

        double tickToX(int tick) const;

        enum ScaleType {
            Bar, Beat, Segment
        };
        QList<QPair<float, ScaleType>> xList;

        void updateTimeline();
    };
}

#endif //SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_P_H
