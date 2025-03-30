import QtQml
import QtQuick
import QtQuick.Shapes

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: timeline

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null

    property QtObject stylesheet: TimelineStylesheet {}
    readonly property QtObject timelineStyleItem: stylesheet.timeline.createObject(timeline)
    readonly property QtObject timeIndicatorsStyleItem: stylesheet.timeIndicators.createObject(timeline)

    readonly property double primaryIndicatorX: locator.mapToX(playbackViewModel?.primaryPosition ?? 0)
    readonly property double secondaryIndicatorX: locator.mapToX(playbackViewModel?.secondaryPosition ?? 0)
    readonly property double cursorIndicatorX: locator.mapToX(playbackViewModel?.cursorPosition ?? -1)

    function setIndicatorPosition(x) {
        if (!timeViewModel || !timeLayoutViewModel || !playbackViewModel)
            return
        playbackViewModel.primaryPosition = playbackViewModel.secondaryPosition = locator.alignTickVisible(locator.mapToTick(x))
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
        timeManipulator.moveViewBy(deltaX)
        setIndicatorPosition(deltaX < 0 ? 0 : width)
    }

    signal positionIndicatorDoubleClicked()
    signal timelineDoubleClicked(tick: int)
    signal contextMenuRequestedForTimeline(tick: int)
    signal contextMenuRequestedForPositionIndicator()

    clip: true
    implicitHeight: 24

    TimeAlignmentPositionLocator {
        id: locator
        anchors.fill: parent
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
        color: timeline.timelineStyleItem.background
        border.width: 1
        border.color: timeline.timelineStyleItem.border
    }

    TimelineScale {
        id: timelineScale
        anchors.fill: parent
        color: timeline.timelineStyleItem.foreground
        timeViewModel: timeline.timeViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel
    }

    Item {
        id: rubberBandLayerViewport
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        x: -(timeline.timeViewModel?.start ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)
        width: (timeline.timeViewModel?.end ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)
        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            rubberBand: Rectangle {
                color: Qt.rgba(timelineScale.color.r, timelineScale.color.g, timelineScale.color.b, 0.5 * timelineScale.color.a)
            }
        }
    }


    Shape {
        id: secondaryIndicator
        width: 32 / Math.sqrt(3)
        height: 14.3333
        anchors.bottom: parent.bottom
        ShapePath {
            id: indicatorPath
            strokeWidth: 0
            fillColor: timeline.timeIndicatorsStyleItem.secondaryIndicator
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
            fillColor: timeline.timeIndicatorsStyleItem.primaryIndicator
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
        color: timeline.timeIndicatorsStyleItem.cursorIndicator
        x: timeline.cursorIndicatorX - 0.5
        visible: timeline.cursorIndicatorX >= 0
    }

    MouseArea {
        id: mouseArea
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        drag.axis: Drag.XAxis
        drag.minimumX: timeline.zeroTickX - 8
        focusPolicy: Qt.StrongFocus

        property bool rejectContextMenu: false
        property double pressedX: 0

        function handlePositionChanged(x, button) {
            if (button === Qt.LeftButton) {
                timeline.setIndicatorPosition(x)
            } else {
                rejectContextMenu = true
                if (!rubberBandLayer.started) {
                    rubberBandLayer.startSelection(mapToItem(rubberBandLayerViewport, locator.mapToX(locator.alignTickVisible(locator.mapToTick(pressedX))), 0))
                }
                rubberBandLayer.updateSelection(mapToItem(rubberBandLayerViewport, locator.mapToX(locator.alignTickVisible(locator.mapToTick(x))), timeline.height))
            }
        }

        DragScroller {
            id: dragScroller
            property int button: 0
            onMoved: function (deltaX) {
                timeManipulator.moveViewBy(deltaX)
                mouseArea.handlePositionChanged(deltaX < 0 ? 0 : timeline.width, button)
            }
        }

        onPressed: function (mouse) {
            rejectContextMenu = false
            pressedX = mouse.x
        }
        onPositionChanged: function (mouse) {
            dragScroller.button = mouse.buttons
            dragScroller.determine(mouse.x, timeline.width, 0, 0, (triggered) => {
                if (triggered)
                    return
                handlePositionChanged(mouse.x, mouse.buttons)
            })
        }
        onReleased: {
            dragScroller.running = false
            cursorShape = Qt.ArrowCursor
            let rect = mapFromItem(rubberBandLayerViewport, rubberBandLayer.endSelection())
            timeline.setZoomedRange(rect.x, rect.width)
        }
        onCanceled: {
            dragScroller.running = false
            cursorShape = Qt.ArrowCursor
            rubberBandLayer.endSelection()
        }
        onClicked: function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                timeline.setIndicatorPosition(mouse.x)
            } else if (mouse.button === Qt.RightButton && !rejectContextMenu) {
                if (primaryIndicator.contains(mapToItem(primaryIndicator, mouse.x, mouse.y))) {
                    timeline.contextMenuRequestedForPositionIndicator()
                } else {
                    timeline.contextMenuRequestedForTimeline(locator.mapToTick(mouse.x))
                }
            }
        }
        onDoubleClicked : function (mouse) {
            if (mouse.button === Qt.LeftButton) {
                if (primaryIndicator.contains(mapToItem(primaryIndicator, mouse.x, mouse.y))) {
                    timeline.positionIndicatorDoubleClicked()
                } else {
                    timeline.timelineDoubleClicked(locator.mapToTick(mouse.x))
                }
            }
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: timeline.scrollBehaviorViewModel
        movableOrientation: Qt.Horizontal
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