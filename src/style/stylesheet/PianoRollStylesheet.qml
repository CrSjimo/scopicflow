import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    property Component pianoRoll: PianoRollStyleItem {}
    property Component scrollBar: ScrollBarStyleItem {}
    property Component rubberBand: RubberBandStyleItem {}
    property Component pianoRollNoteArea: PianoRollNoteAreaStyleItem {}
    property Component popupEdit: PopupEditStyleItem {}
    property Component timeIndicators: TimeIndicatorsStyleItem {}
}