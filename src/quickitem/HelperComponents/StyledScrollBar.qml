import QtQuick
import QtQuick.Controls.Basic

ScrollBar {
    id: scrollBar
    required property color normalColor
    required property color pressedColor
    required property color hoveredColor
    required property double animationRatio
    contentItem: Rectangle {
        implicitWidth: 8
        implicitHeight: 8
        radius: 4
        color: scrollBar.pressed ? scrollBar.pressedColor : scrollBar.hovered ? scrollBar.hoveredColor : scrollBar.normalColor
        Behavior on color {
            ColorAnimation {
                duration: 250 * scrollBar.animationRatio
                easing.type: Easing.OutCubic
            }
        }
    }
}
