pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import QtQuick.Controls.impl

import SVSCraft
import SVSCraft.UIComponents
import SVSCraft.UIComponents.impl

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
    component NoteRubberBandDragHandler: RubberBandDragHandler {
        controller: noteEditLayer.noteEditLayerInteractionController
        selectionController: noteEditLayer.selectionController
        target: noteEditLayer
        coordinateSpace: editArea
        timeManipulator: timeManipulator
        verticalManipulator: clavierManipulator
        rubberBandLayer: rubberBandLayer
    }
    Item {
        id: editArea
        x: 0
        y: 0
        width: noteEditLayer.width
        height: Math.max(0, noteEditLayer.height - noteEditLayer.bottomExpansion)
        readonly property ClipViewModel clipViewModel: noteEditLayer.clipViewModel
        readonly property RangeSequenceViewModel noteSequenceViewModel: noteEditLayer.noteSequenceViewModel
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
        }
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
                            noteDelegate.color: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0]?.color ?? Theme.accentColor
                            noteDelegate.visible: !noteEditLayer.thumbnailDisplay
                            when: noteDelegate.SequenceSlicerLoader.inRange
                        }
                        MouseArea {
                            height: parent.height
                            width: Math.min(parent.width, parent.implicitWidth)
                            onDoubleClicked: () => {
                                noteEditLayer.noteEditLayerInteractionController.itemAdditionalTextDoubleClicked(noteEditLayer, noteDelegate.SequenceSlicerLoader.viewModel)
                            }
                        }
                    }
                }
            }
            Item {
                id: slicerContainer
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

                        property int nextNotePos: 0
                        property int restLength: 0
                        property double restWidth: 0
                        property double restLabelAnchorY: 0

                        Binding {
                            noteDelegate.x: (noteDelegate.noteViewModel?.position ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                            noteDelegate.y: (127 - (noteDelegate.noteViewModel?.key ?? 0)) * (noteEditLayer.clavierViewModel?.pixelDensity ?? 0)
                            noteDelegate.width: (noteDelegate.noteViewModel?.length ?? 0) * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                            noteDelegate.height: noteEditLayer.clavierViewModel?.pixelDensity ?? 0
                            noteDelegate.color: noteEditLayer.trackListViewModel?.items[noteEditLayer.clipViewModel?.trackIndex ?? 0]?.color ?? Theme.accentColor
                            noteDelegate.transparentDisplay: noteEditLayer.transparentDisplay
                            noteDelegate.thumbnailDisplay: noteEditLayer.thumbnailDisplay
                            noteDelegate.nextNotePos: noteDelegate.noteViewModel?.nextNotePosition ?? 0
                            noteDelegate.restLength: (noteDelegate.noteViewModel?.nextNotePosition ?? 0) - (noteDelegate.noteViewModel?.position ?? 0) - (noteDelegate.noteViewModel?.length ?? 0)
                            noteDelegate.restWidth: noteDelegate.restLength * (noteEditLayer.timeLayoutViewModel?.pixelDensity ?? 0)
                            noteDelegate.restLabelAnchorY: {
                                let nextNoteY = (127 - (noteDelegate.noteViewModel?.nextNoteKey ?? 0)) * (noteEditLayer.clavierViewModel?.pixelDensity ?? 0)
                                return noteEditLayer.noteEditLayerInteractionController?.additionalTextPosition === NoteEditLayerInteractionController.AdditionalTextPosition_Up ? Math.min(nextNoteY, noteDelegate.y) : Math.max(nextNoteY, noteDelegate.y)
                            }
                            shortNoteLabel.visible: noteDelegate.noteViewModel?.length <= noteEditLayer.noteEditLayerInteractionController?.shortNoteThreshold
                            shortNoteLabel.anchors.verticalCenterOffset: noteEditLayer.noteEditLayerInteractionController?.additionalTextPosition === NoteEditLayerInteractionController.AdditionalTextPosition_Up ? 0.5 * noteDelegate.height + 16 : -0.5 * noteDelegate.height - 16
                            shortRestLabel.visible: noteDelegate.restLength > 0 && noteDelegate.restLength <= noteEditLayer.noteEditLayerInteractionController?.shortNoteThreshold
                            shortRestLabel.anchors.verticalCenterOffset: noteEditLayer.noteEditLayerInteractionController?.additionalTextPosition === NoteEditLayerInteractionController.AdditionalTextPosition_Up ? -0.5 * noteDelegate.height - 16 : 0.5 * noteDelegate.height + 16
                            when: noteDelegate.SequenceSlicerLoader.inRange
                        }

                        Item {
                            parent: slicerContainer
                            x: noteDelegate.x + 0.5 * noteDelegate.width
                            y: noteDelegate.y + 0.5 * noteDelegate.height
                            z: 2.5
                            opacity: 0.75
                            T.Button {
                                id: shortNoteLabel
                                width: 20
                                height: 20
                                anchors.centerIn: parent
                                background: ButtonRectangle {
                                    control: shortNoteLabel
                                }
                                contentItem: IconImage {
                                    source: "qrc:/qt/qml/dev/sjimo/ScopicFlow/Internal/assets/music_note.svg"
                                    sourceSize.width: 16
                                    sourceSize.height: 16
                                    color: noteDelegate.selected ? Theme.accentColor : Theme.foregroundPrimaryColor
                                }
                                display: T.AbstractButton.IconOnly
                                text: qsTr("Short note")
                                ActionToolTipHelper {
                                    text: shortNoteLabel.text
                                    delay: Theme.toolTipDelay
                                    timeout: Theme.toolTipTimeout
                                    visible: shortNoteLabel.hovered
                                }
                                onClicked: () => {
                                    noteEditLayer.selectionController.select(noteDelegate.noteViewModel, SelectionController.ClearPreviousSelection | SelectionController.SetCurrentItem | SelectionController.Select)
                                }
                            }
                        }

                        Item {
                            parent: slicerContainer
                            x: noteDelegate.x + noteDelegate.width + 0.5 * (noteDelegate.restWidth)
                            y: noteDelegate.restLabelAnchorY + 0.5 * noteDelegate.height
                            z: 2.5
                            opacity: 0.75
                            T.Button {
                                id: shortRestLabel
                                width: 20
                                height: 20
                                anchors.centerIn: parent
                                background: ButtonRectangle {
                                    control: shortRestLabel
                                }
                                contentItem: IconImage {
                                    source: "qrc:/qt/qml/dev/sjimo/ScopicFlow/Internal/assets/music_rest.svg"
                                    sourceSize.width: 16
                                    sourceSize.height: 16
                                    color: Theme.foregroundPrimaryColor
                                }
                                display: T.AbstractButton.IconOnly
                                text: qsTr("Short rest")
                                ActionToolTipHelper {
                                    text: shortRestLabel.text
                                    delay: Theme.toolTipDelay
                                    timeout: Theme.toolTipTimeout
                                    visible: shortRestLabel.hovered
                                }
                                Menu {
                                    id: shortRestMenu
                                    Action {
                                        text: qsTr("Extend previous note forward")
                                        onTriggered: noteEditLayer.noteEditLayerInteractionController.rippleDeleteRequested(noteEditLayer, noteDelegate.noteViewModel, NoteEditLayerInteractionController.RippleDelete_Previous)
                                    }
                                    Action {
                                        text: qsTr("Extend next note backward")
                                        onTriggered: noteEditLayer.noteEditLayerInteractionController.rippleDeleteRequested(noteEditLayer, noteDelegate.noteViewModel, NoteEditLayerInteractionController.RippleDelete_Next)
                                    }
                                }
                                onClicked: () => {
                                    shortRestMenu.popup()
                                }
                            }
                        }

                        RubberBandItemConnections {
                            target: noteDelegate
                            viewModel: noteDelegate.noteViewModel
                            rubberBandLayer: rubberBandLayer
                        }
                        ItemPointerInput {
                            sceneInput: scenePointerInput
                            item: noteDelegate
                            viewModel: noteDelegate.noteViewModel
                            payloadResolver: point => {
                                const edgeWidth = Math.min(8, noteDelegate.width / 3)
                                if (point.x < edgeWidth)
                                    return EdgeDragHandler.LeftEdge
                                if (point.x >= noteDelegate.width - edgeWidth)
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
                            freezeCursorOnPress: true
                            itemPressCursorResolver: hit => hit.payload === -1
                                ? undefined : Qt.SizeHorCursor
                        }
                        MouseArea {
                            anchors.left: parent.left
                            width: Math.min(8, parent.width / 3)
                            height: parent.height
                            acceptedButtons: Qt.NoButton
                            cursorShape: Qt.SizeHorCursor
                        }
                        MouseArea {
                            anchors.right: parent.right
                            width: Math.min(8, parent.width / 3)
                            height: parent.height
                            acceptedButtons: Qt.NoButton
                            cursorShape: Qt.SizeHorCursor
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
                width: associatedItem?.implicitWidth ?? 0
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
