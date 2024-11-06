import QtQml
import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: slider
    property double defaultValue: from
    required property color foregroundColor
    required property color backgroundColor
    required property color primaryColor

    background: Rectangle {
        x: slider.leftPadding
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: slider.availableWidth
        height: implicitHeight
        radius: 2
        color: Qt.rgba(slider.foregroundColor.r, slider.foregroundColor.g, slider.foregroundColor.b, 0.25 * slider.foregroundColor.a)

        Rectangle {
            readonly property double defaultValuePosition: (slider.defaultValue - slider.from) / (slider.to - slider.from) * parent.width
            readonly property double deltaValueSize: slider.visualPosition * parent.width - defaultValuePosition
            x: deltaValueSize < 0 ? defaultValuePosition + deltaValueSize : defaultValuePosition
            width: Math.abs(deltaValueSize)
            height: parent.height
            color: slider.primaryColor
            radius: 2
        }
    }

    handle: Rectangle {
        x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 14
        implicitHeight: 14
        radius: 7
        color: slider.backgroundColor

        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: slider.foregroundColor
            opacity: slider.pressed ? 0.5 : 1.0
        }

        MouseArea {
            anchors.fill: parent

            Timer {
                id: timer
                interval: 250
            }

            onPressed: function (mouse) {
                if (timer.running) {
                    timer.stop()
                    slider.value = slider.defaultValue
                    mouse.accepted = true
                } else {
                    timer.start()
                    mouse.accepted = false
                }

            }
        }
    }
}