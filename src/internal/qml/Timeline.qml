import QtQml
import QtQuick
import QtQuick.Shapes

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: timeline

    property QtObject animationViewModel: null
    readonly property double cursorIndicatorX: locator.mapToX(playbackViewModel?.cursorPosition ?? -1)
    property QtObject interactionControllerNotifier: null
    property QtObject playbackViewModel: null
    readonly property double primaryIndicatorX: locator.mapToX(playbackViewModel?.primaryPosition ?? 0)
    property QtObject scrollBehaviorViewModel: null
    readonly property double secondaryIndicatorX: locator.mapToX(playbackViewModel?.secondaryPosition ?? 0)
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null

    function moveViewOnDraggingPositionIndicator(deltaX) {
        if (!timeViewModel || !timeLayoutViewModel || !playbackViewModel)
            return;
        timeManipulator.moveViewBy(deltaX);
        setIndicatorPosition(deltaX < 0 ? 0 : width);
    }
    function setIndicatorPosition(x) {
        if (!timeViewModel || !timeLayoutViewModel || !playbackViewModel)
            return;
        playbackViewModel.primaryPosition = playbackViewModel.secondaryPosition = locator.alignTickVisible(locator.mapToTick(x));
    }
    function setZoomedRange(selectionX, selectionWidth) {
        if (!timeViewModel || !timeLayoutViewModel)
            return;
        let start = locator.mapToTick(selectionX);
        let end = locator.mapToTick(selectionX + selectionWidth);
        if (end - start < timeLayoutViewModel.positionAlignment)
            return;
        timeViewModel.start = start;
        timeLayoutViewModel.pixelDensity = Math.max(timeLayoutViewModel.minimumPixelDensity, Math.min(width / (end - start), timeLayoutViewModel.maximumPixelDensity));
    }

    clip: true
    implicitHeight: 24

    TimeAlignmentPositionLocator {
        id: locator

        anchors.fill: parent
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }
    TimeManipulator {
        id: timeManipulator

        anchors.fill: parent
        animationViewModel: timeline.animationViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
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
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }
    Item {
        id: rubberBandLayerViewport

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        width: (timeline.timeViewModel?.end ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)
        x: -(timeline.timeViewModel?.start ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)

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

        anchors.bottom: parent.bottom
        height: 14.3333
        width: 32 / Math.sqrt(3)
        x: timeline.secondaryIndicatorX - 16 / Math.sqrt(3)

        ShapePath {
            id: indicatorPath

            fillColor: SFPalette.playheadSecondaryColor
            strokeWidth: 0

            PathLine {
                x: 8 / Math.sqrt(3)
                y: 0
            }
            PathLine {
                x: 24 / Math.sqrt(3)
                y: 0
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: 28 / Math.sqrt(3)
                y: 4
            }
            PathLine {
                x: 20 / Math.sqrt(3)
                y: 12
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: 12 / Math.sqrt(3)
                y: 12
            }
            PathLine {
                x: 4 / Math.sqrt(3)
                y: 4
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: 8 / Math.sqrt(3)
                y: 0
            }
        }
    }
    Shape {
        id: primaryIndicator

        anchors.bottom: parent.bottom
        height: 14.3333
        width: 32 / Math.sqrt(3)
        x: timeline.primaryIndicatorX - 16 / Math.sqrt(3)

        ShapePath {
            fillColor: SFPalette.playheadPrimaryColor
            strokeWidth: 0

            PathLine {
                x: 8 / Math.sqrt(3)
                y: 0
            }
            PathLine {
                x: 24 / Math.sqrt(3)
                y: 0
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: 28 / Math.sqrt(3)
                y: 4
            }
            PathLine {
                x: 20 / Math.sqrt(3)
                y: 12
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: 12 / Math.sqrt(3)
                y: 12
            }
            PathLine {
                x: 4 / Math.sqrt(3)
                y: 4
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: 8 / Math.sqrt(3)
                y: 0
            }
        }
    }
    Rectangle {
        id: cursorIndicator

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: SFPalette.cursorIndicatorColor
        visible: timeline.cursorIndicatorX >= 0
        width: 1
        x: timeline.cursorIndicatorX - 0.5
    }
    MouseArea {
        id: mouseArea

        property double pressedX: 0
        property bool rejectContextMenu: false

        function emitInteractionNotificationSignal(interactionType) {
            let position = containsMouse || pressed ? locator.mapToTick(mouseX) : -1;
            let flag = (containsMouse || pressed) && primaryIndicator.contains(mapToItem(primaryIndicator, mouseX, mouseY)) ? ScopicFlow.InteractionOnPositionIndicator : ScopicFlow.InteractionOnTimeline;
            timeline.interactionControllerNotifier?.sceneInteracted(interactionType, timeline.timeViewModel, timeline.timeLayoutViewModel, position, 0, flag);
        }
        function handleBeforeInteractionNotification(interactionType) {
            let position = containsMouse || pressed ? locator.mapToTick(mouseX) : -1;
            let flag = (containsMouse || pressed) && primaryIndicator.contains(mapToItem(primaryIndicator, mouseX, mouseY)) ? ScopicFlow.InteractionOnPositionIndicator : ScopicFlow.InteractionOnTimeline;
            return timeline.interactionControllerNotifier?.handleSceneInteraction(interactionType, timeline.timeViewModel, timeline.timeLayoutViewModel, position, 0, flag);
        }
        function handlePositionChanged(x, button) {
            if (button === Qt.LeftButton) {
                timeline.setIndicatorPosition(x);
            } else {
                rejectContextMenu = true;
                if (!rubberBandLayer.started) {
                    rubberBandLayer.startSelection(mapToItem(rubberBandLayerViewport, locator.mapToX(locator.alignTickVisible(locator.mapToTick(pressedX))), 0));
                }
                rubberBandLayer.updateSelection(mapToItem(rubberBandLayerViewport, locator.mapToX(locator.alignTickVisible(locator.mapToTick(x))), timeline.height));
            }
        }
        function sendInteractionNotification(interactionType, mouse = null) {
            if (handleBeforeInteractionNotification(interactionType, mouse))
                return false;
            emitInteractionNotificationSignal(interactionType, mouse);
            return true;
        }

        acceptedButtons: Qt.LeftButton | Qt.RightButton
        anchors.fill: parent
        drag.axis: Drag.XAxis
        drag.minimumX: timeline.zeroTickX - 8
        focusPolicy: Qt.StrongFocus
        hoverEnabled: true

        onCanceled: () => {
            dragScroller.running = false;
            cursorShape = Qt.ArrowCursor;
            rubberBandLayer.endSelection();
            sendInteractionNotification(ScopicFlow.II_Canceled);
        }
        onClicked: mouse => {
            if (mouse.button === Qt.LeftButton) {
                if (handleBeforeInteractionNotification(ScopicFlow.II_Clicked))
                    return;
                timeline.setIndicatorPosition(mouse.x);
                emitInteractionNotificationSignal(ScopicFlow.II_Clicked);
            } else if (mouse.button === Qt.RightButton && !rejectContextMenu) {
                sendInteractionNotification(ScopicFlow.II_ContextMenu);
            }
        }
        onDoubleClicked: sendInteractionNotification(ScopicFlow.II_DoubleClicked)
        onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
        onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
        onPositionChanged: mouse => {
            if (!pressed)
                return;
            dragScroller.button = mouse.buttons;
            dragScroller.determine(mouse.x, timeline.width, 0, 0, triggered => {
                if (triggered)
                    return;
                handlePositionChanged(mouse.x, mouse.buttons);
            });
        }
        onPressed: mouse => {
            rejectContextMenu = false;
            pressedX = mouse.x;
            if (!sendInteractionNotification(ScopicFlow.II_Pressed)) {
                mouse.accepted = false;
            }
        }
        onReleased: () => {
            dragScroller.running = false;
            cursorShape = Qt.ArrowCursor;
            let rect = mapFromItem(rubberBandLayerViewport, rubberBandLayer.endSelection());
            timeline.setZoomedRange(rect.x, rect.width);
            sendInteractionNotification(ScopicFlow.II_Released);
        }

        DragScroller {
            id: dragScroller

            property int button: 0

            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX);
                mouseArea.handlePositionChanged(deltaX < 0 ? 0 : timeline.width, button);
            }
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Horizontal
        viewModel: timeline.scrollBehaviorViewModel

        onMoved: (x, _, isPhysicalWheel) => {
            timeManipulator.moveViewBy(x, isPhysicalWheel);
        }
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => {
            timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel);
        }
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        direction: Qt.Horizontal
        viewModel: timeline.scrollBehaviorViewModel

        onMoved: x => {
            timeManipulator.moveViewBy(x);
        }
    }
}
