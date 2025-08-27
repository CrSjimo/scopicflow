import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow

Item {
    id: noteArea

    property QtObject animationViewModel: null
    property QtObject clavierViewModel: null
    readonly property color noteColor: pianoRollNoteAreaBehaviorViewModel?.color ?? Theme.accentColor
    property QtObject noteSequenceViewModel: null
    readonly property double offsetSize: (pianoRollNoteAreaBehaviorViewModel?.offset ?? 0) * (timeLayoutViewModel?.pixelDensity ?? 0)
    property QtObject pianoRollNoteAreaBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    required property rect viewport

    signal contextMenuRequired(position: int, key: int)
    signal doubleClicked(position: int, key: int)
    signal noteContextMenuRequired(model: QtObject)
    signal noteCut(model: QtObject, position: int)

    function ensureCurrentItemVisible() {
        if (noteSequenceViewModel?.handle.currentItem && pianoRollNoteAreaBehaviorViewModel?.editing) {
            timeManipulator.ensureVisible(noteSequenceViewModel.handle.currentItem.position, noteSequenceViewModel.handle.currentItem.length);
        }
    }

    Connections {
        function onCurrentItemChanged() {
            noteArea.ensureCurrentItemVisible();
        }

        target: noteArea.noteSequenceViewModel?.handle ?? null
    }
    Connections {
        function onEditingChanged() {
            noteArea.ensureCurrentItemVisible();
        }

        target: noteArea.pianoRollNoteAreaBehaviorViewModel
    }
    SelectableViewModelManipulator {
        id: selectionManipulator

        viewModel: noteArea.noteSequenceViewModel
    }
    QtObject {
        id: pseudoTimeViewModel

        property double start: (noteArea.timeViewModel?.start ?? 0) - (noteArea.pianoRollNoteAreaBehaviorViewModel?.offset ?? 0)
    }
    TimeAlignmentPositionLocator {
        id: timeLocator

        anchors.fill: parent
        timeLayoutViewModel: noteArea.timeLayoutViewModel
        timeViewModel: pseudoTimeViewModel
    }
    ClavierLocator {
        id: clavierLocator

        anchors.fill: parent
        clavierViewModel: noteArea.clavierViewModel
    }
    TimeManipulator {
        id: timeManipulator

        anchors.fill: parent
        animationViewModel: noteArea.animationViewModel
        timeLayoutViewModel: noteArea.timeLayoutViewModel
        timeViewModel: noteArea.timeViewModel
    }
    ClavierManipulator {
        id: clavierManipulator

        anchors.fill: parent
        animationViewModel: noteArea.animationViewModel
        clavierViewModel: noteArea.clavierViewModel
    }
    PianoRollNoteAreaRubberBandHelper {
        id: rubberBandHelper

        function rubberBandPointToViewportPoint(p) {
            if (!noteArea.timeLayoutViewModel || !noteArea.clavierViewModel)
                return Qt.point(0, 0);
            return Qt.point(p.x * noteArea.timeLayoutViewModel.pixelDensity + noteArea.offsetSize, viewport.height - p.y * noteArea.clavierViewModel.pixelDensity);
        }
        function viewportPointToRubberBandPoint(p) {
            return Qt.point((p.x - noteArea.offsetSize) / noteArea.timeLayoutViewModel.pixelDensity, (viewport.height - p.y) / noteArea.clavierViewModel.pixelDensity);
        }

        noteSequenceViewModel: selectionManipulator.viewModel
        rubberBandLayer: rubberBandLayer
    }
    Item {
        id: viewport

        height: noteArea.viewport.height
        width: noteArea.viewport.width
        x: noteArea.viewport.x
        y: noteArea.viewport.y

        MouseArea {
            id: backRightButtonMouseArea

            acceptedButtons: Qt.RightButton
            anchors.fill: parent
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior !== ScopicFlow.MB_None

            onClicked: mouse => {
                selectionManipulator.select(null, mouse.button, mouse.modifiers);
                let parentPoint = mapToItem(noteArea, mouse.x, mouse.y);
                noteArea.contextMenuRequired(timeLocator.mapToTick(parentPoint.x), clavierLocator.mapToKey(parentPoint.y));
            }
        }
        GenericBackPointerMouseArea {
            id: backPointerMouseArea

            paneItem: noteArea
            verticalManipulator: clavierManipulator
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pointer

            onDoubleClicked: mouse => {
                let parentPoint = mapToItem(noteArea, mouse.x, mouse.y);
                noteArea.doubleClicked(timeLocator.mapToTick(parentPoint.x), clavierLocator.mapToKey(parentPoint.y));
            }
            onRubberBandStartRequired: p => {
                rubberBandLayer.startSelection(rubberBandHelper.viewportPointToRubberBandPoint(p));
            }
            onRubberBandUpdateRequired: p => {
                rubberBandLayer.updateSelection(rubberBandHelper.viewportPointToRubberBandPoint(p));
            }
        }
        GenericBackPenMouseArea {
            id: backPenMouseArea

            lengthHint: noteArea.pianoRollNoteAreaBehaviorViewModel?.lengthHint ?? 0
            mapY: y => {
                return clavierLocator.mapToKey(mapToItem(noteArea, 0, y).y);
            }
            mappedYProperty: "key"
            paneItem: noteArea
            sequenceViewModel: noteArea.noteSequenceViewModel
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pen

            viewModelComponent: NoteViewModel {
            }

            onClicked: mouse => {
                if (dragged)
                    return;
                backPointerMouseArea.clicked(mouse);
            }
        }
        Item {
            id: noteContainer

            property double incrementZCounter: 0

            anchors.fill: parent

            SequenceSlicer {
                model: noteArea.noteSequenceViewModel
                sliceWidth: noteArea.width
                timeLayoutViewModel: noteArea.timeLayoutViewModel
                timeViewModel: noteArea.timeViewModel

                delegate: Item {
                    id: noteRect

                    property bool current: {
                        current = model === noteArea.noteSequenceViewModel.handle.currentItem;
                    }
                    property bool editing: popup.opened
                    property bool editingRequired: {
                        editingRequired = (noteArea.pianoRollNoteAreaBehaviorViewModel?.editing ?? false) && current;
                    }
                    required property QtObject model

                    function bringToFront() {
                        if (model.overlapped)
                            z = ++noteContainer.incrementZCounter;
                    }

                    clip: true
                    opacity: eraserMouseArea.willBeErased ? 0.5 : 1

                    onEditingChanged: {
                        if (current && noteArea.pianoRollNoteAreaBehaviorViewModel) {
                            noteArea.pianoRollNoteAreaBehaviorViewModel.editing = editing;
                        }
                    }
                    onEditingRequiredChanged: {
                        if (editingRequired)
                            popup.open();
                        else
                            popup.close();
                    }

                    Binding {
                        background.height: noteRect.height * (noteArea.pianoRollNoteAreaBehaviorViewModel?.compactDisplay ? 0.5 : 1)
                        background.radius: noteArea.pianoRollNoteAreaBehaviorViewModel?.compactDisplay ? 0.5 * background.height : 4
                        lyricText.visible: !(noteArea.pianoRollNoteAreaBehaviorViewModel?.compactDisplay)
                        noteRect.current: noteArea.noteSequenceViewModel && noteRect.model === noteArea.noteSequenceViewModel.handle.currentItem
                        noteRect.editingRequired: (noteArea.pianoRollNoteAreaBehaviorViewModel?.editing ?? false) && noteRect.current
                        noteRect.height: (noteArea.clavierViewModel?.pixelDensity ?? 0)
                        noteRect.width: noteRect.model.length * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                        noteRect.x: noteRect.model.position * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0) + noteArea.offsetSize
                        noteRect.y: (127 - noteRect.model.key) * (noteArea.clavierViewModel?.pixelDensity ?? 0)
                        when: noteRect.visible
                    }
                    Rectangle {
                        id: background

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        color: noteRect.model.selected ? SFPalette.noteSelectedColorChange.apply(noteArea.noteColor) : noteArea.noteColor

                        Behavior on color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                    Rectangle {
                        id: border

                        anchors.fill: background
                        border.color: SFPalette.noteBorderColorChange.apply(noteArea.noteColor)
                        border.width: Math.min(noteRect.model.selected ? 2 : 1, width / 4)
                        color: "transparent"
                        opacity: noteRect.model.selected ? 1 : 0.5
                        radius: background.radius

                        Behavior on border.color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
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
                        clip: true
                        color: SFPalette.suitableForegroundColor(noteArea.noteColor)
                        text: noteRect.model.lyric

                        Behavior on color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                    ItemPopupEdit {
                        id: popup

                        containerModel: noteArea.noteSequenceViewModel
                        model: noteRect.model
                        radius: background.radius
                        targetProperty: "lyric"

                        Binding {
                            popup.width: noteRect.width
                            when: popup.opened
                        }
                    }
                    Connections {
                        id: cursorIndicatorLeftBinding

                        function onKeyChanged() {
                            noteArea.clavierViewModel.cursorPosition = noteRect.model.key;
                        }
                        function onPositionChanged() {
                            noteArea.timeLayoutViewModel.cursorPosition = noteRect.model.position + noteArea.pianoRollNoteAreaBehaviorViewModel.offset;
                        }

                        enabled: false
                        target: noteRect.model
                    }
                    Connections {
                        id: cursorIndicatorRightBinding

                        function onKeyChanged() {
                            noteArea.clavierViewModel.cursorPosition = noteRect.model.key;
                        }
                        function onLengthChanged() {
                            noteArea.timeLayoutViewModel.cursorPosition = noteRect.model.position + noteRect.model.length + noteArea.pianoRollNoteAreaBehaviorViewModel.offset;
                        }
                        function onPositionChanged() {
                            noteArea.timeLayoutViewModel.cursorPosition = noteRect.model.position + noteRect.model.length + noteArea.pianoRollNoteAreaBehaviorViewModel.offset;
                        }

                        enabled: false
                        target: noteRect.model
                    }
                    MouseArea {
                        id: rightButtonMouseArea

                        acceptedButtons: Qt.RightButton
                        anchors.fill: parent
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior !== ScopicFlow.MB_None

                        onClicked: mouse => {
                            selectionManipulator.select(noteRect.model, mouse.button, mouse.modifiers);
                            noteArea.noteContextMenuRequired(noteRect.model);
                        }
                    }
                    GenericPointerMouseArea {
                        id: pointerMouseArea

                        model: noteRect.model
                        paneItem: noteArea
                        sequenceViewModel: noteArea.noteSequenceViewModel
                        verticalManipulator: clavierManipulator
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pointer || noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pen

                        onDoubleClicked: mouse => {
                            noteArea.noteSequenceViewModel.handle.currentItem = model;
                            noteArea.pianoRollNoteAreaBehaviorViewModel.editing = true;
                        }
                        onDraggingChanged: {
                            if (dragging) {
                                cursorIndicatorLeftBinding.enabled = true;
                                cursorIndicatorLeftBinding.onPositionChanged();
                                cursorIndicatorLeftBinding.onKeyChanged();
                            } else {
                                cursorIndicatorLeftBinding.enabled = false;
                                noteArea.timeLayoutViewModel.cursorPosition = -1;
                                noteArea.clavierViewModel.cursorPosition = -1;
                            }
                        }
                        onMoveSelectedNotesToY: y => {
                            let key = Math.round(noteArea.clavierViewModel.start + (noteArea.height - y) / noteArea.clavierViewModel.pixelDensity - 1);
                            if (key !== model.key) {
                                let deltaKey = key - model.key;
                                for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                    if (note.key + deltaKey < 0 || note.key + deltaKey >= 128)
                                        return;
                                }
                                for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                    note.key = note.key + deltaKey;
                                }
                            }
                        }
                        onPressedChanged: () => {
                            if (pressed)
                                noteRect.bringToFront();
                        }
                    }
                    Repeater {
                        model: 2

                        GenericEdgeMouseArea {
                            id: edgeMouseArea

                            required property int index

                            leftEdge: index
                            model: noteRect.model
                            paneItem: noteArea
                            sequenceViewModel: noteArea.noteSequenceViewModel
                            unitedExtend: noteArea.pianoRollNoteAreaBehaviorViewModel?.unitedExtend ?? false
                            unitedExtendEnabled: true
                            visible: pointerMouseArea.visible

                            onClicked: mouse => {
                                if (dragged)
                                    return;
                                pointerMouseArea.clicked(mouse);
                            }
                            onDoubleClicked: mouse => {
                                pointerMouseArea.doubleClicked(mouse);
                            }
                            onDraggingChanged: () => {
                                let binding = leftEdge ? cursorIndicatorLeftBinding : cursorIndicatorRightBinding;
                                if (dragging) {
                                    binding.enabled = true;
                                    binding.onPositionChanged();
                                    binding.onKeyChanged();
                                } else {
                                    binding.enabled = false;
                                    noteArea.timeLayoutViewModel.cursorPosition = -1;
                                    noteArea.clavierViewModel.cursorPosition = -1;
                                }
                            }
                            onPressedChanged: () => {
                                if (pressed)
                                    noteRect.bringToFront();
                            }
                        }
                    }
                    GenericScissorMouseArea {
                        id: scissorMouseArea

                        model: noteRect.model
                        paneItem: noteArea
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Scissor

                        onCutPositionChanged: () => {
                            if (cutPosition >= 0)
                                noteArea.timeLayoutViewModel.cursorPosition = cutPosition + noteArea.pianoRollNoteAreaBehaviorViewModel.offset;
                            else
                                noteArea.timeLayoutViewModel.cursorPosition = -1;
                        }
                        onPressedChanged: () => {
                            if (pressed)
                                noteRect.bringToFront();
                        }
                        onReleased: mouse => {
                            if (cutPosition !== -1)
                                noteArea.noteCut(model, cutPosition);
                        }
                    }
                    GenericEraserMouseArea {
                        id: eraserMouseArea

                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Eraser

                        onReleased: mouse => {
                            if (willBeErased)
                                noteArea.noteSequenceViewModel.handle.removeItem(noteRect.model);
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

                RubberBandRectangle {
                    readonly property point p1: rubberBandHelper.rubberBandPointToViewportPoint(Qt.point(mappingAdapter.x, mappingAdapter.y + mappingAdapter.height))
                    readonly property point p2: rubberBandHelper.rubberBandPointToViewportPoint(Qt.point(mappingAdapter.x + mappingAdapter.width, mappingAdapter.y))

                    height: p2.y - p1.y
                    width: p2.x - p1.x
                    x: p1.x - mappingAdapter.x
                    y: p1.y - mappingAdapter.y
                }
            }
        }
    }
}
