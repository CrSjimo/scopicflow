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

    SequenceSlicer {
        model: noteArea.noteSequenceViewModel
        timeViewModel: noteArea.timeViewModel
        timeLayoutViewModel: noteArea.timeLayoutViewModel
        sliceWidth: noteArea.width
        delegate: Rectangle {
            id: note
            required property QtObject model
            x: model.position * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
            y: (127 - model.key) * (noteArea.clavierViewModel?.pixelDensity ?? 0)
            width: model.length * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
            height: (noteArea.clavierViewModel?.pixelDensity ?? 0)
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