import QtQml
import QtQuick

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

    function moveSelectionTo(position, key, model) {
        if (position !== model.position) {
            let deltaPosition = position - model.position
            for (let note of noteSequenceViewModel.handle.selection) {
                if (note.position + deltaPosition < 0)
                    return
                if (note.position + deltaPosition > timeViewModel.end)
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
        moveSelectionTo(Math.max(locator.alignTickCeil(timeViewModel.start), Math.min(locator.alignTick(locator.mapToTick(x)), locator.alignTickFloor(timeViewModel.start + width / timeLayoutViewModel.pixelDensity - model.length))), model.key, model)
    }
    function moveSelectedNotesToY(y, model) {
        moveSelectionTo(model.position, Math.max(Math.ceil(clavierViewModel.start), Math.min(Math.round(clavierViewModel.start + (height - y) / clavierViewModel.pixelDensity - 1), Math.floor(clavierViewModel.start + height / clavierViewModel.pixelDensity - 1))), model)
    }
    function moveSelectedNotesOnDragScrolling(directionX, directionY, model) {
        let alignedTick = directionX < 0 ? locator.alignTickCeil(locator.mapToTick(0)) : directionX > 0 ? locator.alignTickFloor(locator.mapToTick(width) - model.length) : locator.alignTick(model.position)
        let key = directionY > 0 ? Math.ceil(clavierViewModel.start) : directionY < 0 ? Math.floor(clavierViewModel.start + height / clavierViewModel.pixelDensity - 1) : model.key
        moveSelectionTo(alignedTick, key, model)
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
                property bool current: false
                property QtObject noteStyleItem: {noteStyleItem = noteArea.stylesheet.pianoRollNoteArea.createObject(noteRect, {noteViewModel: model, current})}
                Binding {
                    when: noteRect.visible
                    noteRect.x: noteRect.model.position * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                    noteRect.y: (127 - noteRect.model.key) * (noteArea.clavierViewModel?.pixelDensity ?? 0)
                    noteRect.width: noteRect.model.length * (noteArea.timeLayoutViewModel?.pixelDensity ?? 0)
                    noteRect.height: (noteArea.clavierViewModel?.pixelDensity ?? 0)
                    noteRect.current: model === noteArea.noteSequenceViewModel.handle.currentItem
                    noteRect.noteStyleItem: noteArea.stylesheet.pianoRollNoteArea.createObject(noteArea, {noteViewModel: noteRect.model, current: noteRect.current})
                }
                color: noteStyleItem.background
                radius: 4

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
                    anchors.verticalCenter: parent.verticalCenter
                    text: noteRect.model.lyric
                    visible: noteRect.width >= width + 8
                    color: noteRect.noteStyleItem.foreground
                }
                MouseArea {
                    id: pointerMouseArea
                    anchors.fill: parent
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
                    Connections {
                        id: cursorIndicatorBinding
                        target: noteRect.model
                        enabled: false
                        function onPositionChanged() {
                            noteArea.timeLayoutViewModel.cursorPosition = noteRect.model.position
                        }
                        function onKeyChanged() {
                            noteArea.clavierViewModel.cursorPosition = noteRect.model.key
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
                        cursorIndicatorBinding.enabled = true
                        cursorIndicatorBinding.onPositionChanged()
                        cursorIndicatorBinding.onKeyChanged()
                        selectionManipulator.select(noteRect.model, Qt.RightButton, mouse.modifiers)
                        let parentPoint = noteRect.mapToItem(noteArea, mouse.x, mouse.y)
                        dragScroller.determine(parentPoint.x, noteArea.width - width, parentPoint.y - height, noteArea.height - height, (triggeredX, triggeredY) => {
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
                        cursorIndicatorBinding.enabled = false
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

                        }
                    }
                }

            }
        }


    }
}