import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject timeAlignmentViewModel: null
    property QtObject playbackViewModel: null
    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null
    property double topMargin: 0
    property double bottomMargin: 0

    padding: 0
    focusPolicy: Qt.StrongFocus

    ScopicFlowInternal.PianoRoll {
        id: pianoRoll
        anchors.fill: parent
        timeAlignmentViewModel: control.timeAlignmentViewModel
        playbackViewModel: control.playbackViewModel
        clavierViewModel: control.clavierViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel
        topMargin: control.topMargin
        bottomMargin: control.bottomMargin
    }
}