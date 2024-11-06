import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Basic

Dial {
    id: control
    required property color foregroundColor
    required property color backgroundColor
    required property color primaryColor
    property double defaultValue: from

    background: Rectangle {
        property color normalColor: Qt.rgba(control.foregroundColor.r * 0.5 + control.backgroundColor.r * 0.5, control.foregroundColor.g * 0.5 + control.backgroundColor.g * 0.5, control.foregroundColor.b * 0.5 + control.backgroundColor.b * 0.5, control.foregroundColor.a * 0.5 + control.backgroundColor.a * 0.5)
        property color pressedColor: Qt.rgba(control.foregroundColor.r * 0.125 + control.backgroundColor.r * 0.875, control.foregroundColor.g * 0.125 + control.backgroundColor.g * 0.875, control.foregroundColor.b * 0.125 + control.backgroundColor.b * 0.875, control.foregroundColor.a * 0.125 + control.backgroundColor.a * 0.875)

        x: control.width / 2 - width / 2
        y: control.height / 2 - height / 2
        implicitWidth: 24
        implicitHeight: 24
        width: Math.min(control.width, control.height)
        height: Math.min(control.width, control.height)
        color: control.pressed ? pressedColor : normalColor
        radius: width / 2

        Shape {
            anchors.fill: parent
            ShapePath {
                strokeWidth: 2
                strokeColor: control.primaryColor
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
        color: control.foregroundColor
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

        Timer {
            id: timer
            interval: 250
        }

        onPressed: function (mouse) {
            if (timer.running) {
                timer.stop()
                control.value = control.defaultValue
                mouse.accepted = true
            } else {
                timer.start()
                mouse.accepted = false
            }

        }
    }
}