import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Item {
    id: area

    property QtObject animationViewModel: null
    property QtObject clavierViewModel: null
    property QtObject noteSequenceViewModel: null
    property QtObject pianoRollNoteAreaBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    required property rect viewport

    signal contextMenuRequired(position: int, key: int)
    signal doubleClicked(position: int, key: int)
    signal noteContextMenuRequired(model: QtObject)
    signal noteCut(model: QtObject, position: int)

    ScopicFlowInternal.PianoRollNoteArea {
        id: noteArea

        anchors.fill: parent
        animationViewModel: area.animationViewModel
        clavierViewModel: area.clavierViewModel
        noteSequenceViewModel: area.noteSequenceViewModel
        pianoRollNoteAreaBehaviorViewModel: area.pianoRollNoteAreaBehaviorViewModel
        timeLayoutViewModel: area.timeLayoutViewModel
        timeViewModel: area.timeViewModel
        viewport: area.viewport

        onContextMenuRequired: (position, key) => area.contextMenuRequired(position, key)
        onDoubleClicked: (position, key) => area.doubleClicked(position, key)
        onNoteContextMenuRequired: model => area.noteContextMenuRequired(model)
        onNoteCut: (model, position) => area.noteCut(model, position)
    }
}
