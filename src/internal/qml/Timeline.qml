import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

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

    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: timeline.timeViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel

        Rectangle {
            id: zoomRubberBand
            x: 0
            y: 0
            width: 0
            height: parent.height
            color: Qt.rgba(timelineScale.color.r, timelineScale.color.g, timelineScale.color.b, 0.5 * timelineScale.color.a)
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
