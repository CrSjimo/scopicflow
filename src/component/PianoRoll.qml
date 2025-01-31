import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject noteSequenceViewModel: null
    property QtObject pianoRollNoteAreaBehaviorViewModel: null
    property double topMargin: 0
    property double bottomMargin: 0

    padding: 0
    focusPolicy: Qt.StrongFocus

    ScopicFlowInternal.PianoRoll {
        id: pianoRoll
        anchors.fill: parent
        timeViewModel: control.timeViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        playbackViewModel: control.playbackViewModel
        clavierViewModel: control.clavierViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        noteSequenceViewModel: control.noteSequenceViewModel
        pianoRollNoteAreaBehaviorViewModel: control.pianoRollNoteAreaBehaviorViewModel
        topMargin: control.topMargin
        bottomMargin: control.bottomMargin
    }
}