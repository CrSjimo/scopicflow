import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Shapes

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

T.Pane {
    id: timeline

    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject interactionControllerNotifier: null

    Accessible.name: qsTr("Timeline")
    focus: true
    focusPolicy: Qt.StrongFocus
    clip: true
    implicitHeight: 24
    background: Rectangle { color: SFPalette.timelineBackgroundColor }

    component PlayheadIndicator: Item {
        id: indicator
        LayoutMirroring.enabled: false
        LayoutMirroring.childrenInherit: true
        implicitHeight: 14.333333333333334 // 43 / 3
        implicitWidth: 18.475208614068027 // 32 / sqrt(3)
        property color color

        Shape {
            id: shape
            width: parent.width
            height: parent.height
            anchors.horizontalCenter: parent.left
            ShapePath {
                id: indicatorPath

                fillColor: indicator.color
                strokeWidth: 0

                PathLine {
                    x: shape.width * 0.25
                    y: 0
                }
                PathLine {
                    x: shape.width * 0.75
                    y: 0
                }
                PathArc {
                    radiusX: 4 / 3
                    radiusY: 4 / 3
                    x: shape.width * 0.875
                    y: 4
                }
                PathLine {
                    x: shape.width * 0.625
                    y: 12
                }
                PathArc {
                    radiusX: 4 / 3
                    radiusY: 4 / 3
                    x: shape.width * 0.375
                    y: 12
                }
                PathLine {
                    x: shape.width * 0.125
                    y: 4
                }
                PathArc {
                    radiusX: 4 / 3
                    radiusY: 4 / 3
                    x: shape.width * 0.25
                    y: 0
                }
            }
        }

    }

    // helpers
    QtObject {
        id: d
        readonly property double cursorIndicatorX: timeManipulator.mapToPosition(playbackViewModel?.cursorPosition ?? -1)
        readonly property double primaryIndicatorX: timeManipulator.mapToPosition(playbackViewModel?.primaryPosition ?? 0)
        readonly property double secondaryIndicatorX: timeManipulator.mapToPosition(playbackViewModel?.secondaryPosition ?? 0)

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
    }
    TimeManipulator {
        id: timeManipulator
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }
    
    // visual components
    TimelineScale {
        id: timelineScale
        anchors.fill: parent
        color: SFPalette.suitableForegroundColor(SFPalette.timelineBackgroundColor)
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }
    Item {
        id: viewportContainer

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
    PlayheadIndicator {
        id: secondaryIndicator
        anchors.bottom: parent.bottom
        x: d.secondaryIndicatorX
        color: SFPalette.playheadSecondaryColor
    }
    PlayheadIndicator {
        id: primaryIndicator
        anchors.bottom: parent.bottom
        x: d.primaryIndicatorX
        color: SFPalette.playheadPrimaryColor
    }
    Rectangle {
        id: cursorIndicator
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: SFPalette.cursorIndicatorColor
        visible: d.cursorIndicatorX >= 0
        width: 1
        x: d.cursorIndicatorX - 0.5
    }
    MouseArea {
        id: mouseArea

        property double pressedX: 0
        property bool rejectContextMenu: false
        function handlePositionChanged(x, button) {
            if (button === Qt.LeftButton) {
                d.setIndicatorPosition(x);
            } else {
                rejectContextMenu = true;
                if (!rubberBandLayer.started) {
                    rubberBandLayer.startSelection(mapToItem(viewportContainer, timeManipulator.mapToPosition(timeManipulator.alignTick(timeManipulator.mapToTick(pressedX), ScopicFlow.AO_Visible)), 0));
                }
                rubberBandLayer.updateSelection(mapToItem(viewportContainer, timeManipulator.mapToPosition(timeManipulator.alignTick(timeManipulator.mapToTick(x), ScopicFlow.AO_Visible)), timeline.height));
            }
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
        }
        onClicked: mouse => {
            if (mouse.button === Qt.LeftButton) {
                d.setIndicatorPosition(mouse.x);
            } else if (mouse.button === Qt.RightButton && !rejectContextMenu) {

            }
        }
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
        }
        onReleased: () => {
            dragScroller.running = false;
            cursorShape = Qt.ArrowCursor;
            let rect = mapFromItem(viewportContainer, rubberBandLayer.endSelection());
            d.setZoomedRange(rect.x, rect.width);
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
    
    // scroll
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
    MiddleButtonMoveHandler {
        anchors.fill: parent
        direction: Qt.Horizontal
        viewModel: timeline.scrollBehaviorViewModel

        onMoved: x => {
            timeManipulator.moveViewBy(x);
        }
    }
}
