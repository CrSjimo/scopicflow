import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    property Component clipPane: ClipPaneStyleItem {}
    property Component timeIndicators: TimeIndicatorsStyleItem {}
    property Component rubberBand: RubberBandStyleItem {}
    property Component scrollBar: ScrollBarStyleItem {}
    property Component popupEdit: PopupEditStyleItem {}
    property Component clip: ClipStyleItem {}
}