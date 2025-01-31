import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal

Item {
    id: noteArea
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject clavierViewModel: null
    property QtObject animationViewModel: null
    property QtObject noteSequenceViewModel: null
    property QtObject pianoRollNoteAreaBehaviorViewModel: null

    required property rect viewport

    required property QtObject stylesheet
    readonly property QtObject popupEditStyleItem: stylesheet.popupEdit.createObject(noteArea)

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

    function moveSelectionTo(position, key, model) {
        if (position !== model.position) {
            let deltaPosition = position - model.position
            for (let note of noteSequenceViewModel.handle.selection) {
                if (note.position + deltaPosition < 0)
                    return
                if (note.position + note.length + deltaPosition > timeViewModel.end)
                    timeViewModel.end = note.position + deltaPosition
            }
            for (let note of noteSequenceViewModel.handle.selection) {
                note.position = note.position + deltaPosition
            }
        }
        if (key !== model.key) {
            let deltaKey = key - model.key
            for (let note of noteSequenceViewModel.handle.selection) {
                if (note.key + deltaKey < 0 || note.key + deltaKey >= 128)
                    return
            }
            for (let note of noteSequenceViewModel.handle.selection) {
                note.key = note.key + deltaKey
            }
        }
    }
    function moveSelectedNotesToX(x, model) {
        moveSelectionTo(Math.max(Math.min(locator.alignTickCeil(timeViewModel.start), model.position), Math.min(locator.alignTick(locator.mapToTick(x)), Math.max(model.position, locator.alignTickFloor(timeViewModel.start + width / timeLayoutViewModel.pixelDensity - model.length)))), model.key, model)
    }
    function moveSelectedNotesToY(y, model) {
        moveSelectionTo(model.position, Math.max(Math.ceil(clavierViewModel.start), Math.min(Math.round(clavierViewModel.start + (height - y) / clavierViewModel.pixelDensity - 1), Math.floor(clavierViewModel.start + height / clavierViewModel.pixelDensity - 1))), model)
    }
    function moveSelectedNotesOnDragScrolling(directionX, directionY, model) {
        let alignedTick = directionX < 0 ? Math.min(model.position, locator.alignTickCeil(locator.mapToTick(0))) : directionX > 0 ? Math.max(model.position, locator.alignTickFloor(locator.mapToTick(width) - model.length)) : model.position
        let key = directionY > 0 ? Math.ceil(clavierViewModel.start) : directionY < 0 ? Math.floor(clavierViewModel.start + height / clavierViewModel.pixelDensity - 1) : model.key
        moveSelectionTo(alignedTick, key, model)
    }
    function extendSelectionLeftTo(position, model, unitedExtendItem, unitedExtendRestrict) {
        if (position !== model.position) {
            let deltaPosition = position - model.position
            for (let note of noteSequenceViewModel.handle.selection) {
                if (note.position + deltaPosition < 0)
                    return
                if (deltaPosition > note.length - timeLayoutViewModel.positionAlignment)
                    return
                if (note.position + deltaPosition > timeViewModel.end)
                    timeViewModel.end = note.position + deltaPosition
            }
            if (unitedExtendRestrict) {
                let note = noteSequenceViewModel.handle.selection[0]
                let previousNote = noteSequenceViewModel.handle.previousItem(note)
                if (previousNote === unitedExtendItem && previousNote.position + previousNote.length === note.position && previousNote.length + deltaPosition <= unitedExtendRestrict && previousNote.length + deltaPosition >= timeLayoutViewModel.positionAlignment) {
                    previousNote.length += deltaPosition
                }
            }
            for (let note of noteSequenceViewModel.handle.selection) {
                note.position += deltaPosition
                note.length -= deltaPosition
            }
        }
    }
    function extendSelectionRightTo(position, model, unitedExtendItem, unitedExtendRestrict) {
        if (position !== model.position + model.length) {
            let deltaPosition = position - (model.position + model.length)
            for (let note of noteSequenceViewModel.handle.selection) {
                if (note.position + note.length + deltaPosition < 0)
                    return
                if (note.position + note.length + deltaPosition < note.position + timeLayoutViewModel.positionAlignment)
                    return
                if (note.position + note.length + deltaPosition > timeViewModel.end)
                    timeViewModel.end = note.position + note.length + deltaPosition
            }
            if (unitedExtendRestrict) {
                let note = noteSequenceViewModel.handle.selection[0]
                let nextNote = noteSequenceViewModel.handle.nextItem(note)
                if (nextNote === unitedExtendItem && nextNote.position === note.position + note.length && nextNote.length - deltaPosition <= unitedExtendRestrict && nextNote.length - deltaPosition >= timeLayoutViewModel.positionAlignment) {
                    nextNote.length -= deltaPosition
                    nextNote.position += deltaPosition
                }
            }
            for (let note of noteSequenceViewModel.handle.selection) {
                note.length += deltaPosition
            }
        }
    }
    function extendSelectedNotesToX(x, model, unitedExtendItem, unitedExtendRestrict, leftEdge) {
        let alignedTick = locator.alignTick(locator.mapToTick(x))
        if (leftEdge)
            extendSelectionLeftTo(alignedTick, model, unitedExtendItem, unitedExtendRestrict)
        else
            extendSelectionRightTo(alignedTick, model, unitedExtendItem, unitedExtendRestrict)
    }
    function extendSelectedNotesOnDragScrolling(directionX, model, unitedExtendItem, unitedExtendRestrict, leftEdge) {
        let alignedTick = directionX < 0 ? locator.alignTickCeil(locator.mapToTick(0)) : locator.alignTickFloor(locator.mapToTick(width))
        if (leftEdge)
            extendSelectionLeftTo(alignedTick, model, unitedExtendItem, unitedExtendRestrict)
        else
            extendSelectionRightTo(alignedTick, model, unitedExtendItem, unitedExtendRestrict)
    }
    function getUnitedExtendRestrict(model, leftEdge) {
        if (leftEdge) {
            if (noteSequenceViewModel.handle.selection.length === 1) {
                let note = model
                let previousNote = noteSequenceViewModel.handle.previousItem(note)
                if (previousNote && previousNote.position + previousNote.length === note.position) {
                    return previousNote
                }
            }
        } else {
            if (noteSequenceViewModel.handle.selection.length === 1) {
                let note = model
                let nextNote = noteSequenceViewModel.handle.nextItem(note)
                if (nextNote && nextNote.position === note.position + note.length) {
                    return nextNote
                }
            }
        }
        return null
    }

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: noteArea.noteSequenceViewModel
    }
    TimeAlignmentPositionLocator {
        id: locator
        anchors.fill: parent
        timeViewModel: noteArea.timeViewModel
        timeLayoutViewModel: noteArea.timeLayoutViewModel
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
        clavierViewModel: pianoRoll.clavierViewModel
        animationViewModel: pianoRoll.animationViewModel
    }
    Item {
        id: viewport
        x: noteArea.viewport.x
        y: noteArea.viewport.y
        width: noteArea.viewport.width
        height: noteArea.viewport.height

        SequenceSlicer {
            model: noteArea.noteSequenceViewModel
            timeViewModel: noteArea.timeViewModel
            timeLayoutViewModel: noteArea.timeLayoutViewModel
            sliceWidth: noteArea.width
            delegate: Rectangle {
                id: noteRect
                required property QtObject model
                property bool current: {current = model === noteArea.noteSequenceViewModel.handle.currentItem}
                property bool editing: popup.opened
                property bool editingRequired: {editingRequired = (noteArea.pianoRollNoteAreaBehaviorViewModel?.editing ?? false) && current}
                property QtObject noteStyleItem: {noteStyleItem = noteArea.stylesheet.pianoRollNoteArea.createObject(noteRect, {noteViewModel: model, current})}
                Binding {
                    when: noteRect.visible
                    noteRect.x: noteRect.model.position * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                    noteRect.y: (127 - noteRect.model.key) * (noteArea.clavierViewModel?.pixelDensity ?? 0)
                    noteRect.width: noteRect.model.length * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                    noteRect.height: (noteArea.clavierViewModel?.pixelDensity ?? 0)
                    noteRect.current: noteArea.noteSequenceViewModel && noteRect.model === noteArea.noteSequenceViewModel.handle.currentItem
                    noteRect.noteStyleItem: noteArea.stylesheet.pianoRollNoteArea.createObject(noteArea, {noteViewModel: noteRect.model, current: noteRect.current})
                    noteRect.editingRequired: (noteArea.pianoRollNoteAreaBehaviorViewModel?.editing ?? false) && noteRect.current
                }
                color: noteStyleItem.background
                radius: 4
                clip: true

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
                    id: border
                    anchors.fill: parent
                    color: "transparent"
                    radius: 4
                    border.width: noteRect.model.selected ? 2 : 1
                    opacity: noteRect.model.selected ? 1 : 0.5
                    border.color: noteRect.noteStyleItem.border
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
                    elide: Text.ElideRight
                    color: noteRect.noteStyleItem.foreground
                }
                ItemPopupEdit {
                    id: popup
                    model: noteRect.model
                    containerModel: noteArea.noteSequenceViewModel
                    targetProperty: "lyric"
                    styleItem: noteArea.popupEditStyleItem
                    width: noteRect.width
                    radius: noteRect.radius
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
                    id: pointerMouseArea
                    anchors.fill: parent
                    enabled: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pointer || noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pen
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    property double pressedDeltaX: 0
                    property double pressedDeltaY: 0
                    property bool dragged: false
                    DragScroller {
                        id: dragScroller
                        onMoved: function (deltaX, deltaY) {
                            timeManipulator.moveViewBy(deltaX)
                            clavierManipulator.moveViewBy(deltaY)
                            noteArea.moveSelectedNotesOnDragScrolling(deltaX, deltaY, noteRect.model)
                        }
                    }
                    onPressed: (mouse) => {
                        dragged = false
                        pressedDeltaX = mouse.x
                        pressedDeltaY = mouse.y
                    }
                    onPositionChanged: (mouse) => {
                        if (!dragged) {
                            dragged = true
                            for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                note.intermediate = true
                            }
                        }
                        cursorIndicatorLeftBinding.enabled = true
                        cursorIndicatorLeftBinding.onPositionChanged()
                        cursorIndicatorLeftBinding.onKeyChanged()
                        selectionManipulator.select(noteRect.model, Qt.RightButton, mouse.modifiers)
                        let parentPoint = noteRect.mapToItem(noteArea, mouse.x, mouse.y)
                        let deltaX = 0
                        if (parentPoint.x - pressedDeltaX < 0 && parentPoint.x - pressedDeltaX + noteRect.width > noteArea.width) {
                            deltaX = Math.min(parentPoint.x, 0) || Math.max(parentPoint.x - noteArea.width, 0)
                        } else if (parentPoint.x - pressedDeltaX < 0) {
                            deltaX = Math.min(parentPoint.x, 0)
                        } else if (parentPoint.x - pressedDeltaX + noteRect.width > noteArea.width) {
                            deltaX = Math.max(parentPoint.x - noteArea.width, 0)
                        } else {
                            deltaX = 0
                        }
                        dragScroller.determine(deltaX, 0, parentPoint.y - pressedDeltaY - height, noteArea.height - height, (triggeredX, triggeredY) => {
                            if (!triggeredX) {
                                noteArea.moveSelectedNotesToX(parentPoint.x - pressedDeltaX, noteRect.model)
                            }
                            if (!triggeredY) {
                                noteArea.moveSelectedNotesToY(parentPoint.y - pressedDeltaY, noteRect.model)
                            }
                        })
                    }
                    onReleased: canceled()
                    onCanceled: {
                        if (dragged) {
                            for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                note.intermediate = false
                            }
                        }
                        dragScroller.running = false
                        cursorIndicatorLeftBinding.enabled = false
                        noteArea.timeLayoutViewModel.cursorPosition = -1
                        noteArea.clavierViewModel.cursorPosition = -1
                    }
                    onClicked: (mouse) => {
                        if (!dragged)
                            selectionManipulator.select(model, mouse.button, mouse.modifiers)
                        if (mouse.button === Qt.RightButton) {

                        }
                    }
                    onDoubleClicked: (mouse) => {
                        if (mouse.button === Qt.LeftButton) {
                            noteArea.noteSequenceViewModel.handle.currentItem = noteRect.model
                            noteArea.pianoRollNoteAreaBehaviorViewModel.editing = true
                        }
                    }
                }
                Repeater {
                    model: 2
                    MouseArea {
                        id: edgeMouseArea
                        required property int index
                        readonly property bool leftEdge: index
                        anchors.top: parent.top
                        anchors.left: leftEdge ? parent.left : undefined
                        anchors.right: leftEdge ? undefined : parent.right
                        anchors.bottom: parent.bottom
                        width: 2
                        enabled: pointerMouseArea.enabled
                        cursorShape: Qt.SizeHorCursor
                        property bool dragged: false
                        property int unitedExtendRestrict: 0
                        property QtObject unitedExtendItem: null
                        DragScroller {
                            id: edgeDragScroller
                            onMoved: function (deltaX) {
                                timeManipulator.moveViewBy(deltaX)
                                noteArea.extendSelectedNotesOnDragScrolling(deltaX, noteRect.model, parent.unitedExtendItem, parent.unitedExtendRestrict, parent.leftEdge)
                            }
                        }
                        onPressed: (mouse) => {
                            dragged = false
                            let united = (Boolean(mouse.modifiers & Qt.ShiftModifier) !== Boolean(noteArea.pianoRollNoteAreaBehaviorViewModel.unitedExtend))
                            if (united) {
                                unitedExtendItem = noteArea.getUnitedExtendRestrict(noteRect.model, leftEdge)
                                unitedExtendRestrict = unitedExtendItem.length
                            } else {
                                unitedExtendItem = null
                                unitedExtendRestrict = 0
                            }
                        }
                        onPositionChanged: (mouse) => {
                            if (!dragged) {
                                dragged = true
                                for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                    note.intermediate = true
                                }
                            }
                            if (leftEdge) {
                                cursorIndicatorLeftBinding.enabled = true
                                cursorIndicatorLeftBinding.onPositionChanged()
                                cursorIndicatorLeftBinding.onKeyChanged()
                            } else {
                                cursorIndicatorRightBinding.enabled = true
                                cursorIndicatorRightBinding.onPositionChanged()
                                cursorIndicatorRightBinding.onKeyChanged()
                            }
                            selectionManipulator.select(noteRect.model, Qt.RightButton, unitedExtendRestrict ? (mouse.modifiers & ~Qt.ShiftModifier) : mouse.modifiers)
                            let parentX = mapToItem(noteArea, mouse.x, mouse.y).x
                            edgeDragScroller.determine(parentX, noteArea.width, 0, 0, (triggered) => {
                                if (!triggered) {
                                    noteArea.extendSelectedNotesToX(parentX, noteRect.model, unitedExtendItem, unitedExtendRestrict, leftEdge)
                                }
                            })
                        }
                        onReleased: canceled()
                        onCanceled: {
                            if (dragged) {
                                for (let note of noteArea.noteSequenceViewModel.handle.selection) {
                                    note.intermediate = false
                                }
                            }
                            edgeDragScroller.running = false
                            if (leftEdge) {
                                cursorIndicatorLeftBinding.enabled = false
                            } else {
                                cursorIndicatorRightBinding.enabled = false
                            }
                            noteArea.timeLayoutViewModel.cursorPosition = -1
                            noteArea.clavierViewModel.cursorPosition = -1
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
            }
        }


    }
}