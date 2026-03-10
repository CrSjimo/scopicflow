import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: note

    property bool current: false
    property bool editScopeFocused: false
    property bool selected: false
    property bool overlapped: false
    property bool lyricEditing: false
    property color color: Theme.accentColor
    property string lyric: ""
    property bool outOfClipRange: false
    property bool thumbnailDisplay: false
    required property SelectionController selectionController

    property QtObject noteViewModel: SequenceSlicerLoader.viewModel

    opacity: lyricEditing ? 0 : 1

    Binding {
        note.z: note.current && note.noteViewModel?.selected ? 3 : note.noteViewModel?.current ? 2 : note.noteViewModel?.selected ? 1 : 0
        note.selected: note.noteViewModel?.selected ?? false
        note.overlapped: note.noteViewModel?.overlapped ?? false
        note.lyric: note.noteViewModel?.lyric ?? ""
        when: note.SequenceSlicerLoader.inRange
    }

    Binding {
        note.editScopeFocused: note.selectionController?.editScopeFocused ?? false
        when: note.SequenceSlicerLoader.inRange && note.current
    }

    Rectangle {
        id: bodyRect

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 0.5
        height: note.thumbnailDisplay ? 4 : note.height - 1
        readonly property color baseColor: note.color
        color: {
            let a = 1
            if (note.outOfClipRange) {
                a /= 3
            }
            if (note.thumbnailDisplay) {
                a /= 2
            }
            let color = note.selected ? SFPalette.noteSelectedColorChange.apply(baseColor) : baseColor
            return Qt.rgba(color.r, color.g, color.b, color.a * a)
        }
        border.width: note.thumbnailDisplay ? 0 : 2
        border.color: note.selected ? Theme.foregroundPrimaryColor : note.overlapped ? Theme.warningColor : baseColor
        radius: 4

        Text {
            anchors.left: parent.left
            anchors.leftMargin: 6
            anchors.right: parent.right
            anchors.rightMargin: 6
            anchors.verticalCenter: parent.verticalCenter
            clip: true
            color: Theme.foregroundPrimaryColor
            visible: width >= contentWidth && !note.thumbnailDisplay
            font: Theme.font
            text: note.lyric
            opacity: note.outOfClipRange ? 0.75 : 1
        }
    }

}
