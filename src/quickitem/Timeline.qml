import ScopicFlowPrivate
import QtQuick
import QtQuick.Shapes

Timeline {
    id: timeLine

    Shape {
        id: secondaryIndicator
        width: 16
        height: 16
        anchors.bottom: parent.bottom
        ShapePath {
            strokeWidth: 0
            fillColor: Qt.rgba(timeLine.palette.positionIndicatorColor.r, timeLine.palette.positionIndicatorColor.g, timeLine.palette.positionIndicatorColor.b, 0.5 * timeLine.palette.positionIndicatorColor.a)
            startX: 0; startY: 0
            PathLine { x: 0; y: 0 }
            PathLine { x: 0; y: 8 }
            PathLine { x: 8; y: 16 }
            PathLine { x: 16; y: 8 }
            PathLine { x: 16; y: 0 }
        }
        x: timeLine.secondaryIndicatorX - 8
    }

    Shape {
        id: primaryIndicator
        width: 16
        height: 16
        anchors.bottom: parent.bottom
        ShapePath {
            strokeWidth: 0
            fillColor: timeLine.palette.positionIndicatorColor
            startX: 0; startY: 0
            PathLine { x: 0; y: 0 }
            PathLine { x: 0; y: 8 }
            PathLine { x: 8; y: 16 }
            PathLine { x: 16; y: 8 }
            PathLine { x: 16; y: 0 }
        }
        x: timeLine.primaryIndicatorX - 8
    }

    Rectangle {
        id: cursorIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: timeLine.palette.cursorIndicatorColor
        x: timeLine.cursorIndicatorX
        visible: timeLine.cursorIndicatorX >= 0
    }

    MouseArea {
        anchors.fill: parent
        drag.axis: Drag.XAxis
        drag.minimumX: timeLine.zeroTickX - 8
        onClicked: function (mouse) {
            timeLine.primaryIndicatorX = mouse.x
        }
        onPositionChanged: function (mouse) {
            if (pressed)
                timeLine.primaryIndicatorX = mouse.x
        }
    }

}