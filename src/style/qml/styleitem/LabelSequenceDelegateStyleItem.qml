import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    required property QtObject labelViewModel
    required property bool current

    property color background: labelViewModel?.selected ? "#40657a" : "#3d444d"
    property color foreground: Colors.autoForeground(background)
    property color border: "#535c68"
}