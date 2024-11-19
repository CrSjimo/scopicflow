import ScopicFlowPrivate

import QtQml
import QtQuick
import QtQuick.Shapes

import './HelperComponents'
import "qrc:/ScopicFlow/modules/dev/sjimo/ScopicFlow/Palettes" as ScopicFlowPalette

Timeline {
    id: timeline

    readonly property QtObject defaultPalette: ScopicFlowPalette.Timeline {

    }

    readonly property QtObject palette: paletteViewModel?.palette?.timeline ?? defaultPalette

    backgroundColor: palette.backgroundColor
    foregroundColor: palette.foregroundColor

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: timeline.timeAlignmentViewModel
        animationViewModel: timeline.animationViewModel
    }

    Rectangle {
        id: selectionRect
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: Qt.rgba(timeline.foregroundColor.r, timeline.foregroundColor.g, timeline.foregroundColor.b, 0.5 * timeline.foregroundColor.a)
        visible: false
        property int start: 0
    }

    Shape {
        id: secondaryIndicator
        width: 32 / Math.sqrt(3)
        height: 14.3333
        anchors.bottom: parent.bottom
        ShapePath {
            id: indicatorPath
            strokeWidth: 0
            fillColor: timeline.palette.secondaryIndicatorColor
            PathLine { x: 8 / Math.sqrt(3); y: 0 }
            PathLine { x: 24 / Math.sqrt(3); y: 0 }
            PathArc { x: 28 / Math.sqrt(3); y: 4; radiusX: 4 / 3; radiusY: 4 / 3}
            PathLine { x: 20 / Math.sqrt(3); y: 12 }
            PathArc { x: 12 / Math.sqrt(3); y: 12; radiusX: 4 / 3; radiusY: 4 / 3}
            PathLine { x: 4 / Math.sqrt(3); y: 4}
            PathArc {x: 8 / Math.sqrt(3); y: 0; radiusX: 4 / 3; radiusY: 4 / 3}
        }
        x: timeline.secondaryIndicatorX - 16 / Math.sqrt(3)
    }

    Shape {
        id: primaryIndicator
        width: 32 / Math.sqrt(3)
        height: 14.3333
        anchors.bottom: parent.bottom
        ShapePath {
            strokeWidth: 0
            fillColor: timeline.palette.primaryIndicatorColor
            pathElements: indicatorPath.pathElements
        }
        x: timeline.primaryIndicatorX - 16 / Math.sqrt(3)
    }

    Rectangle {
        id: cursorIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: timeline.palette.cursorIndicatorColor
        x: timeline.cursorIndicatorX - 0.5
        visible: timeline.cursorIndicatorX >= 0
    }

    MouseArea {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        drag.axis: Drag.XAxis
        drag.minimumX: timeline.zeroTickX - 8

        DragScroller {
            id: dragScroller
            onMoved: function (deltaX) {
                timeline.moveViewOnDraggingPositionIndicator(deltaX)
            }
        }

        property bool rejectContextMenu: false;

        onPressed: function (mouse) {
            rejectContextMenu = false;
        }
        onClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                timeline.primaryIndicatorX = mouse.x
            } else if (mouse.button === Qt.RightButton && !rejectContextMenu) {
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
                    dragScroller.distanceX = mouse.x
                    dragScroller.running = true
                } else if (mouse.x >= timeline.width) {
                    dragScroller.distanceX = mouse.x - timeline.width
                    dragScroller.running = true
                } else {
                    timeline.primaryIndicatorX = mouse.x
                    dragScroller.running = false
                }

            } else if (pressedButtons & Qt.RightButton) {
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
        onReleased: function() {
            dragScroller.running = false
            cursorShape = Qt.ArrowCursor
            if (selectionRect.visible) {
                if (selectionRect.width)
                    timeline.setZoomedRange(selectionRect.x, selectionRect.width)
                selectionRect.visible = false
                rejectContextMenu = true
            }
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: timeline.scrollBehaviorViewModel
        onZoomed: function (ratioX, _, x, _, isPhysicalWheel) {
            timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel)
        }
        onMoved: function (x, _, isPhysicalWheel) {
            timeManipulator.moveViewBy(x, isPhysicalWheel)
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: timeline.scrollBehaviorViewModel
        onMoved: function (x) {
            timeManipulator.moveViewBy(x)
        }
    }

}