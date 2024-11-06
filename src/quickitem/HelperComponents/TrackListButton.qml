import QtQuick
import QtQuick.Controls.Basic

Button {
    id: button
    required property color checkedColor
    required property color foregroundColor
    required property color borderColor
    property string toolTip: ""
    checkable: true
    width: 24
    height: 24
    background: Rectangle {
        property color hoveredColor: Qt.rgba(foregroundColor.r, foregroundColor.g, foregroundColor.b, 0.5 * foregroundColor.a)
        property color pressedColor: Qt.rgba(foregroundColor.r, foregroundColor.g, foregroundColor.b, 0.125 * foregroundColor.a)
        property color normalColor: Qt.rgba(foregroundColor.r, foregroundColor.g, foregroundColor.b, 0.25 * foregroundColor.a)
        border.width: 1
        border.color: borderColor
        radius: 2
        color: button.checked ? button.checkedColor : button.down ? pressedColor : button.hovered ? hoveredColor : normalColor
    }

    contentItem: Text {
        text: button.text
        font: button.font
        color: foregroundColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }

    ToolTip.text: toolTip
    ToolTip.visible: hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000

}