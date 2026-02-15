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
    property Component thumbnailComponent: null

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
                y: trackListManipulator.map[index] ?? 0
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
        verticalManipulator: trackListManipulator
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
    DrawDragHandler {
        id: drawDragHandler
        controller: clipPane.clipPaneInteractionController
        selectionController: clipPane.selectionController
        target: clipPane
        timeManipulator: timeManipulator
        verticalManipulator: trackListManipulator
        viewportContainer: viewportContainer
        onCreateViewModelRequested: (position, trackIndex) => {
            if (trackIndex >= clipPane.trackListViewModel.count)
                return
            viewModel = clipPane.clipPaneInteractionController.createAndInsertClipOnDrawing(clipPane.clipSequenceViewModel, position, trackIndex)
            viewModel.length = clipPane.timeLayoutViewModel.positionAlignment
        }
        onUpdateViewModelRequested: (length) => {
            viewModel.length = length
        }
    }
    GenericComboSceneMouseArea {
        controller: clipPane.clipPaneInteractionController
        selectionController: clipPane.selectionController
        target: clipPane
        timeManipulator: timeManipulator
        verticalManipulator: trackListManipulator
        dispatchMap: ({
            [ClipPaneInteractionController.RubberBandSelect]: rubberBandDragHandler,
            [ClipPaneInteractionController.TimeRangeSelect]: timeRangeDragHandler,
            [ClipPaneInteractionController.Draw]: drawDragHandler
        })
    }
    Connections {
        target: clipPane.selectionController
        function onCurrentItemChanged() {
            let oldItem = slicer.itemForModel(slicer.lastCurrentItem)
            if (oldItem) {
                oldItem.current = false
            }
            let newItem = slicer.itemForModel(clipPane.selectionController.currentItem)
            if (newItem) {
                newItem.current = true
            }
            slicer.lastCurrentItem = clipPane.selectionController.currentItem
        }
    }
    TimeViewportContainer {
        id: viewportContainer

        y: -(clipPane.trackListLayoutViewModel?.viewportOffset ?? 0)
        height: trackListManipulator.viewportHeight
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
                property ClipViewModel lastCurrentItem: null

                delegate: ClipPaneDelegate {
                    id: clipPaneDelegate
                    selectionController: clipPane.selectionController
                    thumbnailComponent: clipPane.thumbnailComponent
                    onClipViewModelChanged: () => {
                        current = (clipPane.selectionController?.currentItem === clipViewModel)
                    }
                    Binding {
                        clipPaneDelegate.x: (clipPaneDelegate.clipViewModel?.position ?? 0) * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.y: trackListManipulator.map[clipPaneDelegate.clipViewModel?.trackIndex ?? 0] ?? 0
                        clipPaneDelegate.width: (clipPaneDelegate.clipViewModel?.length ?? 0) * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.height: clipPane.trackListViewModel?.items[clipPaneDelegate.clipViewModel?.trackIndex ?? 0]?.rowHeight ?? 0
                        clipPaneDelegate.headerMargin: ((clipPane.timeViewModel?.start ?? 0) - (clipPaneDelegate.clipViewModel?.position ?? 0)) * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.color: clipPane.trackListViewModel?.items[clipPaneDelegate.clipViewModel?.trackIndex ?? 0]?.color ?? Qt.rgba(0, 0, 0, 0)
                        when: clipPaneDelegate.SequenceSlicerLoader.inRange
                    }
                    RubberBandItemConnections {
                        target: clipPaneDelegate
                        viewModel: clipPaneDelegate.clipViewModel
                        rubberBandLayer: rubberBandLayer
                    }
                    component ClipMoveDragHandler: MoveDragHandler {
                        controller: clipPane.clipPaneInteractionController
                        selectionController: clipPane.selectionController
                        paneItem: clipPane
                        viewModel: clipPaneDelegate.clipViewModel
                        timeManipulator: timeManipulator
                        verticalManipulator: trackListManipulator
                        onMoveSelectionToYRequested: y => {
                            let trackCount = clipPane.trackListViewModel.items.length;
                            let i = Math.max(0, Math.min(trackListManipulator.mapToPosition(y), trackCount - 1))
                            let d = (y - trackListManipulator.mapToY(i)) / clipPane.trackListViewModel.items[i].rowHeight
                            let targetIndex = Math.round(i + d);
                            if (targetIndex !== viewModel.trackIndex) {
                                let deltaIndex = targetIndex - viewModel.trackIndex;
                                let selection = selectionController.getSelectedItems()
                                for (let clip of selection) {
                                    if (clip.trackIndex + deltaIndex < 0 || clip.trackIndex + deltaIndex >= trackCount)
                                        return;
                                }
                                for (let clip of selection) {
                                    clip.trackIndex += deltaIndex;
                                }
                            }
                        }
                    }
                    ClipMoveDragHandler {
                        id: moveDragHandler
                        moveFlag: ClipPaneInteractionController.MF_Move
                    }
                    ClipMoveDragHandler {
                        id: copyAndMoveDragHandler
                        moveFlag: ClipPaneInteractionController.MF_CopyAndMove
                    }
                    SplitDragHandler {
                        id: splitDragHandler
                        controller: clipPane.clipPaneInteractionController
                        selectionController: clipPane.selectionController
                        paneItem: clipPane
                        viewModel: clipPaneDelegate.clipViewModel
                        timeManipulator: timeManipulator
                    }
                    GenericComboItemMouseArea {
                        controller: clipPane.clipPaneInteractionController
                        selectionController: clipPane.selectionController
                        paneItem: clipPane
                        viewModel: clipPaneDelegate.clipViewModel
                        dispatchMap: ({
                            [ClipPaneInteractionController.Move]: moveDragHandler,
                            [ClipPaneInteractionController.CopyAndMove]: copyAndMoveDragHandler,
                            [ClipPaneInteractionController.Split]: splitDragHandler,
                            [ClipPaneInteractionController.RubberBandSelect]: rubberBandDragHandler,
                            [ClipPaneInteractionController.TimeRangeSelect]: timeRangeDragHandler,
                            [ClipPaneInteractionController.Draw]: drawDragHandler
                        })
                    }
                    component ClipEdgeDragHandler: EdgeDragHandler {
                        controller: clipPane.clipPaneInteractionController
                        selectionController: clipPane.selectionController
                        paneItem: clipPane
                        viewModel: clipPaneDelegate.clipViewModel
                        timeManipulator: timeManipulator
                        onUpdateUnitedExtendRequested: () => {
                            let selection = clipPane.selectionController.getSelectedItems()
                            if (selection.length !== 1) {
                                return
                            }
                            let clip = selection[0];
                            if (edge === EdgeDragHandler.LeftEdge) {
                                for (let previousClip = clip;;) {
                                    previousClip = clipPane.clipSequenceViewModel.iSliceable.previousItem(previousClip);
                                    if (!previousClip || previousClip.position + previousClip.length < clip.position) {
                                        break
                                    }
                                    if (previousClip.position + previousClip.length > clip.position) {
                                        continue
                                    }
                                    if (previousClip.trackIndex !== clip.trackIndex) {
                                        continue
                                    }
                                    unitedExtendItem = previousClip
                                    unitedExtendRestrict = previousClip.length
                                }
                            } else if (edge === EdgeDragHandler.RightEdge) {
                                for (let nextClip = clip;;) {
                                    nextClip = clipPane.clipSequenceViewModel.iSliceable.nextItem(nextClip);
                                    if (!nextClip || nextClip.position > clip.position + clip.length) {
                                        break
                                    }
                                    if (nextClip.position < clip.position + clip.length) {
                                        continue
                                    }
                                    if (nextClip.trackIndex !== clip.trackIndex) {
                                        continue
                                    }
                                    unitedExtendItem = nextClip
                                    unitedExtendRestrict = nextClip.length
                                }
                            }
                        }
                    }
                    Item {
                        id: leftDragHandle
                        anchors.left: parent.left
                        height: parent.height
                        width: Math.min(8, parent.width / 3)
                        ClipEdgeDragHandler {
                            id: leftEdgeDragHandler
                            edge: EdgeDragHandler.LeftEdge
                        }
                        DispatcherMouseArea {
                            determineDragHandler: () => leftEdgeDragHandler
                        }
                        HoverHandler {
                            cursorShape: Qt.SizeHorCursor
                        }
                    }
                    Item {
                        id: rightDragHandle
                        anchors.right: parent.right
                        height: parent.height
                        width: Math.min(8, parent.width / 3)
                        ClipEdgeDragHandler {
                            id: rightEdgeDragHandler
                            edge: EdgeDragHandler.RightEdge
                        }
                        DispatcherMouseArea {
                            determineDragHandler: () => rightEdgeDragHandler
                        }
                        HoverHandler {
                            cursorShape: Qt.SizeHorCursor
                        }
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

            onSelectionAboutToEnd: (rect) => {
                slicer.temporarilyLoadForRubberBand(rect.x, rect.width)
            }
        }
    }
    StandardScrollHandler {
        movableOrientation: Qt.Horizontal | Qt.Vertical
        zoomableOrientation: Qt.Horizontal
        viewModel: clipPane.scrollBehaviorViewModel
        horizontalScrollBarEnabled: true
        horizontalScrollX: -viewportContainer.x
        horizontalScrollWidth: viewportContainer.width
        verticalScrollBarEnabled: true
        verticalScrollY: -viewportContainer.y
        verticalScrollHeight: viewportContainer.height

        onMoved: (x, y, isPhysicalWheel) => {
            timeManipulator.moveViewBy(x, isPhysicalWheel);
            trackListManipulator.moveViewBy(y, isPhysicalWheel);
        }
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => {
            timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel);
        }
    }

}