#ifndef SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_P_H
#define SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_P_H

#include <ScopicFlow/private/PianoRollBackgroundQuickItem_p.h>

namespace sflow {
    class PianoRollBackgroundQuickItemPrivate {
        Q_DECLARE_PUBLIC(PianoRollBackgroundQuickItem)
    public:
        PianoRollBackgroundQuickItem *q_ptr;

        TimeAlignmentViewModel *timeAlignmentViewModel = nullptr;
        SVS::MusicTimeline *timeline = nullptr;

        QColor scaleColor;
        QColor barScaleColor;
        QColor segmentScaleColor;
    };
}

#endif //SCOPIC_FLOW_PIANOROLLBACKGROUNDQUICKITEM_P_P_H
