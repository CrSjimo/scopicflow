import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    property Component trackList: TrackListStyleItem {}
    property Component trackListDelegate: TrackListDelegateStyleItem {}
    property Component rubberBand: RubberBandStyleItem {}
    property Component popupEdit: PopupEditStyleItem {}
}