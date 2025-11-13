import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Shapes

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

    Accessible.name: qsTr("Timeline")
    Accessible.role: Accessible.Pane
    focus: true
    focusPolicy: Qt.StrongFocus
    clip: true
    implicitHeight: 24
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

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
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }

    Item {
        id: viewportContainer
        x: -(timeline.timeViewModel?.start ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)
        y: 0
        width: (timeline.timeViewModel?.end ?? 0) * (timeline.timeLayoutViewModel?.pixelDensity ?? 0)
        height: parent.height

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
    }

    MouseArea {
        id: leftButtonMouseArea

        property double rubberBandOrigin: 0
        property bool isMove: false
        property bool isZoom: false

        acceptedButtons: Qt.LeftButton
        anchors.fill: parent
        focus: true
        focusPolicy: Qt.ClickFocus

        function setIndicatorPosition(x) {
            if (!timeline.timeViewModel || !timeline.timeLayoutViewModel || !timeline.playbackViewModel)
                return;
            timeline.playbackViewModel.primaryPosition = timeline.playbackViewModel.secondaryPosition = timeManipulator.alignTick(timeManipulator.mapToTick(x), ScopicFlow.AO_Visible);
        }
        function setZoomedRange(selectionX, selectionWidth) {
            if (!timeline.timeViewModel || !timeline.timeLayoutViewModel)
                return;
            let start = timeManipulator.mapToTick(selectionX);
            let end = timeManipulator.mapToTick(selectionX + selectionWidth);
            if (end - start < timeline.timeLayoutViewModel.positionAlignment)
                return;
            timeline.timeViewModel.start = start;
            timeline.timeLayoutViewModel.pixelDensity = Math.max(timeline.timeLayoutViewModel.minimumPixelDensity, Math.min(width / (end - start), timeline.timeLayoutViewModel.maximumPixelDensity));
        }
        function handlePositionChanged(x) {
            if (isZoom) {
                let a1 = rubberBandOrigin
                let a2 = mapToItem(viewportContainer, x, 0).x
                zoomRubberBand.x = Math.min(a1, a2)
                zoomRubberBand.width = Math.abs(a1 - a2)
            } else if (isMove) {
                setIndicatorPosition(x);
            }
        }

        onCanceled: () => {
            dragScroller.running = false;
            cursorShape = undefined;
            zoomRubberBand.x = zoomRubberBand.width = 0
            zoomRubberBand.visible = false
            if (isZoom) {
                timeline.timelineInteractionController.interactionOperationFinished(timeline, TimelineInteractionController.ZoomByRubberBand)
            } else if (isMove) {
                timeline.timelineInteractionController.interactionOperationFinished(timeline, TimelineInteractionController.MovePositionIndicator)
            }
        }
        onClicked: mouse => {
            if (isMove) {
                setIndicatorPosition(mouse.x);
            }
        }
        onPositionChanged: mouse => {
            if (!pressed)
                return;
            dragScroller.determine(mouse.x, timeline.width, 0, 0, triggered => {
                if (triggered)
                    return;
                handlePositionChanged(mouse.x);
            });
        }
        onPressed: mouse => {
            rubberBandOrigin = mapToItem(viewportContainer, mouse.x, 0).x
            isMove = ((timeline.timelineInteractionController?.interaction ?? 0) & TimelineInteractionController.MovePositionIndicator) && !(timeline.scrollBehaviorViewModel?.isZoom(mouse.modifiers) ?? false)
            isZoom = ((timeline.timelineInteractionController?.interaction ?? 0) & TimelineInteractionController.ZoomByRubberBand) && (timeline.scrollBehaviorViewModel?.isZoom(mouse.modifiers) ?? false)
            if (isZoom) {
                timeline.timelineInteractionController.interactionOperationStarted(timeline, TimelineInteractionController.ZoomByRubberBand)
                zoomRubberBand.visible = true
            } else if (isMove) {
                timeline.timelineInteractionController.interactionOperationStarted(timeline, TimelineInteractionController.MovePositionIndicator)
            }
        }
        onReleased: () => {
            if (isZoom) {
                setZoomedRange(mapFromItem(viewportContainer, zoomRubberBand.x, 0).x, zoomRubberBand.width);
            }
            canceled()
        }
        onDoubleClicked: (mouse) => {
            if (timeline.timelineInteractionController) {
                timeline.timelineInteractionController.doubleClicked(timeline, timeManipulator.mapToTick(mouse.x))
            }
        }

        DragScroller {
            id: dragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX);
                leftButtonMouseArea.handlePositionChanged(deltaX < 0 ? 0 : timeline.width);
            }
        }
    }

    MouseArea {
        id: rightButtonMouseArea

        acceptedButtons: Qt.RightButton
        anchors.fill: parent
        focus: true
        focusPolicy: Qt.ClickFocus

        onClicked: mouse => {
            if (timeline.timelineInteractionController) {
                timeline.timelineInteractionController.contextMenuRequested(timeline, timeManipulator.mapToTick(mouse.x))
            }
        }
    }

    MouseArea {
        id: hoverMouseArea

        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        hoverEnabled: true

        onEntered: () => {
            if (timeline.timelineInteractionController) {
                timeline.timelineInteractionController.hoverEntered(timeline, timeManipulator.mapToTick(mouseX))
            }
        }

        onPositionChanged: () => {
            if (timeline.timelineInteractionController) {
                timeline.timelineInteractionController.hoverMoved(timeline, timeManipulator.mapToTick(mouseX))
            }
        }

        onExited: () => {
            if (timeline.timelineInteractionController) {
                timeline.timelineInteractionController.hoverExited(timeline)
            }
        }

    }

    Keys.onMenuPressed: () => {
        if (timeline.timelineInteractionController) {
            timeline.timelineInteractionController.contextMenuRequested(timeline, -1)
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
