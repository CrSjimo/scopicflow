import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject trackListViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject clipSequenceViewModel: null
    property QtObject clipPaneBehaviorViewModel: null

    property Component clipGraph: null

    padding: 0
    focusPolicy: Qt.StrongFocus

    signal clipCut(model: QtObject, position: int)
    signal doubleClicked(position: int, trackNumber: int)
    signal contextMenuRequired(position: int, trackNumber: int)
    signal clipContextMenuRequired(model: QtObject)
    signal clipDoubleClicked(model: QtObject)

    ScopicFlowInternal.ClipPane {
        id: clipPane
        anchors.fill: parent

        timeViewModel: control.timeViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        playbackViewModel: control.playbackViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        trackListViewModel: control.trackListViewModel
        trackListLayoutViewModel: control.trackListLayoutViewModel
        clipSequenceViewModel: control.clipSequenceViewModel
        clipPaneBehaviorViewModel: control.clipPaneBehaviorViewModel

        clipGraph: control.clipGraph

        onClipCut: (model, position) => control.clipCut(model, position)
        onDoubleClicked: (position, trackNumber) => control.doubleClicked(position, trackNumber)
        onContextMenuRequired: (position, trackNumber) => control.contextMenuRequired(position, trackNumber)
        onClipContextMenuRequired: (model) => control.clipContextMenuRequired(model)
        onClipDoubleClicked: (model) => control.clipDoubleClicked(model)

    }

}