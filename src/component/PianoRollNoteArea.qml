import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Item {
    id: area
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject clavierViewModel: null
    property QtObject animationViewModel: null
    property QtObject noteSequenceViewModel: null
    property QtObject pianoRollNoteAreaBehaviorViewModel: null

    required property rect viewport

    ScopicFlowInternal.PianoRollNoteArea {
        id: noteArea
        anchors.fill: parent

        timeViewModel: area.timeViewModel
        timeLayoutViewModel: area.timeLayoutViewModel
        clavierViewModel: area.clavierViewModel
        animationViewModel: area.animationViewModel
        noteSequenceViewModel: area.noteSequenceViewModel
        pianoRollNoteAreaBehaviorViewModel: area.pianoRollNoteAreaBehaviorViewModel

        viewport: area.viewport
    }
}