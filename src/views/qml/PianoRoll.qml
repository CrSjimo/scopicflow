import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject animationViewModel: null
    property double bottomMargin: 0
    property QtObject clavierViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    property double topMargin: 0
    property list<Component> viewItems: []

    focusPolicy: Qt.StrongFocus
    padding: 0

    ScopicFlowInternal.PianoRoll {
        id: pianoRoll

        anchors.fill: parent
        animationViewModel: control.animationViewModel
        bottomMargin: control.bottomMargin
        clavierViewModel: control.clavierViewModel
        playbackViewModel: control.playbackViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        timeViewModel: control.timeViewModel
        topMargin: control.topMargin
        viewItems: control.viewItems
    }
}
