import QtQuick
import QtQuick.Shapes
import QtQuick.Controls.Basic

Item {
    id: handler

    property double deltaTickingX: 0
    property double deltaTickingY: 0
    property int direction: Qt.Horizontal | Qt.Vertical
    property var viewModel: null

    signal moved(x: double, y: double)

    Popup {
        id: indicator

        closePolicy: Popup.NoAutoClose
        height: 24
        padding: 0
        width: 24

        background: Item {
        }

        Shape {
            anchors.fill: parent

            ShapePath {
                fillColor: "white"
                strokeColor: "black"
                strokeWidth: 1

                PathAngleArc {
                    centerX: indicator.width * 0.5
                    centerY: indicator.height * 0.5
                    radiusX: indicator.width * 0.125
                    radiusY: indicator.height * 0.125
                    startAngle: 0
                    sweepAngle: 360
                }
            }
        }
        Shape {
            anchors.fill: parent
            visible: (handler.direction & Qt.Vertical) && (handler.deltaTickingY < 0 || (handler.deltaTickingX === 0 || !(handler.direction & Qt.Horizontal)) && handler.deltaTickingY === 0)

            ShapePath {
                fillColor: "white"
                strokeColor: "black"
                strokeWidth: 1

                PathMove {
                    x: indicator.width * 0.5
                    y: 0
                }
                PathLine {
                    x: indicator.width * 0.375
                    y: indicator.height * 0.25
                }
                PathLine {
                    x: indicator.width * 0.625
                    y: indicator.height * 0.25
                }
                PathLine {
                    x: indicator.width * 0.5
                    y: 0
                }
            }
        }
        Shape {
            anchors.fill: parent
            visible: (handler.direction & Qt.Horizontal) && (handler.deltaTickingX > 0 || handler.deltaTickingX === 0 && (handler.deltaTickingY === 0 || !(handler.direction & Qt.Vertical)))

            ShapePath {
                fillColor: "white"
                strokeColor: "black"
                strokeWidth: 1

                PathMove {
                    x: indicator.width
                    y: indicator.height * 0.5
                }
                PathLine {
                    x: indicator.width * 0.75
                    y: indicator.height * 0.375
                }
                PathLine {
                    x: indicator.width * 0.75
                    y: indicator.height * 0.625
                }
                PathLine {
                    x: indicator.width
                    y: indicator.height * 0.5
                }
            }
        }
        Shape {
            anchors.fill: parent
            visible: (handler.direction & Qt.Vertical) && (handler.deltaTickingY > 0 || (handler.deltaTickingX === 0 || !(handler.direction & Qt.Horizontal)) && handler.deltaTickingY === 0)

            ShapePath {
                fillColor: "white"
                strokeColor: "black"
                strokeWidth: 1

                PathMove {
                    x: indicator.width * 0.5
                    y: indicator.height
                }
                PathLine {
                    x: indicator.width * 0.375
                    y: indicator.height * 0.75
                }
                PathLine {
                    x: indicator.width * 0.625
                    y: indicator.height * 0.75
                }
                PathLine {
                    x: indicator.width / 2
                    y: indicator.height
                }
            }
        }
        Shape {
            anchors.fill: parent
            visible: (handler.direction & Qt.Horizontal) && (handler.deltaTickingX < 0 || handler.deltaTickingX === 0 && (handler.deltaTickingY === 0 || !(handler.direction & Qt.Vertical)))

            ShapePath {
                fillColor: "white"
                strokeColor: "black"
                strokeWidth: 1

                PathMove {
                    x: 0
                    y: indicator.height * 0.5
                }
                PathLine {
                    x: indicator.width * 0.25
                    y: indicator.height * 0.375
                }
                PathLine {
                    x: indicator.width * 0.25
                    y: indicator.height * 0.625
                }
                PathLine {
                    x: 0
                    y: indicator.height * 0.5
                }
            }
        }
        MouseArea {
            acceptedButtons: Qt.NoButton
            anchors.fill: parent
            cursorShape: mouseArea.cursorShape
        }
    }
    MouseArea {
        id: mouseArea

        property double originalX: 0
        property double originalY: 0

        function calculateScrollingSpeed(x) {
            if (Math.abs(x) < 8)
                return 0;
            return Math.sign(x) * Math.abs(x / 256);
        }

        acceptedButtons: Qt.MiddleButton
        anchors.fill: parent
        cursorShape: undefined
        focusPolicy: Qt.StrongFocus

        onCanceled: {
            tickingTimer.stop();
            cursorShape = undefined;
            handler.deltaTickingX = 0;
            handler.deltaTickingY = 0;
            indicator.close();
        }
        onPositionChanged: function (mouse) {
            if (handler.viewModel?.affectVelocity) {
                handler.deltaTickingX = calculateScrollingSpeed(mouse.x - originalX) * tickingTimer.interval;
                handler.deltaTickingY = calculateScrollingSpeed(mouse.y - originalY) * tickingTimer.interval;
                if (handler.deltaTickingX !== 0 || handler.deltaTickingY !== 0) {
                    cursorShape = Qt.OpenHandCursor;
                } else {
                    cursorShape = Qt.ArrowCursor;
                }
                tickingTimer.start();
            } else {
                handler.moved(originalX - mouse.x, originalY - mouse.y);
                originalX = mouse.x;
                originalY = mouse.y;
            }
        }
        onPressed: function (mouse) {
            if (handler.viewModel?.affectVelocity) {
                indicator.x = mouse.x - indicator.width / 2;
                indicator.y = mouse.y - indicator.height / 2;
                indicator.open();
                cursorShape = Qt.ArrowCursor;
            } else {
                cursorShape = Qt.ClosedHandCursor;
            }
            originalX = mouse.x;
            originalY = mouse.y;
        }
        onReleased: canceled()

        Timer {
            id: tickingTimer

            interval: 10
            repeat: true

            onTriggered: {
                handler.moved(handler.deltaTickingX, handler.deltaTickingY);
            }
        }
    }
}
