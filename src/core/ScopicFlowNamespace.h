#ifndef SCOPIC_FLOW_SCOPICFLOWNAMESPACE_H
#define SCOPIC_FLOW_SCOPICFLOWNAMESPACE_H

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

#include <QMetaObject>
#include <qqmlintegration.h>

namespace sflow {

    namespace ScopicFlow {
        Q_NAMESPACE_EXPORT(SCOPIC_FLOW_CORE_EXPORT)
        QML_ELEMENT

        enum AlignOption {
            AO_Round,
            AO_Floor,
            AO_Ceil,
            AO_Visible,
        };
        Q_ENUM_NS(AlignOption)

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
            MB_Converter,
        };
        Q_ENUM_NS(MouseBehavior)

        enum AnchorType {
            AT_Break,
            AT_Linear,
            AT_Zero,
            AT_Pchip,
        };
        Q_ENUM_NS(AnchorType)

        enum AnchorNodeFlag {
            AN_LeftCircle = 0x0,
            AN_LeftSquare = 0x1,
            AN_LeftDiamond = 0x2,

            AN_LeftMask = 0xF,

            AN_RightCircle = 0x00,
            AN_RightSquare = 0x10,
            AN_RightDiamond = 0x20,

            AN_RightMask = 0xF0,
        };
        Q_ENUM_NS(AnchorNodeFlag)
        Q_DECLARE_FLAGS(AnchorNode, AnchorNodeFlag)

        enum ItemInteraction {
            II_Pressed,
            II_Released,
            II_Canceled,
            II_HoverEntered,
            II_HoverExited,
            II_Clicked,
            II_DoubleClicked,
            II_ContextMenu,
            II_ItemContextMenu,
        };
        Q_ENUM_NS(ItemInteraction)

        enum TimelineInteractionOption {
            InteractionOnTimeline,
            InteractionOnPositionIndicator,
        };
        Q_ENUM_NS(TimelineInteractionOption)

        enum TrackInteractionOption {
            InteractionOnTrackItem,
            InteractionOnColorIndicator,
            InteractionOnMute,
            InteractionOnSolo,
            InteractionOnRecord,
            InteractionOnRoute,
            InteractionOnTrackName,
            InteractionOnGainSlider,
            InteractionOnGainSpinBox,
            InteractionOnPanDial,
            InteractionOnPanSpinBox,
            InteractionOnLevelMeter,
            InteractionOnPeakLabel,
            InteractionOnTrackHandle,
        };
        Q_ENUM_NS(TrackInteractionOption)

        enum GenericRangeInteractionOption {
            InteractionByMouseBehavior = 0x000,
            InteractionOnEdge = 0x100,

            GenericRangeInteractionOption_InteractionOptionMask = 0xff00,
        };
        Q_ENUM_NS(GenericRangeInteractionOption)

    }

}

Q_DECLARE_OPERATORS_FOR_FLAGS(sflow::ScopicFlow::AnchorNode)

#endif //SCOPIC_FLOW_SCOPICFLOWNAMESPACE_H
