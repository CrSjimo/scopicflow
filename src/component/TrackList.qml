import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject trackListViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null

    property Component trackExtraDelegate: null

    padding: 0
    focusPolicy: Qt.StrongFocus

    signal trackDoubleClicked(index: int)
    signal contextMenuRequestedForTrack(index: int)
    signal contextMenuRequestedForTrackDragging(index: int, target: int)

    ScopicFlowInternal.TrackList {
        anchors.fill: parent
        trackListViewModel: control.trackListViewModel
        trackListLayoutViewModel: control.trackListLayoutViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel
        trackExtraDelegate: control.trackExtraDelegate

        onTrackDoubleClicked: index => control.trackDoubleClicked(index)
        onContextMenuRequestedForTrack: index => control.contextMenuRequestedForTrack(index)
        onContextMenuRequestedForTrackDragging: (index, target) => control.contextMenuRequestedForTrackDragging(index, target)

    }
}