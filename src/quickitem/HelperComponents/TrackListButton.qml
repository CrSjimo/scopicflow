import QtQuick
import QtQuick.Controls.Basic

Button {
    id: button
    property color checkedColor
    required property QtObject palette
    readonly property color borderColor: trackListDelegate.palette.borderColor
    readonly property color foregroundColor: trackListDelegate.palette.foregroundColor
    readonly property color normalColor: trackListDelegate.palette.buttonColor
    readonly property color hoveredColor: trackListDelegate.palette.buttonHoveredColor
    readonly property color pressedColor: trackListDelegate.palette.buttonPressedColor
    required property double animationRatio
    property string toolTip: ""
    checkable: true
    width: 24
    height: 24
    background: Rectangle {
        border.width: 1
        border.color: borderColor
        radius: 2
        color: button.checked ? button.checkedColor : button.down ? pressedColor : button.hovered ? hoveredColor : normalColor
        Behavior on color {
            ColorAnimation {
                duration: button.animationRatio * 250
                easing.type: Easing.OutCubic
            }
        }
    }

    contentItem: Text {
        text: button.text
        font: button.font
        color: foregroundColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    ToolTip.text: toolTip
    Accessible.description: toolTip
    ToolTip.visible: toolTip.length && hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000

}