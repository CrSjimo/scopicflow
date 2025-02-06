import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: noteArea
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject clavierViewModel: null
    property QtObject animationViewModel: null
    property QtObject noteSequenceViewModel: null
    property QtObject pianoRollNoteAreaBehaviorViewModel: null

    required property rect viewport

    property QtObject stylesheet: PianoRollNoteAreaStylesheet {}
    readonly property QtObject popupEditStyleItem: stylesheet.popupEdit.createObject(noteArea)
    readonly property QtObject rubberBandStyleItem: stylesheet.rubberBand.createObject(noteArea)

    signal noteCut(model: QtObject, position: int)
    signal noteContextMenuRequired(model: QtObject)
    signal doubleClicked(position: int, key: int)
    signal contextMenuRequired(position: int, key: int)

    function ensureCurrentItemVisible() {
        if (noteSequenceViewModel?.handle.currentItem && pianoRollNoteAreaBehaviorViewModel?.editing) {
            timeManipulator.ensureVisible(noteSequenceViewModel.handle.currentItem.position, noteSequenceViewModel.handle.currentItem.length)
        }
    }

    Connections {
        target: noteArea.noteSequenceViewModel?.handle ?? null
        function onCurrentItemChanged() {
            noteArea.ensureCurrentItemVisible()
        }
    }
    Connections {
        target: noteArea.pianoRollNoteAreaBehaviorViewModel
        function onEditingChanged() {
            noteArea.ensureCurrentItemVisible()
        }
    }

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: noteArea.noteSequenceViewModel
    }
    TimeAlignmentPositionLocator {
        id: timeLocator
        anchors.fill: parent
        timeViewModel: noteArea.timeViewModel
        timeLayoutViewModel: noteArea.timeLayoutViewModel
    }
    ClavierLocator {
        id: clavierLocator
        anchors.fill: parent
        clavierViewModel: noteArea.clavierViewModel
    }
    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: noteArea.timeViewModel
        timeLayoutViewModel: noteArea.timeLayoutViewModel
        animationViewModel: noteArea.animationViewModel
    }
    ClavierManipulator {
        id: clavierManipulator
        anchors.fill: parent
        clavierViewModel: noteArea.clavierViewModel
        animationViewModel: noteArea.animationViewModel
    }
    PianoRollNoteAreaRubberBandHelper {
        id: rubberBandHelper
        noteSequenceViewModel: selectionManipulator.viewModel
        rubberBandLayer: rubberBandLayer
        function viewportPointToRubberBandPoint(p) {
            return Qt.point(p.x / noteArea.timeLayoutViewModel.pixelDensity, (viewport.height - p.y) / noteArea.clavierViewModel.pixelDensity)
        }
        function rubberBandPointToViewportPoint(p) {
            if (!noteArea.timeLayoutViewModel || !noteArea.clavierViewModel)
                return Qt.point(0, 0)
            return Qt.point(p.x * noteArea.timeLayoutViewModel.pixelDensity, viewport.height - p.y * noteArea.clavierViewModel.pixelDensity)
        }
    }

    Item {
        id: viewport
        x: noteArea.viewport.x
        y: noteArea.viewport.y
        width: noteArea.viewport.width
        height: noteArea.viewport.height

        MouseArea {
            id: backRightButtonMouseArea
            anchors.fill: parent
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior !== PianoRollNoteAreaBehaviorViewModel.None
            acceptedButtons: Qt.RightButton
            onClicked: (mouse) => {
                selectionManipulator.select(null, mouse.button, mouse.modifiers)
                let parentPoint = mapToItem(noteArea, mouse.x, mouse.y);
                noteArea.contextMenuRequired(timeLocator.mapToTick(parentPoint.x), clavierLocator.mapToKey(parentPoint.y))
            }
        }

        GenericBackPointerMouseArea {
            id: backPointerMouseArea
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pointer
            paneItem: noteArea
            verticalManipulator: clavierManipulator

            onRubberBandStartRequired: (p) => {
                rubberBandLayer.startSelection(rubberBandHelper.viewportPointToRubberBandPoint(p))
            }
            onRubberBandUpdateRequired: (p) => {
                rubberBandLayer.updateSelection(rubberBandHelper.viewportPointToRubberBandPoint(p))
            }

            onDoubleClicked: (mouse) => {
                let parentPoint = mapToItem(noteArea, mouse.x, mouse.y);
                noteArea.doubleClicked(timeLocator.mapToTick(parentPoint.x), clavierLocator.mapToKey(parentPoint.y))
            }
        }
        GenericBackPenMouseArea {
            id: backPenMouseArea
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pen
            sequenceViewModel: noteArea.noteSequenceViewModel
            paneItem: noteArea
            viewModelComponent: NoteViewModel {}
            lengthHint: noteArea.pianoRollNoteAreaBehaviorViewModel?.lengthHint ?? 0
            mappedYProperty: "key"
            mapY: (y) => {
                return clavierLocator.mapToKey(mapToItem(noteArea, 0, y).y)
            }
            onClicked: (mouse) => {
                if (dragged)
                    return
                backPointerMouseArea.clicked(mouse)
            }
        }

        Item {
            id: noteContainer
            anchors.fill: parent
            property double incrementZCounter: 0
            SequenceSlicer {
                model: noteArea.noteSequenceViewModel
                timeViewModel: noteArea.timeViewModel
                timeLayoutViewModel: noteArea.timeLayoutViewModel
                sliceWidth: noteArea.width
                delegate: Item {
                    id: noteRect
                    required property QtObject model
                    property bool current: {current = model === noteArea.noteSequenceViewModel.handle.currentItem}
                    property bool editing: popup.opened
                    property bool editingRequired: {editingRequired = (noteArea.pianoRollNoteAreaBehaviorViewModel?.editing ?? false) && current}
                    property QtObject noteStyleItem: {noteStyleItem = noteArea.stylesheet.pianoRollNoteArea.createObject(noteRect, {noteViewModel: model, current, noteColor: noteArea.pianoRollNoteAreaBehaviorViewModel?.color ?? "white"})}
                    opacity: eraserMouseArea.willBeErased ? 0.5 : 1
                    Binding {
                        when: noteRect.visible
                        noteRect.x: noteRect.model.position * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                        noteRect.y: (127 - noteRect.model.key) * (noteArea.clavierViewModel?.pixelDensity ?? 0)
                        noteRect.width: noteRect.model.length * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                        noteRect.height: (noteArea.clavierViewModel?.pixelDensity ?? 0)
                        noteRect.current: noteArea.noteSequenceViewModel && noteRect.model === noteArea.noteSequenceViewModel.handle.currentItem
                        noteRect.noteStyleItem: noteArea.stylesheet.pianoRollNoteArea.createObject(noteArea, {noteViewModel: noteRect.model, current: noteRect.current, noteColor: noteArea.pianoRollNoteAreaBehaviorViewModel?.color ?? "white"})
                        noteRect.editingRequired: (noteArea.pianoRollNoteAreaBehaviorViewModel?.editing ?? false) && noteRect.current
                        background.radius: noteArea.pianoRollNoteAreaBehaviorViewModel?.compactDisplay ? 0.5 * background.height : 4
                        background.height: noteRect.height * (noteArea.pianoRollNoteAreaBehaviorViewModel?.compactDisplay ? 0.5 : 1)
                        lyricText.visible: !(noteArea.pianoRollNoteAreaBehaviorViewModel?.compactDisplay)
                    }
                    clip: true

                    function bringToFront() {
                        if (model.overlapped)
                            z = ++noteContainer.incrementZCounter
                    }

                    onEditingChanged: {
                        if (current && noteArea.pianoRollNoteAreaBehaviorViewModel) {
                            noteArea.pianoRollNoteAreaBehaviorViewModel.editing = editing
                        }
                    }

                    onEditingRequiredChanged: {
                        if (editingRequired)
                            popup.open()
                        else
                            popup.close()
                    }

                    Rectangle {
                        id: background
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        color: noteRect.noteStyleItem.background
                        Behavior on color {
                            ColorAnimation {
                                duration: (noteArea.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                                easing.type: Easing.OutCubic
                            }
                        }
                    }

                    Rectangle {
                        id: border
                        anchors.fill: background
                        color: "transparent"
                        radius: background.radius
                        border.width: Math.min(noteRect.model.selected ? 2 : 1, width / 4)
                        opacity: noteRect.model.selected ? 1 : 0.5
                        border.color: noteRect.noteStyleItem.border
                        Behavior on border.color {
                            ColorAnimation {
                                duration: (noteArea.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                                easing.type: Easing.OutCubic
                            }
                        }
                    }

                    Text {
                        id: lyricText
                        anchors.left: parent.left
                        anchors.leftMargin: 4
                        anchors.right: parent.right
                        anchors.rightMargin: 4
                        anchors.verticalCenter: parent.verticalCenter
                        text: noteRect.model.lyric
                        clip: true
                        color: noteRect.noteStyleItem.foreground
                        Behavior on color {
                            ColorAnimation {
                                duration: (noteArea.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                    ItemPopupEdit {
                        id: popup
                        model: noteRect.model
                        containerModel: noteArea.noteSequenceViewModel
                        targetProperty: "lyric"
                        styleItem: noteArea.popupEditStyleItem
                        Binding {
                            when: popup.opened
                            popup.width: noteRect.width
                        }
                        radius: background.radius
                    }
                    Connections {
                        id: cursorIndicatorLeftBinding
                        target: noteRect.model
                        enabled: false
                        function onPositionChanged() {
                            noteArea.timeLayoutViewModel.cursorPosition = noteRect.model.position
                        }
                        function onKeyChanged() {
                            noteArea.clavierViewModel.cursorPosition = noteRect.model.key
                        }
                    }
                    Connections {
                        id: cursorIndicatorRightBinding
                        target: noteRect.model
                        enabled: false
                        function onPositionChanged() {
                            noteArea.timeLayoutViewModel.cursorPosition = noteRect.model.position + noteRect.model.length
                        }
                        function onLengthChanged() {
                            noteArea.timeLayoutViewModel.cursorPosition = noteRect.model.position + noteRect.model.length
                        }
                        function onKeyChanged() {
                            noteArea.clavierViewModel.cursorPosition = noteRect.model.key
                        }
                    }
                    MouseArea {
                        id: rightButtonMouseArea
                        anchors.fill: parent
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior !== PianoRollNoteAreaBehaviorViewModel.None
                        acceptedButtons: Qt.RightButton
                        onClicked: (mouse) => {
                            selectionManipulator.select(noteRect.model, mouse.button, mouse.modifiers)
                            noteArea.noteContextMenuRequired(noteRect.model)
                        }
                    }
                    GenericPointerMouseArea {
                        id: pointerMouseArea
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pointer || noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pen

                        verticalManipulator: clavierManipulator
                        paneItem: noteArea
                        sequenceViewModel: noteArea.noteSequenceViewModel
                        model: noteRect.model

                        onPressedChanged: () => {
                            if (pressed)
                                noteRect.bringToFront()
                        }

                        onDraggingChanged: {
                            if (dragging) {
                                cursorIndicatorLeftBinding.enabled = true
                                cursorIndicatorLeftBinding.onPositionChanged()
                                cursorIndicatorLeftBinding.onKeyChanged()
                            } else {
                                cursorIndicatorLeftBinding.enabled = false
                                noteArea.timeLayoutViewModel.cursorPosition = -1
                                noteArea.clavierViewModel.cursorPosition = -1
                            }
                        }

                        onMoveSelectedNotesToY: (y) => {
                            let key = Math.round(noteArea.clavierViewModel.start + (noteArea.height - y) / noteArea.clavierViewModel.pixelDensity - 1)
                            if (key !== model.key) {
                                let deltaKey = key - model.key
                                for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                    if (note.key + deltaKey < 0 || note.key + deltaKey >= 128)
                                        return
                                }
                                for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                    note.key = note.key + deltaKey
                                }
                            }
                        }

                        onDoubleClicked: (mouse) => {
                            noteArea.noteSequenceViewModel.handle.currentItem = model
                            noteArea.pianoRollNoteAreaBehaviorViewModel.editing = true
                        }
                    }
                    Repeater {
                        model: 2
                        GenericEdgeMouseArea {
                            id: edgeMouseArea
                            required property int index
                            leftEdge: index
                            visible: pointerMouseArea.visible
                            unitedExtend: noteArea.pianoRollNoteAreaBehaviorViewModel?.unitedExtend ?? false
                            unitedExtendEnabled: true
                            model: noteRect.model
                            sequenceViewModel: noteArea.noteSequenceViewModel
                            paneItem: noteArea

                            onPressedChanged: () => {
                                if (pressed)
                                    noteRect.bringToFront()
                            }

                            onDraggingChanged: () => {
                                let binding = leftEdge ? cursorIndicatorLeftBinding : cursorIndicatorRightBinding
                                if (dragging) {
                                    binding.enabled = true
                                    binding.onPositionChanged()
                                    binding.onKeyChanged()
                                } else {
                                    binding.enabled = false
                                    noteArea.timeLayoutViewModel.cursorPosition = -1
                                    noteArea.clavierViewModel.cursorPosition = -1
                                }
                            }

                            onClicked: (mouse) => {
                                if (dragged)
                                    return
                                pointerMouseArea.clicked(mouse)
                            }
                            onDoubleClicked: (mouse) => {
                                pointerMouseArea.doubleClicked(mouse)
                            }
                        }
                    }
                    GenericScissorMouseArea {
                        id: scissorMouseArea
                        model: noteRect.model
                        paneItem: noteArea
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Scissor
                        onPressedChanged: () => {
                            if (pressed)
                                noteRect.bringToFront()
                        }
                        onCutPositionChanged: () => {
                            noteArea.timeLayoutViewModel.cursorPosition = cutPosition
                        }
                        onReleased: (mouse) => {
                            if (cutPosition !== -1)
                                noteArea.noteCut(model, cutPosition)
                        }
                    }
                    GenericEraserMouseArea {
                        id: eraserMouseArea
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Eraser
                        onReleased: (mouse) => {
                            if (willBeErased)
                                noteArea.noteSequenceViewModel.handle.removeItem(noteRect.model)
                        }
                    }
                }
            }
        }

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            rubberBand: Item {
                id: mappingAdapter
                Rectangle {
                    readonly property point p1: rubberBandHelper.rubberBandPointToViewportPoint(Qt.point(mappingAdapter.x, mappingAdapter.y + mappingAdapter.height))
                    readonly property point p2: rubberBandHelper.rubberBandPointToViewportPoint(Qt.point(mappingAdapter.x + mappingAdapter.width, mappingAdapter.y))
                    x: p1.x - mappingAdapter.x
                    y: p1.y - mappingAdapter.y
                    width: p2.x - p1.x
                    height: p2.y - p1.y
                    color: noteArea.rubberBandStyleItem.background
                    border.width: 1
                    border.color: noteArea.rubberBandStyleItem.border
                }
            }
        }
    }
}