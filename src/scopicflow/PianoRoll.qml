import QtQml
import QtQuick
import QtQuick.Controls.Basic

import "./Private" as ScopicFlowPrivate

Pane {
    id: control
    property QtObject timeAlignmentViewModel: null
    property QtObject playbackViewModel: null
    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null

    padding: 0
    focusPolicy: Qt.StrongFocus

    ScopicFlowPrivate.PianoRoll {
        id: pianoRoll
        anchors.fill: parent
        timeAlignmentViewModel: control.timeAlignmentViewModel
        playbackViewModel: control.playbackViewModel
        clavierViewModel: control.clavierViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel
    }
}