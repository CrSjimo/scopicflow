pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: noteEditLayer

    property RangeSequenceViewModel noteSequenceViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property ClavierViewModel clavierViewModel: null
    property NoteEditLayerInteractionController noteEditLayerInteractionController: null
    property SelectionController selectionController: null
    property ClipViewModel clipViewModel: null
    property ListViewModel trackListViewModel: null

    property bool thumbnailDisplay: false
    property bool active: true
    property bool clipBoundaryVisible: false

    clip: true
    enabled: !thumbnailDisplay

    function editLyricInPlace(noteViewModel) {
        lyricEditPopup.close()
        if (noteViewModel) {
            timeManipulator.ensureVisible(noteViewModel.position, noteViewModel.length, 0, 0)
            clavierManipulator.ensureVisible(noteViewModel.key, 1, 0, 0)
            lyricEditPopup.model = noteViewModel
            noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, noteViewModel, NoteEditLayerInteractionController.StartEditing)
            lyricEditPopup.open()
        }
    }

    function editAdditionalTextInPlace(noteViewModel) {
        additionalTextEditPopup.close()
        if (noteViewModel) {
            timeManipulator.ensureVisible(noteViewModel.position, noteViewModel.length, 0, 0)
            clavierManipulator.ensureVisible(noteViewModel.key, 1, 0, 0)
            additionalTextEditPopup.model = noteViewModel
            noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, noteViewModel, NoteEditLayerInteractionController.StartEditing)
            additionalTextEditPopup.open()
        }
    }

    ClipMappedProxyTimeViewModel {
        id: proxyTimeViewModel
        timeViewModel: noteEditLayer.timeViewModel
        clipViewModel: noteEditLayer.clipViewModel
    }

    TimeManipulator {
        id: timeManipulator

        target: noteEditLayer
        timeLayoutViewModel: noteEditLayer.timeLayoutViewModel
        timeViewModel: proxyTimeViewModel
        startOffset: (noteEditLayer.clipViewModel?.clipStart ?? 0) - (noteEditLayer.clipViewModel?.position ?? 0)
    }
    ClavierManipulator {
        id: clavierManipulator

        target: noteEditLayer
        clavierViewModel: noteEditLayer.clavierViewModel
        viewSize: noteEditLayer.height
    }
    component NoteRubberBandDragHandler: RubberBandDragHandler {
        controller: noteEditLayer.noteEditLayerInteractionController
        selectionController: noteEditLayer.selectionController
        target: noteEditLayer
        timeManipulator: timeManipulator
        verticalManipulator: clavierManipulator
        rubberBandLayer: rubberBandLayer
    }
    NoteRubberBandDragHandler {
        id: rubberBandDragHandler
        mode: RubberBandDragHandler.Normal
    }
    NoteRubberBandDragHandler {
        id: timeRangeDragHandler
        mode: RubberBandDragHandler.TimeRange
    }
    DrawDragHandler {
        id: drawDragHandler
        controller: noteEditLayer.noteEditLayerInteractionController
        selectionController: noteEditLayer.selectionController
        target: noteEditLayer
        timeManipulator: timeManipulator
        verticalManipulator: clavierManipulator
        viewportContainer: viewportContainer
        onCreateViewModelRequested: (position, keyIndex) => {
            if (!noteEditLayer.noteSequenceViewModel)
                return
            viewModel = noteEditLayer.noteEditLayerInteractionController?.createAndInsertNoteOnDrawing(noteEditLayer.noteSequenceViewModel, position, keyIndex)
            if (viewModel) {
                viewModel.length = noteEditLayer.timeLayoutViewModel?.positionAlignment ?? viewModel.length
                viewModel.key = keyIndex
            }
        }
        onUpdateViewModelRequested: (length, keyIndex) => {
            if (!viewModel)
                return
            viewModel.length = length
            viewModel.key = keyIndex
        }
    }
    GenericComboSceneMouseArea {
        controller: noteEditLayer.noteEditLayerInteractionController
        selectionController: noteEditLayer.selectionController
        target: noteEditLayer
        timeManipulator: timeManipulator
        verticalManipulator: clavierManipulator
        dispatchMap: ({
            [NoteEditLayerInteractionController.RubberBandSelect]: rubberBandDragHandler,
            [NoteEditLayerInteractionController.TimeRangeSelect]: timeRangeDragHandler,
            [NoteEditLayerInteractionController.Draw]: drawDragHandler,
        })
    }
    Connections {
        target: noteEditLayer.selectionController
        function onCurrentItemChanged() {
            let oldItem = slicer.itemForModel(slicer.lastCurrentItem)
            if (oldItem) {
                oldItem.current = false
            }
            let newItem = slicer.itemForModel(noteEditLayer.selectionController.currentItem)
            if (newItem) {
                newItem.current = true
            }
            slicer.lastCurrentItem = noteEditLayer.selectionController.currentItem
        }
    }
    TimeViewportContainer {
        timeViewModel: noteEditLayer.timeViewModel
        timeLayoutViewModel: noteEditLayer.timeLayoutViewModel
        Rectangle {
            visible: noteEditLayer.clipBoundaryVisible
            height: parent.height
            x: (noteEditLayer.clipViewModel?.position ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            width: (noteEditLayer.clipViewModel?.length ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            color: "transparent"
            border.color: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0].color ?? Theme.accentColor
            border.width: 2
        }
    }
    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: proxyTimeViewModel
        timeLayoutViewModel: noteEditLayer.timeLayoutViewModel

        ClavierViewportContainer {
            id: clavierViewportContainer

            clavierViewModel: noteEditLayer.clavierViewModel
            Item {
                anchors.fill: parent
                SequenceSlicer {
                    id: additionalTextSlicer
                    leftOutBound: 256
                    viewModel: noteEditLayer.noteSequenceViewModel
                    sliceWidth: noteEditLayer.width
                    timeLayoutViewModel: noteEditLayer.timeLayoutViewModel
                    timeViewModel: proxyTimeViewModel
                    active: noteEditLayer.active
                    delegate: NoteEditLayerAdditionalTextDelegate {
                        id: noteDelegate
                        Binding {
                            noteDelegate.x: (noteDelegate.noteViewModel?.position ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                            noteDelegate.y: (127 - (noteDelegate.noteViewModel?.key ?? 0)) * (noteEditLayer.clavierViewModel?.pixelDensity ?? 0) + (noteEditLayer.noteEditLayerInteractionController?.additionalTextPosition === NoteEditLayerInteractionController.AdditionalTextPosition_Up ? -noteDelegate.height : (noteEditLayer.clavierViewModel?.pixelDensity ?? 0))
                            noteDelegate.width: (noteDelegate.noteViewModel?.length ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                            noteDelegate.color: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0].color ?? Theme.accentColor
                            noteDelegate.visible: !noteEditLayer.thumbnailDisplay
                            when: noteDelegate.SequenceSlicerLoader.inRange
                        }
                    }
                }
            }
            Item {
                anchors.fill: parent
                SequenceSlicer {
                    id: slicer
                    viewModel: noteEditLayer.noteSequenceViewModel
                    sliceWidth: noteEditLayer.width
                    timeLayoutViewModel: noteEditLayer.timeLayoutViewModel
                    timeViewModel: proxyTimeViewModel
                    active: noteEditLayer.active
                    property NoteViewModel lastCurrentItem: null

                    delegate: NoteEditLayerDelegate {
                        id: noteDelegate

                        selectionController: noteEditLayer.selectionController

                        onNoteViewModelChanged: () => {
                            current = (noteEditLayer.selectionController?.currentItem === noteViewModel)
                        }

                        Binding {
                            noteDelegate.x: (noteDelegate.noteViewModel?.position ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                            noteDelegate.y: (127 - (noteDelegate.noteViewModel?.key ?? 0)) * (noteEditLayer.clavierViewModel?.pixelDensity ?? 0)
                            noteDelegate.width: (noteDelegate.noteViewModel?.length ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                            noteDelegate.height: noteEditLayer.clavierViewModel?.pixelDensity ?? 0
                            noteDelegate.color: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0].color ?? Theme.accentColor
                            noteDelegate.outOfClipRange: {
                                if (!noteDelegate.noteViewModel || !noteEditLayer.clipViewModel) {
                                    return false
                                }
                                let p = noteDelegate.noteViewModel.position
                                let l = noteDelegate.noteViewModel.length
                                let cs = noteEditLayer.clipViewModel.clipStart
                                let cl = noteEditLayer.clipViewModel.length
                                return p + l <= cs || p >= cs + cl
                            }
                            noteDelegate.thumbnailDisplay: noteEditLayer.thumbnailDisplay
                            when: noteDelegate.SequenceSlicerLoader.inRange
                        }
                        RubberBandItemConnections {
                            target: noteDelegate
                            viewModel: noteDelegate.noteViewModel
                            rubberBandLayer: rubberBandLayer
                        }
                        component NoteMoveDragHandler: MoveDragHandler {
                            controller: noteEditLayer.noteEditLayerInteractionController
                            selectionController: noteEditLayer.selectionController
                            paneItem: noteEditLayer
                            viewModel: noteDelegate.noteViewModel
                            timeManipulator: timeManipulator
                            verticalManipulator: clavierManipulator

                            onMoveSelectionToYRequested: y => {
                                let targetKey = Math.round(clavierManipulator.mapToPosition(y)) - 1
                                let currentKey = noteDelegate.noteViewModel.key
                                let deltaKey = targetKey - currentKey
                                if (deltaKey === 0)
                                    return
                                let selection = noteEditLayer.selectionController.getSelectedItems()
                                for (let note of selection) {
                                    let candidate = note.key + deltaKey
                                    if (candidate < 0 || candidate > 127)
                                        return
                                }
                                for (let note of selection) {
                                    note.key += deltaKey
                                }
                            }
                        }
                        NoteMoveDragHandler {
                            id: moveDragHandler
                            moveFlag: NoteEditLayerInteractionController.MF_Move
                        }
                        NoteMoveDragHandler {
                            id: copyAndMoveDragHandler
                            moveFlag: NoteEditLayerInteractionController.MF_CopyAndMove
                        }
                        SplitDragHandler {
                            id: splitDragHandler
                            controller: noteEditLayer.noteEditLayerInteractionController
                            selectionController: noteEditLayer.selectionController
                            paneItem: noteEditLayer
                            viewModel: noteDelegate.noteViewModel
                            timeManipulator: timeManipulator
                        }
                        component NoteEdgeDragHandler: EdgeDragHandler {
                            controller: noteEditLayer.noteEditLayerInteractionController
                            selectionController: noteEditLayer.selectionController
                            paneItem: noteEditLayer
                            viewModel: noteDelegate.noteViewModel
                            timeManipulator: timeManipulator
                        }
                        GenericComboItemMouseArea {
                            controller: noteEditLayer.noteEditLayerInteractionController
                            selectionController: noteEditLayer.selectionController
                            paneItem: noteEditLayer
                            viewModel: noteDelegate.noteViewModel
                            dispatchMap: ({
                                [NoteEditLayerInteractionController.Move]: moveDragHandler,
                                [NoteEditLayerInteractionController.CopyAndMove]: copyAndMoveDragHandler,
                                [NoteEditLayerInteractionController.Split]: splitDragHandler,
                                [NoteEditLayerInteractionController.RubberBandSelect]: rubberBandDragHandler,
                                [NoteEditLayerInteractionController.TimeRangeSelect]: timeRangeDragHandler,
                                [NoteEditLayerInteractionController.Draw]: drawDragHandler,
                            })
                        }
                        Item {
                            id: leftDragHandle
                            anchors.left: parent.left
                            height: noteDelegate.height
                            width: Math.min(8, parent.width / 3)
                            NoteEdgeDragHandler {
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
                            height: noteDelegate.height
                            width: Math.min(8, parent.width / 3)
                            NoteEdgeDragHandler {
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
                selectionController: noteEditLayer.selectionController
                z: 2

                rubberBand: RubberBandRectangle {
                }

                onSelectionAboutToEnd: (rect) => {
                    slicer.temporarilyLoadForRubberBand(rect.x, rect.width)
                }
            }
            ItemPopupEdit {
                id: lyricEditPopup

                readonly property Item associatedItem: slicer.itemForModel(model)

                containerModel: noteEditLayer.noteSequenceViewModel
                targetProperty: "lyric"
                radius: 2
                width: associatedItem?.width ?? 0
                height: associatedItem?.height ?? 0
                x: associatedItem?.x ?? 0
                y: associatedItem?.y ?? 0

                onEditPreviousRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MovePrevious)
                onEditNextRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveNext)
                onEditHomeRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveHome)
                onEditEndRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveEnd)
                onAccepted: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.CommitEditing)
                onRejected: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.AbortEditing)
                onVisibleChanged: () => {
                    if (associatedItem)
                        associatedItem.lyricEditing = visible
                }
            }
            ItemPopupEdit {
                id: additionalTextEditPopup

                readonly property Item associatedItem: additionalTextSlicer.itemForModel(model)

                containerModel: noteEditLayer.noteSequenceViewModel
                targetProperty: "additionalText"
                radius: 2
                width: associatedItem?.width ?? 0
                height: associatedItem?.height ?? 0
                x: associatedItem?.x ?? 0
                y: associatedItem?.y ?? 0

                onEditPreviousRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MovePrevious)
                onEditNextRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveNext)
                onEditHomeRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveHome)
                onEditEndRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveEnd)
                onAccepted: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.CommitEditing)
                onRejected: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.AbortEditing)
                onVisibleChanged: () => {
                    if (associatedItem)
                        associatedItem.additionalTextEditing = visible
                }
            }
        }
    }
}
