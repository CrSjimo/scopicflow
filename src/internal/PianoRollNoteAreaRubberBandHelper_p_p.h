#ifndef SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_P_H
#define SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_P_H

#include <ScopicFlowInternal/private/PianoRollNoteAreaRubberBandHelper_p.h>

namespace sflow {

    class SliceableViewModelQmlHandle;

    class PianoRollNoteAreaRubberBandHelperPrivate {
        Q_DECLARE_PUBLIC(PianoRollNoteAreaRubberBandHelper)
    public:
        PianoRollNoteAreaRubberBandHelper *q_ptr;
        RangeSequenceViewModel *noteSequenceViewModel = nullptr;
        SliceableViewModelQmlHandle *handle = nullptr;
        RubberBandLayerQuickItem *rubberBandLayer = nullptr;

        void handleItemUpdated();
        void handleItemUpdated(QObject *item);
        void handleItemInserted(QObject *item);
        void handleItemRemoved(QObject *item);
    };
}

#endif //SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_P_H
