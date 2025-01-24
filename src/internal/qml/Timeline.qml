import QtQml
import QtQuick
import QtQuick.Shapes

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Palette as ScopicFlowPalette

Item {
    id: timeline

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null

    readonly property double primaryIndicatorX: locator.mapToX(playbackViewModel?.primaryPosition ?? 0)
    readonly property double secondaryIndicatorX: locator.mapToX(playbackViewModel?.secondaryPosition ?? 0)
    readonly property double cursorIndicatorX: locator.mapToX(playbackViewModel?.cursorPosition ?? -1)

    function setIndicatorPosition(x) {
        if (!timeViewModel || !timeLayoutViewModel || !playbackViewModel)
            return
        let tick = locator.alignTick(Math.max(0, locator.mapToTick(x)))
        if (locator.mapToX(tick) < 0)
            tick += timeLayoutViewModel.positionAlignment
        else if (locator.mapToX(tick) > width)
            tick -= timeLayoutViewModel.positionAlignment
        playbackViewModel.primaryPosition = playbackViewModel.secondaryPosition = tick
    }
    function setZoomedRange(selectionX, selectionWidth) {
        if (!timeViewModel || !timeLayoutViewModel)
            return
        let start = locator.mapToTick(selectionX)
        let end = locator.mapToTick(selectionX + selectionWidth)
        if (end - start < timeLayoutViewModel.positionAlignment)
            return
        timeViewModel.start = start
        timeLayoutViewModel.pixelDensity = Math.max(timeLayoutViewModel.minimumPixelDensity, Math.min(width / (end - start), timeLayoutViewModel.maximumPixelDensity))
    }
    function moveViewOnDraggingPositionIndicator(deltaX) {
        if (!timeViewModel || !timeLayoutViewModel || !playbackViewModel)
            return
        let newStart = Math.max(0.0, timeViewModel.start + deltaX / timeLayoutViewModel.pixelDensity)
        let newEnd = newStart + width / timeLayoutViewModel.pixelDensity
        timeViewModel.start = newStart
        timeViewModel.end = Math.max(timeViewModel.end, newEnd)
        if (deltaX < 0) {
            let tick = locator.alignTickCeil(Math.max(0, locator.mapToTick(0)))
            playbackViewModel.primaryPosition = playbackViewModel.secondaryPosition = tick
        } else {
            let tick = locator.alignTickFloor(Math.max(0, locator.mapToTick(width)))
            playbackViewModel.primaryPosition = playbackViewModel.secondaryPosition = tick
        }
    }

    signal positionIndicatorDoubleClicked()
    signal timelineDoubleClicked(tick: int)
    signal contextMenuRequestedForTimeline(tick: int)
    signal contextMenuRequestedForPositionIndicator()

    readonly property QtObject defaultPalette: ScopicFlowPalette.Timeline {

    }

    readonly property QtObject palette: paletteViewModel?.palette?.timeline ?? defaultPalette

    clip: true
    implicitHeight: 24

    TimeAlignmentPositionLocator {
        id: locator
        timeViewModel: timeline.timeViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel
    }

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: timeline.timeViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel
        animationViewModel: timeline.animationViewModel
    }

    Rectangle {
        id: backgroundRect
        anchors.fill: parent
        color: timeline.palette.backgroundColor
    }

    TimelineScale {
        id: timelineScale
        anchors.fill: parent
        color: timeline.palette.foregroundColor
        timeViewModel: timeline.timeViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel
    }


    Rectangle {
        id: selectionRect
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: Qt.rgba(timelineScale.color.r, timelineScale.color.g, timelineScale.color.b, 0.5 * timelineScale.color.a)
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
            PathLine { x: 8 / Math.sqrt(3); y: 0 }
            PathLine { x: 24 / Math.sqrt(3); y: 0 }
            PathArc { x: 28 / Math.sqrt(3); y: 4; radiusX: 4 / 3; radiusY: 4 / 3}
            PathLine { x: 20 / Math.sqrt(3); y: 12 }
            PathArc { x: 12 / Math.sqrt(3); y: 12; radiusX: 4 / 3; radiusY: 4 / 3}
            PathLine { x: 4 / Math.sqrt(3); y: 4}
            PathArc {x: 8 / Math.sqrt(3); y: 0; radiusX: 4 / 3; radiusY: 4 / 3}
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
        focusPolicy: Qt.StrongFocus

        property bool rejectContextMenu: false

        DragScroller {
            id: dragScroller
            onMoved: function (deltaX) {
                timeline.moveViewOnDraggingPositionIndicator(deltaX)
            }
        }

        onPressed: function (mouse) {
            rejectContextMenu = false
        }
        Timer {
            id: clickTimer
            interval: 0
            property var button: null
            property double x: 0
            property double y: 0
            onTriggered: {
                if (button === Qt.LeftButton) {
                    timeline.setIndicatorPosition(x)
                } else if (button === Qt.RightButton && !parent.rejectContextMenu) {
                    if (primaryIndicator.contains(mapToItem(primaryIndicator, x, y))) {
                        timeline.contextMenuRequestedForPositionIndicator()
                    } else {
                        timeline.contextMenuRequestedForTimeline(locator.mapToTick(x))
                    }
                }
            }
        }
        onClicked: function (mouse) {
            clickTimer.button = mouse.button
            clickTimer.x = mouse.x
            clickTimer.y = mouse.y
            clickTimer.start()
        }
        onDoubleClicked : function (mouse) {
            if (clickTimer.running)
                clickTimer.stop()
            if (mouse.button === Qt.LeftButton) {
                if (primaryIndicator.contains(mapToItem(primaryIndicator, mouse.x, mouse.y))) {
                    timeline.positionIndicatorDoubleClicked()
                } else {
                    timeline.timelineDoubleClicked(locator.mapToTick(mouse.x))
                }
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
                    timeline.setIndicatorPosition(mouse.x)
                    dragScroller.running = false
                }

            } else if (pressedButtons & Qt.RightButton) {
                let alignedX = Math.min(Math.max(0, locator.alignedX(mouse.x)), timeline.width)
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
            dragScroller.running = false
            cursorShape = Qt.ArrowCursor
            if (selectionRect.visible) {
                if (selectionRect.width)
                    timeline.setZoomedRange(selectionRect.x, selectionRect.width)
                selectionRect.visible = false
                rejectContextMenu = true
            }
        }
        onCanceled: {
            dragScroller.running = false
            cursorShape = Qt.ArrowCursor
            selectionRect.visible = false
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
        direction: Qt.Horizontal
        onMoved: function (x) {
            timeManipulator.moveViewBy(x)
        }
    }

}