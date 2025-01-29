import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    required property bool isBlackKey

    property color background: isBlackKey ? "#212529" : "#f8f9fa"
    property color foreground: Colors.autoForeground(background)
    property color border: "#636e72"
    property color backgroundHovered: isBlackKey ? "#495057" : "#868e96"
    property color backgroundPressed: isBlackKey ? "#343a40" : "#dee2e6"

}