#ifndef SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_P_H
#define SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_P_H

#include <ScopicFlowInternal/private/PianoRollScaleQuickItem_p.h>

#include <SVSCraftCore/musictimeline.h>

namespace sflow {
    class PianoRollScaleQuickItemPrivate {
        Q_DECLARE_PUBLIC(PianoRollScaleQuickItem)
    public:
        PianoRollScaleQuickItem *q_ptr;

        TimeViewModel *timeViewModel = nullptr;
        TimeLayoutViewModel *timeLayoutViewModel = nullptr;
        QPointer<SVS::MusicTimeline> timeline = nullptr;

        QColor beatScaleColor;
        QColor barScaleColor;
        QColor segmentScaleColor;

        double tickToX(int tick) const;

        enum ScaleType {
            Bar, Beat, Segment
        };
        QList<QPair<float, ScaleType>> xList;
    };
}

#endif //SCOPIC_FLOW_PIANOROLLSCALEQUICKITEM_P_P_H
