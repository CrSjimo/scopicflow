#ifndef SCOPIC_FLOW_SCOPICFLOWFOREIGN_P_H
#define SCOPIC_FLOW_SCOPICFLOWFOREIGN_P_H

#include <qqmlregistration.h>

#include <ScopicFlowCore/LabelViewModel.h>
#include <ScopicFlowCore/ClavierViewModel.h>
#include <ScopicFlowCore/PianoRollNoteAreaBehaviorViewModel.h>
#include <ScopicFlowCore/NoteViewModel.h>
#include <ScopicFlowCore/ClipPaneBehaviorViewModel.h>
#include <ScopicFlowCore/ClipViewModel.h>

namespace sflow {
    struct LabelViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(LabelViewModel)
        QML_FOREIGN(LabelViewModel)
    };
    struct NoteViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(NoteViewModel)
        QML_FOREIGN(NoteViewModel)
    };
    struct ClipViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(ClipViewModel)
        QML_FOREIGN(ClipViewModel)
    };
}

#endif //SCOPIC_FLOW_SCOPICFLOWFOREIGN_P_H
