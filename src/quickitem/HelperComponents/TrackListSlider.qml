import QtQml
import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: slider
    property double defaultValue: from
    required property color foregroundColor
    required property color backgroundColor
    required property color primaryColor
    property double animationRatio: 1

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

    handle: Item {
        Rectangle {
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth) - width / 2
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            property color pressedColor: Qt.rgba(slider.foregroundColor.r * 0.5 + slider.backgroundColor.r * 0.5, slider.foregroundColor.g * 0.5 + slider.backgroundColor.g * 0.5, slider.foregroundColor.b * 0.5 + slider.backgroundColor.b * 0.5, slider.foregroundColor.a * 0.5 + slider.backgroundColor.a * 0.5)
            property double handleSize: 12
            width: handleSize
            height: handleSize
            radius: handleSize / 2
            NumberAnimation on handleSize {
                id: hoverEnterAnimation
                from: 12
                to: 16
                easing.type: Easing.OutCubic
                duration: 250 * slider.animationRatio
            }
            NumberAnimation on handleSize {
                id: hoverExitAnimation
                from: 16
                to: 12
                easing.type: Easing.OutCubic
                duration: 250 * slider.animationRatio
            }

            color: slider.pressed ? pressedColor : slider.foregroundColor

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

    onHoveredChanged: {
        hoverEnterAnimation.stop()
        hoverExitAnimation.stop()
        if (hovered) {
            hoverEnterAnimation.start()
        } else {
            hoverExitAnimation.start()
        }
    }
}