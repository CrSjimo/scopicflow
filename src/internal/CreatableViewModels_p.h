#ifndef SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H
#define SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H

#include <qqmlregistration.h>

#include <ScopicFlow/LabelViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/PianoRollNoteAreaBehaviorViewModel.h>
#include <ScopicFlow/NoteViewModel.h>

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
}

#endif //SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H