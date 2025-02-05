import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    required property QtObject clipViewModel
    required property color clipColor
    required property bool current

    readonly property color _normalBackground: clipViewModel?.selected ? Qt.lighter(clipColor, 1.5) : Qt.lighter(clipColor, 1.25)
    readonly property color _normalForeground: Colors.autoForeground(_normalBackground)
    readonly property color _solidBackground: _normalBackground

    property color header: clipColor
    property color background: Qt.rgba(_solidBackground.r, _solidBackground.g, _solidBackground.a, 0.75 * _solidBackground.a)
    property color border: Qt.darker(clipColor, 1.1)
    property color foreground: _normalForeground
    property color text: Colors.autoForeground(header)
}