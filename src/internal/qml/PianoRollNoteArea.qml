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

    SequenceSlicer {
        model: noteArea.noteSequenceViewModel
        timeViewModel: noteArea.timeViewModel
        timeLayoutViewModel: noteArea.timeLayoutViewModel
        sliceWidth: noteArea.sliceWidth
        delegate: Rectangle {
            id: note
            required property QtObject model
            Binding {
                when: note.visible
                note.x: note.model.position * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                note.y: (127 - note.model.key) * (noteArea.clavierViewModel?.pixelDensity ?? 0)
                note.width: note.model.length * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                note.height: (noteArea.clavierViewModel?.pixelDensity ?? 0)
            }
            color: "#7FFF0000" // TODO
            radius: 2

            Text {
                id: lyricText
                anchors.left: parent.left
                anchors.leftMargin: 4
                anchors.verticalCenter: parent.verticalCenter
                text: note.model.lyric
                visible: note.width >= width + 8
                color: "black" // TODO
            }

        }
    }
}