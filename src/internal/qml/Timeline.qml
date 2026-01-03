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
                loopMouseArea.moveToX((deltaX < 0 ? 0 : timeline.width) - loopRangeMouseArea.pressedDeltaX);
            }
        }

    }

    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: timeline.timeViewModel
        timeLayoutViewModel: timeline.timeLayoutViewModel

        Rectangle {
            id: loopRangeRect
            anchors.bottom: parent.bottom
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
                width: 2
                height: parent.height
                color: SFPalette.loopColor
            }
            Shape {
                anchors.left: parent.horizontalCenter
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
                width: 2
                height: parent.height
                color: SFPalette.loopColor
            }
            Shape {
                anchors.right: parent.horizontalCenter
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
        primaryIndicator.anchors.topMargin: 20
        secondaryIndicator.anchors.topMargin: 20

        PlayheadIndicator {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4
            x: positionIndicators.primaryIndicator.x
            color: SFPalette.playheadPrimaryColor
        }

        PlayheadIndicator {
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 4
            x: positionIndicators.secondaryIndicator.x
            color: SFPalette.playheadSecondaryColor
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
