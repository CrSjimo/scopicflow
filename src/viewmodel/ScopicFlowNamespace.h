#ifndef SCOPIC_FLOW_SCOPICFLOWNAMESPACE_H
#define SCOPIC_FLOW_SCOPICFLOWNAMESPACE_H

#include <ScopicFlow/ScopicFlowGlobal.h>

#include <QMetaObject>

namespace sflow {

    namespace ScopicFlow {
        Q_NAMESPACE_EXPORT(SCOPIC_FLOW_EXPORT)

        enum LabelStrategy {
            LS_All,
            LS_C,
            LS_None
        };
        Q_ENUM_NS(LabelStrategy)

        enum MouseBehavior {
            MB_None,
            MB_Pointer,
            MB_Pen,
            MB_Eraser,
            MB_Scissor,
            MB_Pencil,
        };
        Q_ENUM_NS(MouseBehavior)

        enum AnchorType {
            AT_Break,
            AT_Free,
            AT_Linear,
            AT_Zero,
            AT_Pchip,
        };
        Q_ENUM_NS(AnchorType)


    }

}

#endif //SCOPIC_FLOW_SCOPICFLOWNAMESPACE_H
