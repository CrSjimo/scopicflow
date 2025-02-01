import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    required property QtObject noteViewModel
    required property color noteColor
    required property bool current

    readonly property color _normalBackground: noteViewModel?.selected ? Qt.lighter(noteColor, 1.5) : noteColor
    readonly property color _normalForeground: Colors.autoForeground(_normalBackground)

    property color background: noteViewModel?.overlapped ? Qt.darker(_normalBackground, 1.2) : _normalBackground
    property color border: Qt.darker(noteColor, 1.1)
    property color foreground: noteViewModel?.overlapped ? Qt.rgba(_normalForeground.r, _normalForeground.g, _normalForeground.b, 0.5) : _normalForeground
}