#ifndef SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_P_H
#define SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_P_H

#include <ScopicFlow/private/LabelSequenceQuickItem_p.h>

namespace sflow {
    class LabelSequenceQuickItemPrivate {
        Q_DECLARE_PUBLIC(LabelSequenceQuickItem)
    public:
        LabelSequenceQuickItem *q_ptr;

        TimeAlignmentViewModel *timeAlignmentViewModel = nullptr;
        PlaybackViewModel *playbackViewModel = nullptr;
        ScrollBehaviorViewModel *scrollBehaviorViewModel = nullptr;
        AnimationViewModel *animationViewModel = nullptr;
        PaletteViewModel *paletteViewModel = nullptr;

        LabelSequenceViewModel *labelSequenceViewModel = nullptr;
    };
}

#endif //SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_P_H
