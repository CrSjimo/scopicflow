import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Shapes

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

T.Pane {
    id: timeline

    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property PlaybackViewModel playbackViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null

    Accessible.name: qsTr("Timeline")
    focus: true
    focusPolicy: Qt.StrongFocus
    clip: true
    implicitHeight: 24
    background: Rectangle {
        color: Theme.backgroundColor(timeline.ThemedItem.backgroundLevel)
    }

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

    // Helpers
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
        parent: timeline
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }
    
    // Visual components
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

    // Left button
    MouseArea {
        id: leftButtonMouseArea

        property double pressedX: 0
        property bool rejectContextMenu: false

        acceptedButtons: Qt.LeftButton
        anchors.fill: parent

        onCanceled: () => {
            dragScroller.running = false;
            cursorShape = undefined;

        }
        onClicked: mouse => {
            d.setIndicatorPosition(mouse.x);
        }
        onPositionChanged: mouse => {
            if (!pressed)
                return;
            dragScroller.determine(mouse.x, timeline.width, 0, 0, triggered => {
                if (triggered)
                    return;
                d.setIndicatorPosition(mouse.x);
            });
        }
        onPressed: mouse => {
            pressedX = mouse.x;
        }
        onReleased: canceled()

        DragScroller {
            id: dragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX);
                d.setIndicatorPosition(deltaX < 0 ? 0 : timeline.width);
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

}
