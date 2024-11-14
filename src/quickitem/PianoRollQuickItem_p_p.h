#ifndef SCOPIC_FLOW_PIANOROLLQUICKITEM_P_P_H
#define SCOPIC_FLOW_PIANOROLLQUICKITEM_P_P_H

#include <ScopicFlow/private/PianoRollQuickItem_p.h>

namespace sflow {
    class PianoRollQuickItemPrivate {
        Q_DECLARE_PUBLIC(PianoRollQuickItem)
    public:
        PianoRollQuickItem *q_ptr;

        TimeAlignmentViewModel *timeAlignmentViewModel = nullptr;
        PlaybackViewModel *playbackViewModel = nullptr;
        ClavierViewModel *clavierViewModel = nullptr;
        ScrollBehaviorViewModel *scrollBehaviorViewModel = nullptr;
        AnimationViewModel *animationViewModel = nullptr;
        PaletteViewModel *paletteViewModel = nullptr;
    };
}

#endif //SCOPIC_FLOW_PIANOROLLQUICKITEM_P_P_H
