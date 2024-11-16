import QtQml
import QtQuick

Item {
    id: noteItem
    required property string lyric
    required property bool selected
    required property bool silent
    required property bool invalid
    required property bool lyricError
    required property QtObject palette

    clip: true

    Rectangle {
        id: noteRect
        anchors.fill: parent
        radius: 2
        color: selected ? noteItem.palette.noteSelectedColor : invalid ? noteItem.palette.noteInvalidColor : silent ? noteItem.palette.noteSilentColor : noteItem.palette.noteColor
        border.width: 2
        border.color: invalid ? noteItem.palette.noteInvalidBorderColor : silent ? noteItem.palette.noteSilentBorderColor : selected ? noteItem.palette.noteSelectedBorderColor : noteItem.palette.noteBorderColor
    }

    Text {
        id: lyricText
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        text: noteItem.lyric
        visible: parent.width >= width + 8
        color: lyricError ? noteItem.palette.lyricErrorTextColor: invalid ? noteItem.palette.noteInvalidTextColor : silent ? noteItem.palette.noteSilentTextColor : selected ? noteItem.palette.noteSelectedTextColor : noteItem.palette.noteTextColor
    }

}