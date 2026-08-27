import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Shapes

import SVSCraft
import SVSCraft.UIComponents
import SVSCraft.UIComponents.impl

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: timeline

    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property PlaybackViewModel playbackViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimelineInteractionController timelineInteractionController: null

    onTimeViewModelChanged: pointerRouter.cancel()

    Accessible.name: qsTr("Timeline")
    Accessible.role: Accessible.Pane
    focus: true
    focusPolicy: Qt.StrongFocus
    clip: true
    implicitHeight: 32
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    Keys.onMenuPressed: () => {
        if (timelineInteractionController) {
            timelineInteractionController.contextMenuRequested(timeline, -1)
        }
    }

    TimeManipulator {
        id: timeManipulator
        target: timeline
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundColor(timeline.ThemedItem.backgroundLevel)
    }

    TimelineScale {
        id: timelineScale
        anchors.fill: parent
        color: Theme.foregroundColor(timeline.ThemedItem.foregroundLevel)
        font: Theme.font
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }

    DispatchedDragHandler {
        id: playheadDragHandler
        onStarted: event => {
            timeline.timelineInteractionController.positionIndicatorMovingStarted(timeline)
            playheadDragScroller.setIndicatorPosition(event.position.x)
        }
        onMoved: event => {
            playheadDragScroller.determine(
                event.position.x, event.surfaceRect.width, 0, 0, triggered => {
                if (triggered)
                    return
                playheadDragScroller.setIndicatorPosition(event.position.x)
            })
        }
        onFinished: {
            timeline.timelineInteractionController.positionIndicatorMovingFinished(timeline)
            playheadDragScroller.running = false
        }
        onCanceled: {
            timeline.timelineInteractionController.positionIndicatorMovingFinished(timeline)
            playheadDragScroller.running = false
        }
        DragScroller {
            id: playheadDragScroller
            function setIndicatorPosition(x) {
                timeline.playbackViewModel.primaryPosition = timeline.playbackViewModel.secondaryPosition = timeManipulator.alignPosition(timeManipulator.mapToPosition(x), ScopicFlow.AO_Visible);
            }
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX);
                setIndicatorPosition(deltaX < 0 ? 0 : timeline.width);
            }
        }
    }

    DispatchedDragHandler {
        id: zoomDragHandler
        onStarted: event => {
            timeline.timelineInteractionController.rubberBandDraggingStarted(timeline)
            zoomDragScroller.rubberBandOrigin = timeline.mapToItem(
                viewportContainer, event.position).x
            zoomRubberBand.visible = true
        }
        onMoved: event => {
            zoomDragScroller.determine(
                event.position.x, event.surfaceRect.width, 0, 0, triggered => {
                if (triggered)
                    return
                zoomDragScroller.handlePositionChanged(event.position.x)
            })
        }
        onFinished: {
            const selectionX = timeline.mapFromItem(
                viewportContainer, zoomRubberBand.x, 0).x
            const selectionWidth = zoomRubberBand.width
            const start = timeManipulator.mapToPosition(selectionX)
            const end = timeManipulator.mapToPosition(selectionX + selectionWidth)
            zoomDragScroller.running = false
            zoomRubberBand.x = zoomRubberBand.width = 0
            zoomRubberBand.visible = false
            if (end - start < timeline.timeLayoutViewModel.positionAlignment) {
                timeline.timelineInteractionController.rubberBandDraggingAborted(timeline)
                return
            }
            timeline.timeViewModel.start = start
            timeline.timeLayoutViewModel.pixelDensity = Math.max(
                timeline.timeLayoutViewModel.minimumPixelDensity,
                Math.min(surfaceRect.width / (end - start),
                         timeline.timeLayoutViewModel.maximumPixelDensity))
            timeline.timelineInteractionController.rubberBandDraggingCommitted(timeline)
        }
        onCanceled: {
            zoomDragScroller.running = false
            zoomRubberBand.x = zoomRubberBand.width = 0
            zoomRubberBand.visible = false
            timeline.timelineInteractionController.rubberBandDraggingAborted(timeline)
        }
        DragScroller {
            id: zoomDragScroller
            property double rubberBandOrigin: 0
            function handlePositionChanged(x) {
                const a1 = rubberBandOrigin
                const a2 = timeline.mapToItem(viewportContainer, x, 0).x
                zoomRubberBand.x = Math.min(a1, a2)
                zoomRubberBand.width = Math.abs(a1 - a2)
            }
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX);
                handlePositionChanged(deltaX < 0 ? 0 : timeline.width);
            }
        }
    }

    PointerInteractionRouter {
        id: pointerRouter
    }

    PointerInputArea {
        anchors.fill: parent
        router: pointerRouter
        coordinateSpace: timeline
        hitResolver: (point, _) => ({
            valid: true,
            target: null,
            targetRect: Qt.rect(0, 0, timeline.width, timeline.height),
            hoverRegion: 0,
            payload: undefined,
        })
        handlerResolver: (event, hit) => {
            if (!timeline.timelineInteractionController)
                return null
            if ((event.modifiers & Qt.ControlModifier)
                    && (timeline.timelineInteractionController.interaction
                        & TimelineInteractionController.ZoomByRubberBand)) {
                return zoomDragHandler
            }
            if ((event.modifiers & Qt.AltModifier)
                    && (timeline.timelineInteractionController.interaction
                        & TimelineInteractionController.AdjustLoopRange)) {
                return null
            }
            return timeline.timelineInteractionController.interaction
                    & TimelineInteractionController.MovePlayhead
                ? playheadDragHandler : null
        }
    }

    Connections {
        target: pointerRouter

        function onClicked(event, hit) {
            if (!timeline.playbackViewModel)
                return
            timeline.playbackViewModel.primaryPosition
                = timeline.playbackViewModel.secondaryPosition
                = timeManipulator.alignPosition(
                    timeManipulator.mapToPosition(event.position.x), ScopicFlow.AO_Visible)
        }

        function onDoubleClicked(event, hit) {
            timeline.timelineInteractionController?.doubleClicked(
                timeline, timeManipulator.mapToPosition(event.position.x))
        }

        function onContextMenuRequested(event, hit) {
            timeline.timelineInteractionController?.contextMenuRequested(
                timeline, timeManipulator.mapToPosition(event.position.x))
        }

        function onHoverEntered(event, hit) {
            timeline.timelineInteractionController?.hoverEntered(
                timeline, timeManipulator.mapToPosition(event.position.x))
        }

        function onHoverMoved(event, hit) {
            timeline.timelineInteractionController?.hoverMoved(
                timeline, timeManipulator.mapToPosition(event.position.x))
        }

        function onHoverExited(hit) {
            timeline.timelineInteractionController?.hoverExited(timeline)
        }
    }

    component LoopMouseArea: MouseArea {
        id: loopMouseArea
        acceptedButtons: Qt.LeftButton
        anchors.fill: parent
        visible: Boolean(timeline.timelineInteractionController?.interaction & TimelineInteractionController.AdjustLoopRange)
        hoverEnabled: true
        cursorShape: Qt.SizeHorCursor
        CursorBinding.cursorShape: cursorShape

        property bool dragged: false
        property double pressedDeltaX: 0
        property double pressedDeltaXBias: 0
        required property int adjustmentOperation

        signal positionUpdated(position: int)

        function moveToX(x) {
            let position = timeManipulator.alignPosition(timeManipulator.mapToPosition(x))
            positionUpdated(position)
        }

        onPressed: (mouse) => {
            CursorBinding.enabled = true
            dragged = false
            pressedDeltaX = mouse.x + pressedDeltaXBias;
        }

        onPositionChanged: (mouse) => {
            if (!pressed)
                return
            if (!dragged) {
                dragged = true
                timeline.timelineInteractionController.loopRangeAdjustingStarted(timeline, adjustmentOperation)
            }
            let parentX = mapToItem(timeline, mouse.x, 0).x;
            loopDragScroller.determine(parentX, timeline.width, 0, 0, triggered => {
                if (triggered)
                    return;
                moveToX(parentX - pressedDeltaX);
            });
        }

        onCanceled: () => {
            CursorBinding.enabled = false
            loopDragScroller.running = false
            timeline.timelineInteractionController.loopRangeAdjustingFinished(timeline, adjustmentOperation)
        }

        onReleased: canceled()

        onEntered: () => {
            timeline.timelineInteractionController.loopHoverEntered(timeline, adjustmentOperation)
        }

        onExited: () => {
            timeline.timelineInteractionController.loopHoverExited(timeline, adjustmentOperation)
        }

        DragScroller {
            id: loopDragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX);
                loopMouseArea.moveToX((deltaX < 0 ? 0 : timeline.width) - loopMouseArea.pressedDeltaX);
            }
        }

    }

    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: timeline.timeViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel

        Rectangle {
            id: loopRangeRect
            anchors.top: parent.top
            visible: (timeline.playbackViewModel?.loopLength ?? -1) >= 0
            x: (timeline.playbackViewModel?.loopStart ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)
            width: (timeline.playbackViewModel?.loopLength ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)
            height: 4
            color: SFPalette.loopColor

            LoopMouseArea {
                adjustmentOperation: TimelineInteractionController.AdjustRange
                onPositionUpdated: (position) => {
                    timeline.playbackViewModel.loopStart = position;
                }
            }
        }

        Item {
            id: loopStartIndicator
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: (timeline.playbackViewModel?.loopLength ?? -1) >= 0
            x: (timeline.playbackViewModel?.loopStart ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0) - 2
            width: 4
            Rectangle {
                x: 1
                width: 1
                height: 12
                color: SFPalette.loopColor
            }
            Shape {
                anchors.left: parent.horizontalCenter
                anchors.leftMargin: -0.5
                width: 12
                height: 12
                ShapePath {
                    strokeWidth: 1
                    strokeColor: SFPalette.loopColor
                    fillColor: Qt.rgba(SFPalette.loopColor.r, SFPalette.loopColor.g, SFPalette.loopColor.b, 0.5 * SFPalette.loopColor.a)
                    PathLine {
                        x: 12
                        y: 0
                    }
                    PathLine {
                        x: 0
                        y: 12
                    }
                }
            }
            LoopMouseArea {
                adjustmentOperation: TimelineInteractionController.AdjustStart
                anchors.bottomMargin: {
                    if (!timeline.playbackViewModel)
                        return 0
                    let a = timeline.playbackViewModel.loopStart
                    let b = timeline.playbackViewModel.primaryPosition
                    let c = timeline.playbackViewModel.secondaryPosition
                    return Math.min(Math.abs(a - b), Math.abs(a - c)) <= 8 / (timeline.timeLayoutViewModel?.pixelDensity ?? 1) ? 18.333333333333334 : 0
                }
                onPositionUpdated: (position) => {
                    let delta = position - timeline.playbackViewModel.loopStart
                    if (timeline.playbackViewModel.loopLength - delta <= 0) {
                        return
                    }
                    timeline.playbackViewModel.loopStart += delta
                    timeline.playbackViewModel.loopLength -= delta
                }
            }
        }

        Item {
            id: loopEndIndicator
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: (timeline.playbackViewModel?.loopLength ?? -1) >= 0
            x: ((timeline.playbackViewModel?.loopStart ?? 0) + (timeline.playbackViewModel?.loopLength ?? 0)) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0) - 2
            width: 4
            Rectangle {
                x: 1
                width: 1
                height: 12
                color: SFPalette.loopColor
            }
            Shape {
                anchors.right: parent.horizontalCenter
                anchors.rightMargin: 0.5
                width: 12
                height: 12
                ShapePath {
                    strokeWidth: 1
                    strokeColor: SFPalette.loopColor
                    fillColor: Qt.rgba(SFPalette.loopColor.r, SFPalette.loopColor.g, SFPalette.loopColor.b, 0.5 * SFPalette.loopColor.a)
                    startX: 12
                    startY: 0
                    PathLine {
                        x: 0
                        y: 0
                    }
                    PathLine {
                        x: 12
                        y: 12
                    }
                }
            }
            LoopMouseArea {
                adjustmentOperation: TimelineInteractionController.AdjustEnd
                anchors.bottomMargin: {
                    if (!timeline.playbackViewModel)
                        return 0
                    let a = timeline.playbackViewModel.loopStart + timeline.playbackViewModel.loopLength
                    let b = timeline.playbackViewModel.primaryPosition
                    let c = timeline.playbackViewModel.secondaryPosition
                    return Math.min(Math.abs(a - b), Math.abs(a - c)) <= 8 / (timeline.timeLayoutViewModel?.pixelDensity ?? 1) ? 18.333333333333334 : 0
                }
                onPositionUpdated: (position) => {
                    let delta = position - (timeline.playbackViewModel.loopStart + timeline.playbackViewModel.loopLength)
                    if (timeline.playbackViewModel.loopLength + delta <= 0) {
                        return
                    }
                    timeline.playbackViewModel.loopLength += delta
                }
            }
        }

        Rectangle {
            id: zoomRubberBand
            x: 0
            y: 0
            width: 0
            height: parent.height
            color: Qt.rgba(timelineScale.color.r, timelineScale.color.g, timelineScale.color.b, 0.5 * timelineScale.color.a)
        }
    }

    PositionIndicators {
        id: positionIndicators

        anchors.fill: parent
        timeViewModel: timeline.timeViewModel
        playbackViewModel: timeline.playbackViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel
        primaryIndicator.visible: false
        secondaryIndicator.visible: false
        cursorIndicator.anchors.topMargin: 16

        PlayheadIndicator {
            anchors.bottom: parent.bottom
            x: positionIndicators.primaryIndicator.x
            color: SFPalette.playheadPrimaryColor
        }

        PlayheadIndicator {
            anchors.bottom: parent.bottom
            x: positionIndicators.secondaryIndicator.x
            color: SFPalette.playheadSecondaryColor
        }

        Text {
            anchors.top: parent.top
            anchors.topMargin: 2
            x: positionIndicators.cursorIndicator.x - implicitWidth / 2
            text: {
                if (!timeline.timeViewModel?.timeline || (timeline.timeLayoutViewModel?.cursorPosition ?? -1) < 0)
                    return ""
                return GlobalHelper.musicTimelineTextFromValue(timeline.timeViewModel.timeline, timeline.timeLayoutViewModel.cursorPosition, 1, 1, 3)
            }
            font.family: Theme.font.family
            font.pixelSize: 12
            color: SFPalette.cursorIndicatorColor
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
            timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel);
        }
    }

}
