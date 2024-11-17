import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Basic

Dial {
    id: control
    required property QtObject palette
    property double defaultValue: from
    property double animationRatio: 1
    property string toolTip: ""
    readonly property bool intermediate: pressed || timer.running

    signal reset()

    background: Rectangle {
        x: control.width / 2 - width / 2
        y: control.height / 2 - height / 2
        implicitWidth: 24
        implicitHeight: 24
        width: Math.min(control.width, control.height)
        height: Math.min(control.width, control.height)
        color: control.pressed ? control.palette.dialPressedColor : control.hovered ? control.palette.dialHoveredColor : control.palette.dialColor
        Behavior on color {
            ColorAnimation {
                duration: 250 * control.animationRatio
                easing.type: Easing.OutCubic
            }
        }
        radius: width / 2

        Shape {
            anchors.fill: parent
            ShapePath {
                strokeWidth: 2
                strokeColor: control.palette.primaryColor
                fillColor: "transparent"
                capStyle: ShapePath.RoundCap
                PathAngleArc {
                    centerX: control.width / 2
                    centerY: control.height / 2
                    radiusX: control.width / 2 - 1
                    radiusY: control.height / 2 - 1
                    startAngle: control.startAngle + (control.defaultValue - control.from) / (control.to - control.from) * (control.endAngle - control.startAngle) - 90
                    sweepAngle: -(startAngle + 90 - control.angle)
                }
            }
        }
    }

    handle: Rectangle {
        id: handleItem
        x: control.background.x + control.background.width / 2 - width / 2
        y: control.background.y + control.background.height / 2 - height / 2
        width: 1
        height: 4
        color: control.palette.foregroundColor
        antialiasing: true
        opacity: control.enabled ? 1 : 0.3
        transform: [
            Translate {
                y: -control.background.width * 0.4 + handleItem.height
            },
            Rotation {
                angle: control.angle
                origin.x: handleItem.width / 2
                origin.y: handleItem.height / 2
            }
        ]
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.SizeHorCursor

        Timer {
            id: timer
            interval: 250
        }

        onPressed: function (mouse) {
            if (timer.running) {
                control.reset()
                timer.stop()
                mouse.accepted = true
            } else {
                timer.start()
                mouse.accepted = false
            }

        }
    }

    ToolTip.text: toolTip
    Accessible.description: toolTip
    ToolTip.visible: toolTip.length && hovered
    ToolTip.delay: 1000
    ToolTip.timeout: 5000

    inputMode: Dial.Horizontal
}