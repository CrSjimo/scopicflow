import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    required property bool active

    property color background: "#242a30"
    property color foreground: Colors.autoForeground(background)
    property color border: active ? "#3498db" : "#222222"
}