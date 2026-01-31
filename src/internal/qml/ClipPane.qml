pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: clipPane

    property RangeSequenceViewModel clipSequenceViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property ListViewModel trackListViewModel: null
    property TrackListLayoutViewModel trackListLayoutViewModel: null
    property ClipPaneInteractionController clipPaneInteractionController: null
    property SelectionController selectionController: null

    clip: true

    TimeManipulator {
        id: timeManipulator

        target: clipPane
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        timeViewModel: clipPane.timeViewModel
    }
    TrackListManipulator {
        id: trackListManipulator

        target: clipPane
        trackListLayoutViewModel: clipPane.trackListLayoutViewModel
        trackListViewModel: clipPane.trackListViewModel
    }
    Rectangle {
        id: background

        anchors.fill: parent
        color: SFPalette.editAreaPrimaryColor
    }
    PianoRollScale {
        anchors.fill: parent
        barScaleColor: SFPalette.scalePrimaryColor
        beatScaleColor: SFPalette.scaleSecondaryColor
        segmentScaleColor: SFPalette.scaleTertiaryColor
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        timeViewModel: clipPane.timeViewModel
    }
    Item {
        id: trackSplitters

        anchors.left: parent.left
        anchors.right: parent.right
        y: -(clipPane.trackListLayoutViewModel?.viewportOffset ?? 0)

        Repeater {
            id: trackSpaceRepeater

            model: (clipPane.trackListViewModel?.count ?? 0) + 1

            Item {
                required property int index
                readonly property QtObject trackViewModel: index !== (clipPane.trackListViewModel?.count ?? 0) ? clipPane.trackListViewModel.items[index] : null

                width: parent.width
                height: trackViewModel?.rowHeight ?? 0
                y: trackListManipulator.map[index]
                Rectangle {
                    width: parent.width
                    height: 2
                    anchors.verticalCenter: parent.top
                    color: SFPalette.scaleTertiaryColor
                }
            }
        }
    }
    component ClipPaneRubberBandDragHandler: RubberBandDragHandler {
        controller: clipPane.clipPaneInteractionController
        selectionController: clipPane.selectionController
        target: clipPane
        timeManipulator: timeManipulator
        rubberBandLayer: rubberBandLayer
    }
    ClipPaneRubberBandDragHandler {
        id: rubberBandDragHandler
        mode: RubberBandDragHandler.Normal
    }
    ClipPaneRubberBandDragHandler {
        id: timeRangeDragHandler
        mode: RubberBandDragHandler.TimeRange
    }
    GenericComboSceneMouseArea {
        controller: clipPane.clipPaneInteractionController
        selectionController: clipPane.selectionController
        target: clipPane
        timeManipulator: timeManipulator
        verticalManipulator: trackListManipulator
        dispatchMap: ({
            [ClipPaneInteractionController.RubberBandSelect]: rubberBandDragHandler,
            [ClipPaneInteractionController.TimeRangeSelect]: timeRangeDragHandler
        })
    }
    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: clipPane.timeViewModel
        timeLayoutViewModel: clipPane.timeLayoutViewModel

        Item {
            anchors.fill: parent
            SequenceSlicer {
                id: slicer
                leftOutBound: 256
                viewModel: clipPane.clipSequenceViewModel
                sliceWidth: clipPane.width
                timeLayoutViewModel: clipPane.timeLayoutViewModel
                timeViewModel: clipPane.timeViewModel

                delegate: ClipPaneDelegate {
                    id: clipPaneDelegate
                    selectionController: clipPane.selectionController
                    Binding {
                        clipPaneDelegate.x: clipPaneDelegate.clipViewModel.position * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.y: trackListManipulator.map[clipPaneDelegate.clipViewModel?.trackIndex ?? 0] ?? 0
                        clipPaneDelegate.width: clipPaneDelegate.clipViewModel.length * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.height: clipPane.trackListViewModel?.items[clipPaneDelegate.clipViewModel?.trackIndex ?? 0]?.rowHeight ?? 0
                        when: clipPaneDelegate.SequenceSlicerLoader.inRange
                    }
                    RubberBandItemConnections {
                        target: clipPaneDelegate
                        viewModel: clipPaneDelegate.clipViewModel
                        rubberBandLayer: rubberBandLayer
                    }
                    component LabelMoveDragHandler: MoveDragHandler {
                        controller: clipPane.clipPaneInteractionController
                        selectionController: clipPane.selectionController
                        paneItem: clipPane
                        viewModel: clipPaneDelegate.clipViewModel
                        timeManipulator: timeManipulator
                        verticalManipulator: trackListManipulator
                    }
                    LabelMoveDragHandler {
                        id: moveDragHandler
                        moveFlag: ClipPaneInteractionController.MF_Move
                    }
                    LabelMoveDragHandler {
                        id: copyAndMoveDragHandler
                        moveFlag: ClipPaneInteractionController.MF_CopyAndMove
                    }
                    GenericComboItemMouseArea {
                        controller: clipPane.clipPaneInteractionController
                        selectionController: clipPane.selectionController
                        paneItem: clipPane
                        viewModel: clipPaneDelegate.clipViewModel
                        dispatchMap: ({
                            [ClipPaneInteractionController.Move]: moveDragHandler,
                            [ClipPaneInteractionController.CopyAndMove]: copyAndMoveDragHandler,
                            [ClipPaneInteractionController.RubberBandSelect]: rubberBandDragHandler,
                            [ClipPaneInteractionController.TimeRangeSelect]: timeRangeDragHandler
                        })
                    }
                }
            }
        }
        RubberBandLayer {
            id: rubberBandLayer

            anchors.fill: parent
            selectionController: clipPane.selectionController
            z: 2

            rubberBand: RubberBandRectangle {
            }
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Horizontal | Qt.Vertical
        zoomableOrientation: Qt.Horizontal
        viewModel: clipPane.scrollBehaviorViewModel

        onMoved: (x, y, isPhysicalWheel) => {
            timeManipulator.moveViewBy(x, isPhysicalWheel);
            trackListManipulator.moveViewBy(y, isPhysicalWheel);
        }
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => {
            timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel);
        }
    }

}