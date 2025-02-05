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

    padding: 0
    focusPolicy: Qt.StrongFocus

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
    }

}