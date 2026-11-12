pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: clipEditLayer

    property RangeSequenceViewModel clipSequenceViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property ListViewModel trackListViewModel: null
    property TrackListLayoutViewModel trackListLayoutViewModel: null
    property ClipPaneInteractionController clipPaneInteractionController: null
    property SelectionController selectionController: null
    property Component thumbnailComponent: null
    property Item selectionOverlayParent: null

    onClipSequenceViewModelChanged: scenePointerInput.router.cancel()

    clip: true

    TimeManipulator {
        id: timeManipulator

        target: clipEditLayer
        timeLayoutViewModel: clipEditLayer.timeLayoutViewModel
        timeViewModel: clipEditLayer.timeViewModel
    }
    TrackListManipulator {
        id: trackListManipulator

        target: clipEditLayer
        trackListLayoutViewModel: clipEditLayer.trackListLayoutViewModel
        trackListViewModel: clipEditLayer.trackListViewModel
    }
    component ClipPaneRubberBandDragHandler: RubberBandDragHandler {
        controller: clipEditLayer.clipPaneInteractionController
        selectionController: clipEditLayer.selectionController
        target: clipEditLayer
        timeManipulator: timeManipulator
        verticalManipulator: trackListManipulator
        rubberBandLayer: rubberBandLayer
        selector: rubberBandSelector
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
        controller: clipEditLayer.clipPaneInteractionController
        selectionController: clipEditLayer.selectionController
        target: clipEditLayer
        timeManipulator: timeManipulator
        verticalManipulator: trackListManipulator
        onCreateViewModelRequested: (position, trackIndex) => {
            if (trackIndex >= clipEditLayer.trackListViewModel.count)
                return
            viewModel = clipEditLayer.clipPaneInteractionController.createAndInsertClipOnDrawing(clipEditLayer.clipSequenceViewModel, position, trackIndex)
            viewModel.length = clipEditLayer.timeLayoutViewModel.positionAlignment
        }
        onUpdateViewModelRequested: (length) => {
            viewModel.length = length
        }
    }
    component ClipMoveDragHandler: MoveDragHandler {
        controller: clipEditLayer.clipPaneInteractionController
        selectionController: clipEditLayer.selectionController
        paneItem: clipEditLayer
        timeManipulator: timeManipulator
        verticalManipulator: trackListManipulator

        onMoveSelectionToYRequested: (y, viewModel) => {
            const trackCount = clipEditLayer.trackListViewModel.items.length
            const index = Math.max(0, Math.min(
                trackListManipulator.mapToPosition(y), trackCount - 1))
            const offset = (y - trackListManipulator.mapToY(index))
                / clipEditLayer.trackListViewModel.items[index].rowHeight
            const targetIndex = Math.round(index + offset)
            if (targetIndex === viewModel.trackIndex)
                return
            const deltaIndex = targetIndex - viewModel.trackIndex
            const selection = clipEditLayer.selectionController.getSelectedItems()
            for (const clip of selection) {
                if (clip.trackIndex + deltaIndex < 0
                        || clip.trackIndex + deltaIndex >= trackCount) {
                    return
                }
            }
            for (const clip of selection)
                clip.trackIndex += deltaIndex
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
        controller: clipEditLayer.clipPaneInteractionController
        selectionController: clipEditLayer.selectionController
        paneItem: clipEditLayer
        timeManipulator: timeManipulator
    }
    component ClipEdgeDragHandler: EdgeDragHandler {
        controller: clipEditLayer.clipPaneInteractionController
        selectionController: clipEditLayer.selectionController
        paneItem: clipEditLayer
        timeManipulator: timeManipulator

        onUpdateUnitedExtendRequested: {
            const selection = clipEditLayer.selectionController.getSelectedItems()
            if (selection.length !== 1)
                return
            const clip = selection[0]
            if (edge === EdgeDragHandler.LeftEdge) {
                for (let previousClip = clip;;) {
                    previousClip = clipEditLayer.clipSequenceViewModel.iSliceable.previousItem(previousClip)
                    if (!previousClip
                            || previousClip.position + previousClip.length < clip.position) {
                        break
                    }
                    if (previousClip.position + previousClip.length > clip.position
                            || previousClip.trackIndex !== clip.trackIndex) {
                        continue
                    }
                    unitedExtendItem = previousClip
                    unitedExtendRestrict = previousClip.length
                }
            } else {
                for (let nextClip = clip;;) {
                    nextClip = clipEditLayer.clipSequenceViewModel.iSliceable.nextItem(nextClip)
                    if (!nextClip || nextClip.position > clip.position + clip.length)
                        break
                    if (nextClip.position < clip.position + clip.length
                            || nextClip.trackIndex !== clip.trackIndex) {
                        continue
                    }
                    unitedExtendItem = nextClip
                    unitedExtendRestrict = nextClip.length
                }
            }
        }
    }
    ClipEdgeDragHandler {
        id: leftEdgeDragHandler
        edge: EdgeDragHandler.LeftEdge
    }
    ClipEdgeDragHandler {
        id: rightEdgeDragHandler
        edge: EdgeDragHandler.RightEdge
    }
    ScenePointerInput {
        id: scenePointerInput
        controller: clipEditLayer.clipPaneInteractionController
        selectionController: clipEditLayer.selectionController
        paneItem: clipEditLayer
        coordinateSpace: clipEditLayer
        timeManipulator: timeManipulator
        verticalManipulator: trackListManipulator
        dispatchMap: ({
            [ClipPaneInteractionController.Move]: moveDragHandler,
            [ClipPaneInteractionController.CopyAndMove]: copyAndMoveDragHandler,
            [ClipPaneInteractionController.Split]: splitDragHandler,
            [ClipPaneInteractionController.RubberBandSelect]: rubberBandDragHandler,
            [ClipPaneInteractionController.TimeRangeSelect]: timeRangeDragHandler,
            [ClipPaneInteractionController.Draw]: drawDragHandler
        })
    }
    Connections {
        target: clipEditLayer.selectionController
        function onCurrentItemChanged() {
            let oldItem = slicer.itemForModel(slicer.lastCurrentItem)
            if (oldItem) {
                oldItem.current = false
            }
            let newItem = slicer.itemForModel(clipEditLayer.selectionController.currentItem)
            if (newItem) {
                newItem.current = true
            }
            slicer.lastCurrentItem = clipEditLayer.selectionController.currentItem
        }
    }
    TimeViewportContainer {
        id: viewportContainer

        y: -(clipEditLayer.trackListLayoutViewModel?.viewportOffset ?? 0)
        height: trackListManipulator.viewportHeight
        timeViewModel: clipEditLayer.timeViewModel
        timeLayoutViewModel: clipEditLayer.timeLayoutViewModel

        Item {
            anchors.fill: parent
            SequenceSlicer {
                id: slicer
                leftOutBound: 256
                viewModel: clipEditLayer.clipSequenceViewModel
                sliceWidth: clipEditLayer.width
                timeLayoutViewModel: clipEditLayer.timeLayoutViewModel
                timeViewModel: clipEditLayer.timeViewModel
                property ClipViewModel lastCurrentItem: null

                delegate: ClipPaneDelegate {
                    id: clipPaneDelegate
                    selectionController: clipEditLayer.selectionController
                    thumbnailComponent: clipEditLayer.thumbnailComponent
                    onClipViewModelChanged: () => {
                        current = (clipEditLayer.selectionController?.currentItem === clipViewModel)
                    }
                    Binding {
                        clipPaneDelegate.x: (clipPaneDelegate.clipViewModel?.position ?? 0) * (clipEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.y: trackListManipulator.map[clipPaneDelegate.clipViewModel?.trackIndex ?? 0] ?? 0
                        clipPaneDelegate.width: (clipPaneDelegate.clipViewModel?.length ?? 0) * (clipEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.height: clipEditLayer.trackListViewModel?.items[clipPaneDelegate.clipViewModel?.trackIndex ?? 0]?.rowHeight ?? 0
                        clipPaneDelegate.headerMargin: ((clipEditLayer.timeViewModel?.start ?? 0) - (clipPaneDelegate.clipViewModel?.position ?? 0)) * (clipEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipPaneDelegate.color: clipEditLayer.trackListViewModel?.items[clipPaneDelegate.clipViewModel?.trackIndex ?? 0]?.color ?? Qt.rgba(0, 0, 0, 0)
                        when: clipPaneDelegate.SequenceSlicerLoader.inRange
                    }
                    RubberBandItemConnections {
                        target: clipPaneDelegate
                        viewModel: clipPaneDelegate.clipViewModel
                        selector: rubberBandSelector
                    }
                    ItemPointerInput {
                        sceneInput: scenePointerInput
                        item: clipPaneDelegate
                        viewModel: clipPaneDelegate.clipViewModel
                        payloadResolver: point => {
                            const edgeWidth = Math.min(8, clipPaneDelegate.width / 3)
                            if (point.x < edgeWidth)
                                return EdgeDragHandler.LeftEdge
                            if (point.x >= clipPaneDelegate.width - edgeWidth)
                                return EdgeDragHandler.RightEdge
                            return -1
                        }
                        itemHandlerResolver: (event, hit) => {
                            if (hit.payload === EdgeDragHandler.LeftEdge)
                                return leftEdgeDragHandler
                            if (hit.payload === EdgeDragHandler.RightEdge)
                                return rightEdgeDragHandler
                            return scenePointerInput.resolveHandler(event, hit)
                        }
                        itemCursorResolver: hit => hit.payload === -1
                            ? undefined : Qt.SizeHorCursor
                    }

                }
            }
        }
        RubberBandLayer {
            id: rubberBandLayer

            parent: clipEditLayer.selectionOverlayParent ?? viewportContainer
            x: parent === viewportContainer ? 0 : viewportContainer.x
            y: parent === viewportContainer ? 0 : viewportContainer.y
            width: viewportContainer.width
            height: viewportContainer.height
            z: 2

            rubberBand: RubberBandRectangle {
            }
        }
        RubberBandSelector {
            id: rubberBandSelector

            selectionController: clipEditLayer.selectionController
            onSelectionAboutToEnd: (rect) => {
                slicer.temporarilyLoadForRubberBand(rect.x, rect.width)
            }
        }
    }
    StandardScrollHandler {
        movableOrientation: Qt.Horizontal | Qt.Vertical
        zoomableOrientation: Qt.Horizontal
        viewModel: clipEditLayer.scrollBehaviorViewModel
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
