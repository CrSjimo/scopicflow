import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: edgeMouseArea
    required property bool leftEdge
    required property bool unitedExtend
    required property bool unitedExtendEnabled
    required property QtObject model
    required property QtObject sequenceViewModel
    required property Item paneItem

    readonly property bool dragging: dragged & pressed
    
    anchors.top: parent.top
    anchors.left: leftEdge ? parent.left : undefined
    anchors.right: leftEdge ? undefined : parent.right
    anchors.bottom: parent.bottom
    width: 4
    cursorShape: Qt.SizeHorCursor
    property bool dragged: false
    property int unitedExtendRestrict: 0
    property QtObject unitedExtendItem: null

    function getUnitedExtendRestrict() {
        if (leftEdge) {
            if (sequenceViewModel.handle.selection.length === 1) {
                let note = model
                let previousNote = sequenceViewModel.handle.previousItem(note)
                if (previousNote && previousNote.position + previousNote.length === note.position) {
                    return previousNote
                }
            }
        } else {
            if (sequenceViewModel.handle.selection.length === 1) {
                let note = model
                let nextNote = sequenceViewModel.handle.nextItem(note)
                if (nextNote && nextNote.position === note.position + note.length) {
                    return nextNote
                }
            }
        }
        return null
    }

    function extendSelectionLeftTo(position) {
        if (position !== model.position) {
            let deltaPosition = position - model.position
            for (let note of sequenceViewModel.handle.selection) {
                if (note.position + deltaPosition < 0)
                    return
                if (deltaPosition > note.length - timeLayoutViewModel.positionAlignment)
                    return
                if (note.position + deltaPosition > timeViewModel.end)
                    timeViewModel.end = note.position + deltaPosition
            }
            if (unitedExtendRestrict) {
                let note = sequenceViewModel.handle.selection[0]
                let previousNote = sequenceViewModel.handle.previousItem(note)
                if (previousNote === unitedExtendItem && previousNote.position + previousNote.length === note.position && previousNote.length + deltaPosition <= unitedExtendRestrict && previousNote.length + deltaPosition >= timeLayoutViewModel.positionAlignment) {
                    previousNote.length += deltaPosition
                }
            }
            for (let note of sequenceViewModel.handle.selection) {
                note.position += deltaPosition
                note.length -= deltaPosition
            }
        }
    }
    function extendSelectionRightTo(position) {
        if (position !== model.position + model.length) {
            let deltaPosition = position - (model.position + model.length)
            for (let note of sequenceViewModel.handle.selection) {
                if (note.position + note.length + deltaPosition < 0)
                    return
                if (note.position + note.length + deltaPosition < note.position + timeLayoutViewModel.positionAlignment)
                    return
                if (note.position + note.length + deltaPosition > timeViewModel.end)
                    timeViewModel.end = note.position + note.length + deltaPosition
            }
            if (unitedExtendRestrict) {
                let note = sequenceViewModel.handle.selection[0]
                let nextNote = sequenceViewModel.handle.nextItem(note)
                if (nextNote === unitedExtendItem && nextNote.position === note.position + note.length && nextNote.length - deltaPosition <= unitedExtendRestrict && nextNote.length - deltaPosition >= timeLayoutViewModel.positionAlignment) {
                    nextNote.length -= deltaPosition
                    nextNote.position += deltaPosition
                }
            }
            for (let note of sequenceViewModel.handle.selection) {
                note.length += deltaPosition
            }
        }
    }
    function extendSelectedNotesToX(x) {
        let alignedTick = Math.max(timeLocator.alignTickCeil(timeLocator.mapToTick(0)), Math.min(timeLocator.alignTick(timeLocator.mapToTick(x)), timeLocator.alignTickFloor(timeLocator.mapToTick(paneItem.width))))
        if (leftEdge)
            extendSelectionLeftTo(alignedTick)
        else
            extendSelectionRightTo(alignedTick)
    }
    function extendSelectedNotesOnDragScrolling(directionX) {
        let alignedTick = directionX < 0 ? timeLocator.alignTickCeil(timeLocator.mapToTick(0)) : timeLocator.alignTickFloor(timeLocator.mapToTick(paneItem.width))
        if (leftEdge)
            extendSelectionLeftTo(alignedTick)
        else
            extendSelectionRightTo(alignedTick)
    }
    
    DragScroller {
        id: edgeDragScroller
        onMoved: function (deltaX) {
            timeManipulator.moveViewBy(deltaX)
            edgeMouseArea.extendSelectedNotesOnDragScrolling(deltaX)
        }
    }
    onPressed: (mouse) => {
        dragged = false
        let united = (Boolean(mouse.modifiers & Qt.ShiftModifier) !== unitedExtend) && unitedExtendEnabled
        if (united) {
            unitedExtendItem = getUnitedExtendRestrict()
            unitedExtendRestrict = unitedExtendItem.length
        } else {
            unitedExtendItem = null
            unitedExtendRestrict = 0
        }
    }
    onPositionChanged: (mouse) => {
        if (!dragged) {
            dragged = true
            for (let note of sequenceViewModel.handle.selection) {
                note.intermediate = true
            }
        }
        selectionManipulator.select(model, Qt.RightButton, unitedExtendRestrict ? (mouse.modifiers & ~Qt.ShiftModifier) : mouse.modifiers)
        let parentX = mapToItem(paneItem, mouse.x, mouse.y).x
        edgeDragScroller.determine(parentX, paneItem.width, 0, 0, (triggered) => {
            if (!triggered) {
                extendSelectedNotesToX(parentX)
            }
        })
    }
    onReleased: canceled()
    onCanceled: {
        if (dragged) {
            for (let note of sequenceViewModel.handle.selection) {
                note.intermediate = false
            }
        }
        edgeDragScroller.running = false
    }
}