#ifndef SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_P_H
#define SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_P_H

#include <ScopicFlowInternal/private/PianoRollNoteAreaRubberBandHelper_p.h>

#include <QPointer>

namespace sflow {

    class SliceableViewModelQmlHandle;

    class PianoRollNoteAreaRubberBandHelperPrivate {
        Q_DECLARE_PUBLIC(PianoRollNoteAreaRubberBandHelper)
    public:
        PianoRollNoteAreaRubberBandHelper *q_ptr;
        QPointer<RangeSequenceViewModel> noteSequenceViewModel;
        QPointer<SliceableViewModelQmlHandle> handle;
        QPointer<RubberBandLayerQuickItem> rubberBandLayer;

        void handleItemUpdated();
        void handleItemUpdated(QObject *item);
        void handleItemInserted(QObject *item);
        void handleItemRemoved(QObject *item);
    };
}

#endif //SCOPIC_FLOW_PIANOROLLNOTEAREARUBBERBANDHELPER_P_P_H
