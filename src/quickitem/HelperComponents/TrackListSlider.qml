import QtQml
import QtQuick
import QtQuick.Controls.Basic

Slider {
    id: slider
    property double defaultValue: from
    required property QtObject palette
    property double animationRatio: 1
    property string toolTip: ""
    readonly property bool intermediate: pressed || timer.running

    signal reset()

    background: Rectangle {
        x: slider.leftPadding
        y: slider.topPadding + slider.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 4
        width: slider.availableWidth
        height: implicitHeight
        radius: 2
        color: slider.palette.sliderBackgroundColor

        Rectangle {
            readonly property double defaultValuePosition: (slider.defaultValue - slider.from) / (slider.to - slider.from) * parent.width
            readonly property double deltaValueSize: slider.visualPosition * parent.width - defaultValuePosition
            x: deltaValueSize < 0 ? defaultValuePosition + deltaValueSize : defaultValuePosition
            width: Math.abs(deltaValueSize)
            height: parent.height
            color: slider.palette.primaryColor
            radius: 2
        }
    }

    handle: Item {
        Rectangle {
            id: sliderHandle
            x: slider.leftPadding + slider.visualPosition * (slider.availableWidth) - width / 2
            y: slider.topPadding + slider.availableHeight / 2 - height / 2
            property double handleSize: 12
            width: handleSize
            height: handleSize
            radius: handleSize / 2
            Behavior on handleSize {
                NumberAnimation {
                    duration: 250 * slider.animationRatio
                    easing.type: Easing.OutCubic
                }
            }

            color: slider.pressed ? slider.palette.sliderHandlePressedColor : slider.palette.sliderHandleColor
            Behavior on color {
                ColorAnimation {
                    duration: 250 * slider.animationRatio
                    easing.type: Easing.OutCubic
                }
            }

            MouseArea {
                anchors.fill: parent

                Timer {
                    id: timer
                    interval: 250
                }

                onPressed: function (mouse) {
                    if (timer.running) {
                        slider.reset()
                        timer.stop()
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
        if (hovered) {
            sliderHandle.handleSize = 16
        } else {
            sliderHandle.handleSize = 12
        }
    }

    ToolTip.text: toolTip
    Accessible.description: toolTip
    ToolTip.visible: toolTip.length && hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000
}