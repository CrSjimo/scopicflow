import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: timeline

    required property TimeLayoutViewModel timeLayoutViewModel
    required property TimeViewModel timeViewModel
    property PlaybackViewModel playbackViewModel: null
    property TimelineInteractionController timelineInteractionController: null

    readonly property bool canMovePlayhead: playbackViewModel !== null
        && timeViewModel !== null
        && (timeLayoutViewModel?.pixelDensity ?? 0) > 0
        && (timeLayoutViewModel?.positionAlignment ?? 0) > 0
        && Boolean((timelineInteractionController?.interaction ?? 0) & TimelineInteractionController.MovePlayhead)

    onTimeViewModelChanged: pointerRouter.cancel()
    onTimeLayoutViewModelChanged: pointerRouter.cancel()
    onPlaybackViewModelChanged: pointerRouter.cancel()
    onTimelineInteractionControllerChanged: pointerRouter.cancel()
    onCanMovePlayheadChanged: {
        if (!canMovePlayhead)
            pointerRouter.cancel()
    }

    Accessible.name: qsTr("Absolute Timeline")
    Accessible.role: Accessible.Pane
    clip: true
    implicitHeight: 20
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    function seekToX(x: real) {
        if (!canMovePlayhead)
            return

        const position = timeManipulator.alignPosition(timeManipulator.mapToPosition(x), ScopicFlow.AO_Visible)
        playbackViewModel.primaryPosition = playbackViewModel.secondaryPosition = Math.max(0, position)
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

    AbsoluteTimelineScale {
        anchors.fill: parent
        color: Theme.foregroundColor(timeline.ThemedItem.foregroundLevel)
        font: Theme.font
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }

    DispatchedDragHandler {
        id: playheadDragHandler

        property TimelineInteractionController activeController: null

        function finishMoving() {
            playheadDragScroller.running = false
            activeController?.positionIndicatorMovingFinished(timeline)
            activeController = null
        }

        onStarted: event => {
            activeController = timeline.timelineInteractionController
            activeController.positionIndicatorMovingStarted(timeline)
            timeline.seekToX(event.position.x)
        }
        onMoved: event => {
            playheadDragScroller.determine(event.position.x, event.surfaceRect.width, 0, 0, triggered => {
                if (!triggered)
                    timeline.seekToX(event.position.x)
            })
        }
        onFinished: finishMoving()
        onCanceled: finishMoving()

        DragScroller {
            id: playheadDragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX)
                timeline.seekToX(deltaX < 0 ? 0 : timeline.width)
            }
        }
    }

    PointerInteractionRouter {
        id: pointerRouter
        onClicked: event => timeline.seekToX(event.position.x)
    }

    PointerInputArea {
        anchors.fill: parent
        enabled: timeline.canMovePlayhead
        acceptedButtons: Qt.LeftButton
        hoverEnabled: false
        router: pointerRouter
        handlerResolver: () => playheadDragHandler
    }
}
