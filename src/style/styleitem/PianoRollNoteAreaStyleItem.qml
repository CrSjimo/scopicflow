import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    required property QtObject noteViewModel
    required property bool current

    property color background: noteViewModel?.selected ? "#66ccff" : "#3498db"
    property color border: noteViewModel?.selected ? "#4d7e9e" : "#4d7e9e"
    property color foreground: Colors.autoForeground(background)
}