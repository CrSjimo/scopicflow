import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject animationViewModel: null
    property Component clipGraph: null
    property QtObject clipPaneBehaviorViewModel: null
    property QtObject clipSequenceViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject trackListViewModel: null

    signal clipContextMenuRequired(model: QtObject)
    signal clipCut(model: QtObject, position: int)
    signal clipDoubleClicked(model: QtObject)
    signal contextMenuRequired(position: int, trackNumber: int)
    signal doubleClicked(position: int, trackNumber: int)

    focusPolicy: Qt.StrongFocus
    padding: 0

    ScopicFlowInternal.ClipPane {
        id: clipPane

        anchors.fill: parent
        animationViewModel: control.animationViewModel
        clipGraph: control.clipGraph
        clipPaneBehaviorViewModel: control.clipPaneBehaviorViewModel
        clipSequenceViewModel: control.clipSequenceViewModel
        playbackViewModel: control.playbackViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        timeViewModel: control.timeViewModel
        trackListLayoutViewModel: control.trackListLayoutViewModel
        trackListViewModel: control.trackListViewModel

        onClipContextMenuRequired: model => control.clipContextMenuRequired(model)
        onClipCut: (model, position) => control.clipCut(model, position)
        onClipDoubleClicked: model => control.clipDoubleClicked(model)
        onContextMenuRequired: (position, trackNumber) => control.contextMenuRequired(position, trackNumber)
        onDoubleClicked: (position, trackNumber) => control.doubleClicked(position, trackNumber)
    }
}
