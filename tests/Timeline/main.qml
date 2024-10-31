import ScopicFlow 1.0;
import QtQuick
import QtQuick.Shapes

Timeline {
    id: timeLine

    MouseArea {
        anchors.fill: parent
        onClicked: {
            primaryIndicatorX = mouse.x
        }
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
        onXChanged: {
            timeLine.primaryIndicatorX = x + 8
        }

        MouseArea {
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: timeLine.zeroTickX - 8
        }
    }

}

