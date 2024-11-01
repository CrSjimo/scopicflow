import ScopicFlowPrivate
import QtQuick
import QtQuick.Shapes

Timeline {
    id: timeline

    Shape {
        id: secondaryIndicator
        width: 16
        height: 16
        anchors.bottom: parent.bottom
        ShapePath {
            strokeWidth: 0
            fillColor: Qt.rgba(timeline.palette.positionIndicatorColor.r, timeline.palette.positionIndicatorColor.g, timeline.palette.positionIndicatorColor.b, 0.5 * timeline.palette.positionIndicatorColor.a)
            startX: 0; startY: 0
            PathLine { x: 0; y: 0 }
            PathLine { x: 0; y: 8 }
            PathLine { x: 8; y: 16 }
            PathLine { x: 16; y: 8 }
            PathLine { x: 16; y: 0 }
        }
        x: timeline.secondaryIndicatorX - 8
    }

    Shape {
        id: primaryIndicator
        width: 16
        height: 16
        anchors.bottom: parent.bottom
        ShapePath {
            strokeWidth: 0
            fillColor: timeline.palette.positionIndicatorColor
            startX: 0; startY: 0
            PathLine { x: 0; y: 0 }
            PathLine { x: 0; y: 8 }
            PathLine { x: 8; y: 16 }
            PathLine { x: 16; y: 8 }
            PathLine { x: 16; y: 0 }
        }
        x: timeline.primaryIndicatorX - 8
    }

    Rectangle {
        id: cursorIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: timeline.palette.cursorIndicatorColor
        x: timeline.cursorIndicatorX
        visible: timeline.cursorIndicatorX >= 0
    }

    MouseArea {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        drag.axis: Drag.XAxis
        drag.minimumX: timeline.zeroTickX - 8
        onClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                timeline.primaryIndicatorX = mouse.x
            } else {
                if (primaryIndicator.contains(mapToItem(primaryIndicator, mouse.x, mouse.y))) {
                    timeline.contextMenuRequestedForPositionIndicator();
                } else {
                    timeline.handleContextMenuRequest(mouse.x)
                }
            }

        }
        onDoubleClicked : function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                timeline.primaryIndicatorX = mouse.x
                timeline.positionIndicatorDoubleClicked()
            }
        }
        onPositionChanged: function (mouse) {
            if (pressed && (pressedButtons & Qt.LeftButton))
                timeline.primaryIndicatorX = mouse.x
        }
    }

}