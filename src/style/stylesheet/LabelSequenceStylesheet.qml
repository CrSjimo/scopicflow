import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    property Component labelSequence: LabelSequenceStyleItem {}
    property Component labelSequenceDelegate: LabelSequenceDelegateStyleItem {}
    property Component rubberBand: RubberBandStyleItem {}
    property Component popupEdit: PopupEditStyleItem {}
    property Component timeIndicators: TimeIndicatorsStyleItem {}
}