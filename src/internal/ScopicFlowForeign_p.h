#ifndef SCOPIC_FLOW_SCOPICFLOWFOREIGN_P_H
#define SCOPIC_FLOW_SCOPICFLOWFOREIGN_P_H

#include <qqmlregistration.h>

#include <ScopicFlow/LabelViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/PianoRollNoteAreaBehaviorViewModel.h>
#include <ScopicFlow/NoteViewModel.h>
#include <ScopicFlow/ClipPaneBehaviorViewModel.h>
#include <ScopicFlow/ClipViewModel.h>

namespace sflow {
    class ScopicFlowNamespaceForeign : public QObject {
        Q_OBJECT
        QML_NAMED_ELEMENT(ScopicFlow)
        QML_EXTENDED_NAMESPACE(sflow::ScopicFlow)
        QML_SINGLETON
    };
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