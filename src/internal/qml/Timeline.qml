import QtQml
import QtQuick
import QtQuick.Shapes

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: timeline

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null

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
        color: SFPalette.timelineBackgroundColor
    }

    TimelineScale {
        id: timelineScale
        anchors.fill: parent
        color: SFPalette.suitableForegroundColor(SFPalette.timelineBackgroundColor)
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
            fillColor: SFPalette.playheadSecondaryColor
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
            fillColor: SFPalette.playheadPrimaryColor
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
        color: SFPalette.cursorIndicatorColor
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
        hoverEnabled: true

        property bool rejectContextMenu: false
        property double pressedX: 0

        function sendInteractionNotification(interactionType, mouse = null) {
            if (handleBeforeInteractionNotification(interactionType, mouse))
                return false
            emitInteractionNotificationSignal(interactionType, mouse)
            return true
        }
        function handleBeforeInteractionNotification(interactionType) {
            let position = containsMouse || pressed ? locator.mapToTick(mouseX) : -1
            let flag = (containsMouse || pressed) && primaryIndicator.contains(mapToItem(primaryIndicator, mouseX, mouseY)) ? ScopicFlow.InteractionOnPositionIndicator : ScopicFlow.InteractionOnTimeline
            return timeline.interactionControllerNotifier?.handleSceneInteraction(interactionType, timeline.timeViewModel, timeline.timeLayoutViewModel, position, 0, flag)
        }
        function emitInteractionNotificationSignal(interactionType) {
            let position = containsMouse || pressed ? locator.mapToTick(mouseX) : -1
            let flag = (containsMouse || pressed) && primaryIndicator.contains(mapToItem(primaryIndicator, mouseX, mouseY)) ? ScopicFlow.InteractionOnPositionIndicator : ScopicFlow.InteractionOnTimeline
            timeline.interactionControllerNotifier?.sceneInteracted(interactionType, timeline.timeViewModel, timeline.timeLayoutViewModel, position, 0, flag)
        }
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
            onMoved: (deltaX) => {
                timeManipulator.moveViewBy(deltaX)
                mouseArea.handlePositionChanged(deltaX < 0 ? 0 : timeline.width, button)
            }
        }

        onPressed: (mouse) => {
            rejectContextMenu = false
            pressedX = mouse.x
            if (!sendInteractionNotification(ScopicFlow.II_Pressed)) {
                mouse.accepted = false
            }
        }
        onReleased: () => {
            dragScroller.running = false
            cursorShape = Qt.ArrowCursor
            let rect = mapFromItem(rubberBandLayerViewport, rubberBandLayer.endSelection())
            timeline.setZoomedRange(rect.x, rect.width)
            sendInteractionNotification(ScopicFlow.II_Released)
        }
        onCanceled: () => {
            dragScroller.running = false
            cursorShape = Qt.ArrowCursor
            rubberBandLayer.endSelection()
            sendInteractionNotification(ScopicFlow.II_Canceled)
        }
        onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
        onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
        onClicked: (mouse) => {
            if (mouse.button === Qt.LeftButton) {
                if (handleBeforeInteractionNotification(ScopicFlow.II_Clicked))
                    return
                timeline.setIndicatorPosition(mouse.x)
                emitInteractionNotificationSignal(ScopicFlow.II_Clicked)
            } else if (mouse.button === Qt.RightButton && !rejectContextMenu) {
                sendInteractionNotification(ScopicFlow.II_ContextMenu)
            }
        }
        onDoubleClicked: sendInteractionNotification(ScopicFlow.II_DoubleClicked)
        onPositionChanged: (mouse) => {
            if (!pressed)
                return
            dragScroller.button = mouse.buttons
            dragScroller.determine(mouse.x, timeline.width, 0, 0, (triggered) => {
                if (triggered)
                    return
                handlePositionChanged(mouse.x, mouse.buttons)
            })
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: timeline.scrollBehaviorViewModel
        movableOrientation: Qt.Horizontal
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => {
            timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel)
        }
        onMoved: (x, _, isPhysicalWheel) => {
            timeManipulator.moveViewBy(x, isPhysicalWheel)
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: timeline.scrollBehaviorViewModel
        direction: Qt.Horizontal
        onMoved: (x) => {
            timeManipulator.moveViewBy(x)
        }
    }

}