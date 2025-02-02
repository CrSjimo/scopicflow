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

    function ensureCurrentItemVisible() {
        if (noteSequenceViewModel?.handle.currentItem && pianoRollNoteAreaBehaviorViewModel?.editing) {
            timeManipulator.ensureVisible(noteSequenceViewModel.handle.currentItem.position, noteSequenceViewModel.handle.currentItem.length)
        }
    }

    Component {
        id: noteViewModelComponent
        NoteViewModel {

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
        moveSelectionTo(Math.max(Math.min(timeLocator.alignTickCeil(timeViewModel.start), model.position), Math.min(timeLocator.alignTick(timeLocator.mapToTick(x)), Math.max(model.position, timeLocator.alignTickFloor(timeViewModel.start + width / timeLayoutViewModel.pixelDensity - model.length)))), model.key, model)
    }
    function moveSelectedNotesToY(y, model) {
        moveSelectionTo(model.position, Math.max(Math.ceil(clavierViewModel.start), Math.min(Math.round(clavierViewModel.start + (height - y) / clavierViewModel.pixelDensity - 1), Math.floor(clavierViewModel.start + height / clavierViewModel.pixelDensity - 1))), model)
    }
    function moveSelectedNotesOnDragScrolling(directionX, directionY, model) {
        let alignedTick = directionX < 0 ? Math.min(model.position, timeLocator.alignTickCeil(timeLocator.mapToTick(0))) : directionX > 0 ? Math.max(model.position, timeLocator.alignTickFloor(timeLocator.mapToTick(width) - model.length)) : model.position
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
        let alignedTick = timeLocator.alignTick(timeLocator.mapToTick(x))
        if (leftEdge)
            extendSelectionLeftTo(alignedTick, model, unitedExtendItem, unitedExtendRestrict)
        else
            extendSelectionRightTo(alignedTick, model, unitedExtendItem, unitedExtendRestrict)
    }
    function extendSelectedNotesOnDragScrolling(directionX, model, unitedExtendItem, unitedExtendRestrict, leftEdge) {
        let alignedTick = directionX < 0 ? timeLocator.alignTickCeil(timeLocator.mapToTick(0)) : timeLocator.alignTickFloor(timeLocator.mapToTick(width))
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
        clavierViewModel: pianoRoll.clavierViewModel
        animationViewModel: pianoRoll.animationViewModel
    }
    PianoRollNoteAreaRubberBandHelper {
        id: rubberBandHelper
        noteSequenceViewModel: selectionManipulator.viewModel
        rubberBandLayer: rubberBandLayer
        function viewportPointToRubberBandPoint(p) {
            return Qt.point(p.x / noteArea.timeLayoutViewModel.pixelDensity, (viewport.height - p.y) / noteArea.clavierViewModel.pixelDensity)
        }
        function rubberBandPointToViewportPoint(p) {
            if (!noteArea.timeLayoutViewModel)
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
            id: backPointerMouseArea
            anchors.fill: parent
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pointer
            property bool dragged: false
            property double pressedX: 0
            property double pressedY: 0
            property point lastTargetPoint: Qt.point(0, 0)
            function doDragRubberBand(targetPoint) {
                rubberBandLayer.updateSelection(rubberBandHelper.viewportPointToRubberBandPoint(targetPoint))
                lastTargetPoint = targetPoint
            }
            DragScroller {
                id: rubberBandDragScroller
                onMoved: (deltaX, deltaY) => {
                    timeManipulator.moveViewBy(deltaX)
                    clavierManipulator.moveViewBy(deltaY)
                    if (deltaX !== 0) {
                        parent.doDragRubberBand(Qt.point(noteArea.mapToItem(viewport, deltaX > 0 ? noteArea.width : 0, 0).x, parent.lastTargetPoint.y))
                    }
                    if (deltaY !== 0) {
                        parent.doDragRubberBand(Qt.point(parent.lastTargetPoint.x, noteArea.mapToItem(viewport,0, deltaY > 0 ? noteArea.height : 0).y))
                    }
                }
            }
            onPressed: (mouse) => {
                dragged = false
                pressedX = mouse.x
                pressedY = mouse.y
            }
            onPositionChanged: (mouse) => {
                dragged = true
                if (!rubberBandLayer.started) {
                    selectionManipulator.select(null, Qt.RightButton, mouse.modifiers)
                    rubberBandLayer.startSelection(rubberBandHelper.viewportPointToRubberBandPoint(Qt.point(pressedX, pressedY)))
                }
                let parentPoint = viewport.mapToItem(noteArea, mouse.x, mouse.y)
                rubberBandDragScroller.determine(parentPoint.x, noteArea.width, parentPoint.y, noteArea.height, (triggeredX, triggeredY) => {
                    if (!triggeredX) {
                        doDragRubberBand(Qt.point(mouse.x, lastTargetPoint.y))
                    }
                    if (!triggeredY) {
                        doDragRubberBand(Qt.point(lastTargetPoint.x, mouse.y))
                    }
                })
            }
            onReleased: canceled()
            onCanceled: () => {
                rubberBandLayer.endSelection()
                rubberBandDragScroller.running = false
            }
            onClicked: (mouse) => {
                if (mouse.button === Qt.RightButton) {

                } else {
                    if (!dragged) {
                        selectionManipulator.select(null, mouse.button, mouse.modifiers)
                    }
                }

            }
        }

        MouseArea {
            id: backPenMouseArea
            anchors.fill: parent
            visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pen
            cursorShape: Qt.UpArrowCursor // TODO pen cursor
            property int pressedPosition: 0
            property double pressedX: 0
            property int pressedKey: 0
            property bool dragged: false
            property QtObject item: null
            property bool lengthHintModified: false
            DragScroller {
                id: penDragScroller
                onMoved: function (deltaX) {
                    timeManipulator.moveViewBy(deltaX)
                    parent.handlePositionChanged(deltaX > 0 ? parent.mapFromItem(noteArea, noteArea.width, 0).x : parent.mapFromItem(noteArea, 0, 0).x)
                }
            }
            function handlePositionChanged(x) {
                let length = timeLocator.alignTick(timeLocator.mapToTick(mapToItem(noteArea, x - pressedX, 0).x))
                if (!item) {
                    let realLength = length || noteArea.pianoRollNoteAreaBehaviorViewModel.lengthHint
                    if (realLength) {
                        item = noteViewModelComponent.createObject(null, {
                            position: pressedPosition,
                            length: realLength,
                            key: pressedKey,
                            intermediate: true,
                            selected: true
                        })
                        noteArea.noteSequenceViewModel.handle.insertItem(item)
                        noteArea.noteSequenceViewModel.handle.currentItem = item
                    }
                } else {
                    if (!lengthHintModified && !length)
                        return
                    length = Math.max(noteArea.timeLayoutViewModel.positionAlignment, length)
                    item.length = length
                    lengthHintModified = true
                }
            }
            onPressed: (mouse) => {
                dragged = false
                item = null
                pressedX = mouse.x
                pressedPosition = timeLocator.alignTick(timeLocator.mapToTick(mapToItem(noteArea, mouse.x, 0).x))
                pressedKey = clavierLocator.mapToKey(mapToItem(noteArea, 0, mouse.y).y)
                lengthHintModified = !noteArea.pianoRollNoteAreaBehaviorViewModel.lengthHint
            }
            onPositionChanged: (mouse) => {
                if (!dragged)
                    dragged = true
                if (!item)
                    selectionManipulator.select(null, mouse.button, mouse.modifiers)
                let parentX = mapToItem(noteArea, mouse.x, 0).x
                penDragScroller.determine(parentX, noteArea.width, 0, 0, (triggered) => {
                    if (!triggered)
                        handlePositionChanged(mouse.x)
                })
            }
            onReleased: (mouse) => {
                penDragScroller.running = false
                if (item) {
                    item.intermediate = false
                    item = null
                }
            }
            onCanceled: () => {
                penDragScroller.running = false
                if (item) {
                    noteArea.noteSequenceViewModel.handle.removeItem(item)
                    item = null
                }
            }
            onClicked: (mouse) => {
                if (dragged)
                    return
                backPointerMouseArea.clicked(mouse)
            }
            onDoubleClicked: (mouse) => {
                if (!noteArea.pianoRollNoteAreaBehaviorViewModel.lengthHint)
                    return
                let item = noteViewModelComponent.createObject(null, {
                    position: pressedPosition,
                    length: noteArea.pianoRollNoteAreaBehaviorViewModel.lengthHint,
                    key: pressedKey,
                })
                noteArea.noteSequenceViewModel.handle.insertItem(item)
                selectionManipulator.select(item, mouse.button, mouse.modifiers)
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
                    property bool willBeErased: false
                    opacity: willBeErased ? 0.5 : 1
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
                        id: pointerMouseArea
                        anchors.fill: parent
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pointer || noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Pen
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
                            noteRect.bringToFront()
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
                            visible: pointerMouseArea.visible
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
                                noteRect.bringToFront()
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
                    MouseArea {
                        id: scissorMouseArea
                        anchors.fill: parent
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Scissor
                        cursorShape: Qt.UpArrowCursor // TODO scissor cursor
                        property int pressedPosition: 0
                        property double pressedY
                        property bool dragged: false
                        onPressed: (mouse) => {
                            pressedPosition = Math.max(noteRect.model.position + noteArea.timeLayoutViewModel.positionAlignment, Math.min(timeLocator.alignTick(timeLocator.mapToTick(mapToItem(noteArea, mouse.x, 0).x)), noteRect.model.position + noteRect.model.length - noteArea.timeLayoutViewModel.positionAlignment))
                            pressedY = mouse.y
                            dragged = false
                            noteRect.bringToFront()
                        }
                        onPositionChanged: (mouse) => {
                            if (Math.abs(mouse.y - pressedY) > 8 && pressedPosition !== noteRect.model.position + noteRect.model.length) {
                                dragged = true
                                noteArea.timeLayoutViewModel.cursorPosition = pressedPosition
                            } else {
                                dragged = false
                                noteArea.timeLayoutViewModel.cursorPosition = -1
                            }

                        }
                        onReleased: (mouse) => {
                            if (!dragged)
                                return
                            noteArea.timeLayoutViewModel.cursorPosition = -1
                            // TODO emit signal
                        }
                        onCanceled: () => {
                            noteArea.timeLayoutViewModel.cursorPosition = -1
                        }
                    }
                    MouseArea {
                        id: eraserMouseArea
                        anchors.fill: parent
                        visible: noteArea.pianoRollNoteAreaBehaviorViewModel?.mouseBehavior === PianoRollNoteAreaBehaviorViewModel.Eraser
                        cursorShape: Qt.CrossCursor // TODO erasor cursor
                        property double pressedX: 0
                        property double pressedY: 0
                        property bool dragged: false
                        onPressed: (mouse) => {
                            pressedX = mouse.x
                            pressedY = mouse.y
                            dragged = false
                            noteRect.bringToFront()
                        }
                        onPositionChanged: (mouse) => {
                            if (Math.abs(mouse.x - pressedX) > 8 || Math.abs(mouse.y - pressedY) > 8) {
                                noteRect.willBeErased = dragged = true
                            } else {
                                noteRect.willBeErased = dragged = false
                            }
                        }
                        onReleased: (mouse) => {
                            if (!dragged)
                                return
                            let model = noteRect.model
                            noteArea.noteSequenceViewModel.handle.removeItem(model)
                        }
                        onCanceled: () => {
                            noteRect.willBeErased = false
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