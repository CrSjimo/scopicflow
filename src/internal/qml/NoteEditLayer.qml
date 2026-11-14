pragma ComponentBehavior: Bound

import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: noteEditLayer

    property RangeSequenceViewModel noteSequenceViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property ClavierViewModel clavierViewModel: null
    property NoteEditLayerInteractionController noteEditLayerInteractionController: null
    property SelectionController selectionController: null
    property ClipViewModel clipViewModel: null
    property ListViewModel trackListViewModel: null

    property bool thumbnailDisplay: false
    property bool transparentDisplay: false
    property bool active: true
    property bool clipBoundaryVisible: false
    property int bottomExpansion: 0

    onNoteSequenceViewModelChanged: scenePointerInput.router.cancel()
    onClipViewModelChanged: scenePointerInput.router.cancel()

    clip: true
    enabled: !thumbnailDisplay

    function editLyricInPlace(noteViewModel: NoteViewModel) {
        lyricEditPopup.close()
        if (noteViewModel) {
            timeManipulator.ensureVisible(noteViewModel.position, noteViewModel.length, 0, 0)
            clavierManipulator.ensureVisible(noteViewModel.key, 1, 0, 0)
            lyricEditPopup.model = noteViewModel
            noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, noteViewModel, NoteEditLayerInteractionController.StartEditing)
            lyricEditPopup.open()
        }
    }

    function editAdditionalTextInPlace(noteViewModel: NoteViewModel) {
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
        viewSize: noteEditLayer.height - noteEditLayer.bottomExpansion
    }
    component ConfiguredNoteRubberBandDragHandler: NoteRubberBandDragHandler {
        controller: noteEditLayer.noteEditLayerInteractionController
        selectionController: noteEditLayer.selectionController
        target: noteEditLayer
        coordinateSpace: editArea
        timeManipulator: timeManipulator
        verticalManipulator: clavierManipulator
        rubberBandLayer: rubberBandLayer
        selector: rubberBandSelector
        noteItem: noteCanvas
    }
    Item {
        id: editArea
        x: 0
        y: 0
        width: noteEditLayer.width
        height: Math.max(0, noteEditLayer.height - noteEditLayer.bottomExpansion)
        readonly property ClipViewModel clipViewModel: noteEditLayer.clipViewModel
        readonly property RangeSequenceViewModel noteSequenceViewModel: noteEditLayer.noteSequenceViewModel
        ConfiguredNoteRubberBandDragHandler {
            id: rubberBandDragHandler
            mode: RubberBandDragHandler.Normal
        }
        ConfiguredNoteRubberBandDragHandler {
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
            onCreateViewModelRequested: (position, keyIndex) => {
                if (!noteEditLayer.noteSequenceViewModel)
                    return
                viewModel = noteEditLayer.noteEditLayerInteractionController?.createAndInsertNoteOnDrawing(noteEditLayer.noteSequenceViewModel, position, keyIndex)
                if (viewModel) {
                    viewModel.length = noteEditLayer.timeLayoutViewModel?.positionAlignment ?? viewModel.length
                    viewModel.key = Math.max(0, Math.min(keyIndex, 127))
                }
            }
            onUpdateViewModelRequested: (length, keyIndex) => {
                if (!viewModel)
                    return
                viewModel.length = length
                viewModel.key = Math.max(0, Math.min(keyIndex, 127))
            }
        }

        component NoteMoveDragHandler: MoveDragHandler {
            controller: noteEditLayer.noteEditLayerInteractionController
            selectionController: noteEditLayer.selectionController
            paneItem: editArea
            timeManipulator: timeManipulator
            verticalManipulator: clavierManipulator

            onMoveSelectionToYRequested: (y, viewModel) => {
                const targetKey = Math.round(clavierManipulator.mapToPosition(y)) - 1
                const deltaKey = targetKey - viewModel.key
                if (deltaKey === 0)
                    return
                const selection = noteEditLayer.selectionController.getSelectedItems()
                for (const note of selection) {
                    const candidate = note.key + deltaKey
                    if (candidate < 0 || candidate > 127)
                        return
                }
                for (const note of selection)
                    note.key += deltaKey
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
            timeManipulator: timeManipulator
        }
        component NoteEdgeDragHandler: EdgeDragHandler {
            controller: noteEditLayer.noteEditLayerInteractionController
            selectionController: noteEditLayer.selectionController
            paneItem: noteEditLayer
            timeManipulator: timeManipulator

            onUpdateUnitedExtendRequested: {
                const selection = noteEditLayer.selectionController.getSelectedItems()
                if (selection.length !== 1)
                    return
                const note = selection[0]
                if (edge === EdgeDragHandler.LeftEdge) {
                    for (let previousNote = note;;) {
                        previousNote = noteEditLayer.noteSequenceViewModel.iSliceable.previousItem(previousNote)
                        if (!previousNote
                                || previousNote.position + previousNote.length < note.position) {
                            break
                        }
                        if (previousNote.position + previousNote.length > note.position)
                            continue
                        unitedExtendItem = previousNote
                        unitedExtendRestrict = previousNote.length
                    }
                } else {
                    for (let nextNote = note;;) {
                        nextNote = noteEditLayer.noteSequenceViewModel.iSliceable.nextItem(nextNote)
                        if (!nextNote || nextNote.position > note.position + note.length)
                            break
                        if (nextNote.position < note.position + note.length)
                            continue
                        unitedExtendItem = nextNote
                        unitedExtendRestrict = nextNote.length
                    }
                }
            }
        }
        NoteEdgeDragHandler {
            id: leftEdgeDragHandler
            edge: EdgeDragHandler.LeftEdge
        }
        NoteEdgeDragHandler {
            id: rightEdgeDragHandler
            edge: EdgeDragHandler.RightEdge
        }
        ScenePointerInput {
            id: scenePointerInput
            controller: noteEditLayer.noteEditLayerInteractionController
            selectionController: noteEditLayer.selectionController
            paneItem: noteEditLayer
            coordinateSpace: editArea
            timeManipulator: timeManipulator
            verticalManipulator: clavierManipulator
            freezeCursorOnPress: true
            dispatchMap: ({
                [NoteEditLayerInteractionController.Move]: moveDragHandler,
                [NoteEditLayerInteractionController.CopyAndMove]: copyAndMoveDragHandler,
                [NoteEditLayerInteractionController.Split]: splitDragHandler,
                [NoteEditLayerInteractionController.RubberBandSelect]: rubberBandDragHandler,
                [NoteEditLayerInteractionController.TimeRangeSelect]: timeRangeDragHandler,
                [NoteEditLayerInteractionController.Draw]: drawDragHandler,
            })
            clickInterceptor: (event, hit, doubleClick) => {
                if (hit.hoverRegion !== NoteEditLayerItem.AdditionalTextHitRegion)
                    return false
                if (doubleClick && hit.payload) {
                    noteEditLayer.noteEditLayerInteractionController?.itemAdditionalTextDoubleClicked(
                        noteEditLayer, hit.payload)
                }
                return true
            }
        }
    }
    TimeViewportContainer {
        timeViewModel: noteEditLayer.timeViewModel
        timeLayoutViewModel: noteEditLayer.timeLayoutViewModel
        Rectangle {
            visible: noteEditLayer.clipBoundaryVisible
            height: parent.height - noteEditLayer.bottomExpansion
            x: ((noteEditLayer.clipViewModel?.position ?? 0) - (noteEditLayer.clipViewModel?.clipStart ?? 0)) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            width: ((noteEditLayer.clipViewModel?.length ?? 0) + (noteEditLayer.clipViewModel?.clipStart ?? 0)) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            color: "transparent"
            border.color: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0]?.color ?? Theme.accentColor
            border.width: 2
        }
        Rectangle {
            visible: noteEditLayer.clipBoundaryVisible
            height: parent.height - noteEditLayer.bottomExpansion
            x: (noteEditLayer.clipViewModel?.position ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            width: (noteEditLayer.clipViewModel?.length ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            color: "transparent"
            border.color: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0]?.color ?? Theme.accentColor
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
                id: canvasContainer
                anchors.fill: parent

                readonly property color noteFillColor: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0]?.color ?? Theme.accentColor

                NoteEditLayerItem {
                    id: noteCanvas

                    anchors.fill: parent

                    noteSequenceViewModel: noteEditLayer.noteSequenceViewModel
                    timeViewModel: proxyTimeViewModel
                    timeLayoutViewModel: noteEditLayer.timeLayoutViewModel
                    clavierViewModel: noteEditLayer.clavierViewModel
                    selectionController: noteEditLayer.selectionController
                    rubberBandSelector: rubberBandSelector
                    fillColor: canvasContainer.noteFillColor
                    selectedFillColor: SFPalette.noteSelectedColorChange.apply(canvasContainer.noteFillColor)
                    selectedBorderColor: Theme.foregroundPrimaryColor
                    overlappedBorderColor: (noteEditLayer.noteEditLayerInteractionController?.warnOfOverlappingNotes ?? true) ? Theme.warningColor : "transparent"
                    textColor: Theme.foregroundPrimaryColor
                    additionalTextColor: Theme.foregroundSecondaryColor
                    highlightedAdditionalTextColor: canvasContainer.noteFillColor
                    font: Theme.font
                    active: noteEditLayer.active
                    transparentDisplay: noteEditLayer.transparentDisplay
                    thumbnailDisplay: noteEditLayer.thumbnailDisplay
                    editScopeFocused: noteEditLayer.selectionController?.editScopeFocused ?? false
                    lyricEditingItem: lyricEditPopup.visible ? lyricEditPopup.model : null
                    additionalTextEditingItem: additionalTextEditPopup.visible ? additionalTextEditPopup.model : null
                    viewportWidth: noteEditLayer.width
                    viewportHeight: noteEditLayer.height
                    additionalTextAbove: noteEditLayer.noteEditLayerInteractionController?.additionalTextPosition === NoteEditLayerInteractionController.AdditionalTextPosition_Up
                }

                PointerInputArea {
                    anchors.fill: parent
                    router: scenePointerInput.router
                    coordinateSpace: editArea
                    containmentMask: noteCanvas
                    clearHoverOnInvalidHit: true
                    freezeCursorOnPress: true
                    hitResolver: (surfacePoint, localPoint) => noteCanvas.hitTest(localPoint, editArea)
                    handlerResolver: (event, hit) => {
                        if (hit.hoverRegion === NoteEditLayerItem.AdditionalTextHitRegion)
                            return null
                        if (hit.payload === EdgeDragHandler.LeftEdge)
                            return leftEdgeDragHandler
                        if (hit.payload === EdgeDragHandler.RightEdge)
                            return rightEdgeDragHandler
                        return scenePointerInput.resolveHandler(event, hit)
                    }
                    cursorResolver: hit => (!hit.valid
                            || hit.hoverRegion === NoteEditLayerItem.AdditionalTextHitRegion
                            || hit.payload === -1) ? Qt.ArrowCursor : Qt.SizeHorCursor
                    pressCursorResolver: cursorResolver
                }
            }
            RubberBandLayer {
                id: rubberBandLayer

                anchors.fill: parent
                z: 2

                rubberBand: RubberBandRectangle {
                }
            }
            RubberBandSelector {
                id: rubberBandSelector

                selectionController: noteEditLayer.selectionController
                itemHint: RubberBandSelector.SameHeight
            }
            ItemPopupEdit {
                id: lyricEditPopup

                readonly property rect associatedRect: {
                    noteCanvas.geometryRevision
                    return noteCanvas.itemRect(model)
                }

                containerModel: noteEditLayer.noteSequenceViewModel
                targetProperty: "lyric"
                radius: 2
                width: associatedRect.width
                height: associatedRect.height
                x: associatedRect.x
                y: associatedRect.y

                onEditPreviousRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MovePrevious)
                onEditNextRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveNext)
                onEditHomeRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveHome)
                onEditEndRequested: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveEnd)
                onAccepted: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.CommitEditing)
                onRejected: noteEditLayer.noteEditLayerInteractionController?.lyricInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.AbortEditing)
            }
            ItemPopupEdit {
                id: additionalTextEditPopup

                readonly property rect associatedRect: {
                    noteCanvas.geometryRevision
                    return noteCanvas.additionalTextRect(model)
                }

                containerModel: noteEditLayer.noteSequenceViewModel
                targetProperty: "additionalText"
                radius: 2
                width: associatedRect.width
                height: associatedRect.height
                x: associatedRect.x
                y: associatedRect.y

                onEditPreviousRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MovePrevious)
                onEditNextRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveNext)
                onEditHomeRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveHome)
                onEditEndRequested: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.MoveEnd)
                onAccepted: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.CommitEditing)
                onRejected: noteEditLayer.noteEditLayerInteractionController?.additionalTextInPlaceEditOperationTriggered(noteEditLayer, model, NoteEditLayerInteractionController.AbortEditing)
            }
        }
    }
}
