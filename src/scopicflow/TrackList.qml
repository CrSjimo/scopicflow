import QtQml
import QtQuick
import QtQuick.Controls.Basic

import "./Private" as ScopicFlowPrivate

Pane {
    id: control
    property QtObject trackListViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null

    padding: 0
    focusPolicy: Qt.StrongFocus

    signal trackDoubleClicked(index: int)
    signal contextMenuRequestedForTrack(index: int)
    signal contextMenuRequestedForTrackDragging(index: int, target: int)

    ScopicFlowPrivate.TrackList {
        anchors.fill: parent
        trackListViewModel: control.trackListViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel

        onTrackDoubleClicked: index => control.trackDoubleClicked(index)
        onContextMenuRequestedForTrack: index => control.contextMenuRequestedForTrack(index)
        onContextMenuRequestedForTrackDragging: (index, target) => control.contextMenuRequestedForTrackDragging(index, target)

    }
}