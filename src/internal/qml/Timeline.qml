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

    GenericBackRightButtonMouseArea {
        id: rightButtonMouseArea

        controller: timelineInteractionController
        timeManipulator: timeManipulator
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
            timeline.playbackViewModel.primaryPosition = timeline.playbackViewModel.secondaryPosition = timeManipulator.alignPosition(timeManipulator.mapToPosition(x), ScopicFlow.AO_Visible);
        }
        function setZoomedRange(selectionX, selectionWidth) {
            if (!timeline.timeViewModel || !timeline.timeLayoutViewModel)
                return;
            let start = timeManipulator.mapToPosition(selectionX);
            let end = timeManipulator.mapToPosition(selectionX + selectionWidth);
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
                timeline.timelineInteractionController.rubberBandDraggingFinished(timeline)
            } else if (isMove) {
                timeline.timelineInteractionController.positionIndicatorMovingFinished(timeline)
            }
        }
        onClicked: mouse => {

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
                timeline.timelineInteractionController.rubberBandDraggingStarted(timeline)
                zoomRubberBand.visible = true
            } else if (isMove) {
                timeline.timelineInteractionController.positionIndicatorMovingStarted(timeline)
                setIndicatorPosition(mouse.x);
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
                timeline.timelineInteractionController.doubleClicked(timeline, timeManipulator.mapToPosition(mouse.x))
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

    GenericBackHoverMouseArea {
        id: hoverMouseArea

        controller: timeline.timelineInteractionController
        target: timeline
        timeManipulator: timeManipulator

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
