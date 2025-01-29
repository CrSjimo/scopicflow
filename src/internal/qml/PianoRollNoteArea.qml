import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

Item {
    id: noteArea
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject clavierViewModel: null
    property QtObject animationViewModel: null
    property QtObject noteSequenceViewModel: null
    property double sliceWidth: 0

    required property QtObject stylesheet

    SequenceSlicer {
        model: noteArea.noteSequenceViewModel
        timeViewModel: noteArea.timeViewModel
        timeLayoutViewModel: noteArea.timeLayoutViewModel
        sliceWidth: noteArea.sliceWidth
        delegate: Rectangle {
            id: note
            required property QtObject model
            property bool current: false
            property QtObject noteStyleItem: {noteStyleItem = noteArea.stylesheet.pianoRollNoteArea.createObject(note, {noteViewModel: model, current})}
            Binding {
                when: note.visible
                note.x: note.model.position * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                note.y: (127 - note.model.key) * (noteArea.clavierViewModel?.pixelDensity ?? 0)
                note.width: note.model.length * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                note.height: (noteArea.clavierViewModel?.pixelDensity ?? 0)
                note.current: model === noteArea.noteSequenceViewModel.handle.currentItem
                note.noteStyleItem: noteArea.stylesheet.pianoRollNoteArea.createObject(note, {noteViewModel: note.model, current: note.current})
            }
            color: noteStyleItem.background
            radius: 2


            Rectangle {
                id: border
                anchors.fill: parent
                color: "transparent"
                border.width: note.model.selected ? 2 : 1
                opacity: note.model.selected ? 1 : 0.5
                border.color: note.noteStyleItem.border
            }

            Text {
                id: lyricText
                anchors.left: parent.left
                anchors.leftMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                text: note.model.lyric
                visible: note.width >= width + 8
                color: note.noteStyleItem.foreground
            }

        }
    }
}