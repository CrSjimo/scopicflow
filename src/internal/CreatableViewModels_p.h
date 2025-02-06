#ifndef SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H
#define SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H

#include <qqmlregistration.h>

#include <ScopicFlow/LabelViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/PianoRollNoteAreaBehaviorViewModel.h>
#include <ScopicFlow/NoteViewModel.h>
#include <ScopicFlow/ClipPaneBehaviorViewModel.h>
#include <ScopicFlow/ClipViewModel.h>

namespace sflow {
    struct LabelViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(LabelViewModel)
        QML_FOREIGN(LabelViewModel)
    };
    struct ClavierViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(ClavierViewModel)
        QML_FOREIGN(ClavierViewModel)
    };
    struct PianoRollNoteAreaBehaviorViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(PianoRollNoteAreaBehaviorViewModel)
        QML_FOREIGN(PianoRollNoteAreaBehaviorViewModel)
    };
    struct NoteViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(NoteViewModel)
        QML_FOREIGN(NoteViewModel)
    };
    struct ClipPaneBehaviorViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(ClipPaneBehaviorViewModel)
        QML_FOREIGN(ClipPaneBehaviorViewModel)
    };
    struct ClipViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(ClipViewModel)
        QML_FOREIGN(ClipViewModel)
    };
}

#endif //SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H