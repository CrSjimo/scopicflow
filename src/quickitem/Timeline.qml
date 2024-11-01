import ScopicFlowPrivate
import QtQml
import QtQuick
import QtQuick.Shapes

Timeline {
    id: timeline

    Rectangle {
        id: selectionRect
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: Qt.rgba(timeline.palette.foregroundColor.r, timeline.palette.foregroundColor.g, timeline.palette.foregroundColor.b, 0.5 * timeline.palette.foregroundColor.a)
        visible: false
        property int start: 0
    }

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

        property bool rejectContextMenu: false;

        property double deltaTickingX: 4

        Timer {
            id: tickingTimer
            interval: 10
            repeat: true

            onTriggered: {
                timeline.moveViewOnDraggingPositionIndicator(parent.deltaTickingX)
            }
        }

        onPressed: {
            rejectContextMenu = false;
        }

        onClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                timeline.primaryIndicatorX = mouse.x
            } else if (!rejectContextMenu) {
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
            if (pressedButtons & Qt.LeftButton) {
                if (mouse.x < 0) {
                    deltaTickingX = mouse.x / 8
                    tickingTimer.start()
                } else if (mouse.x >= timeline.width) {
                    deltaTickingX = (mouse.x - timeline.width) / 8
                    tickingTimer.start()
                } else {
                    timeline.primaryIndicatorX = mouse.x
                    tickingTimer.stop()
                }

            } else {
                let alignedX = Math.min(Math.max(0, timeline.getAlignedX(mouse.x)), timeline.width)
                if (!selectionRect.visible) {
                    cursorShape = Qt.OpenHandCursor
                    selectionRect.visible = true
                    selectionRect.start = alignedX
                    selectionRect.x = selectionRect.start
                    selectionRect.width = 0
                } else {
                    if (alignedX > selectionRect.start) {
                        selectionRect.x = selectionRect.start
                        selectionRect.width = alignedX - selectionRect.start
                    } else {
                        selectionRect.width = selectionRect.start - alignedX
                        selectionRect.x = alignedX
                    }
                }
            }
        }
        onReleased: {
            tickingTimer.stop()
            if (selectionRect.visible) {
                cursorShape = Qt.ArrowCursor
                if (selectionRect.width)
                    timeline.setZoomedRange(selectionRect.x, selectionRect.width)
                selectionRect.visible = false
                rejectContextMenu = true
            }
        }
    }

}