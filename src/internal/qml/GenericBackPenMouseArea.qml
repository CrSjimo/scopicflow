import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: backPenMouseArea
    anchors.fill: parent
    cursorShape: Qt.UpArrowCursor // TODO pen cursor
    property int pressedPosition: 0
    property double pressedX: 0
    property int pressedMappedYObject: 0
    property bool dragged: false
    property QtObject item: null
    property bool lengthHintModified: false

    required property QtObject sequenceViewModel
    required property QtObject paneItem
    required property Component viewModelComponent
    required property int lengthHint
    required property var mapY
    required property string mappedYProperty

    DragScroller {
        id: penDragScroller
        onMoved: function (deltaX) {
            timeManipulator.moveViewBy(deltaX)
            parent.handlePositionChanged(deltaX > 0 ? parent.mapFromItem(parent.paneItem, parent.paneItem.width, 0).x : parent.mapFromItem(parent.paneItem, 0, 0).x)
        }
    }
    function handlePositionChanged(x) {
        let length = timeLocator.alignTick(timeLocator.mapToTick(mapToItem(paneItem, x - pressedX, 0).x))
        if (!item) {
            let realLength = length || lengthHint
            if (realLength) {
                item = viewModelComponent.createObject(null, {
                    position: pressedPosition,
                    length: realLength,
                    intermediate: true,
                    selected: true,
                    [mappedYProperty]: pressedMappedYObject
                })
                sequenceViewModel.handle.insertItem(item)
                sequenceViewModel.handle.currentItem = item
            }
        } else {
            if (!lengthHintModified && !length)
                return
            length = Math.max(timeLocator.timeLayoutViewModel.positionAlignment, length)
            item.length = length
            lengthHintModified = true
        }
    }
    onPressed: (mouse) => {
        dragged = false
        item = null
        pressedX = mouse.x
        pressedPosition = timeLocator.alignTick(timeLocator.mapToTick(mapToItem(paneItem, mouse.x, 0).x))
        pressedMappedYObject = mapY(mouse.y)
        lengthHintModified = !lengthHint
    }
    onPositionChanged: (mouse) => {
        if (!dragged)
            dragged = true
        if (!item)
            selectionManipulator.select(null, mouse.button, mouse.modifiers)
        let parentX = mapToItem(paneItem, mouse.x, 0).x
        penDragScroller.determine(parentX, paneItem.width, 0, 0, (triggered) => {
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
            sequenceViewModel.handle.removeItem(item)
            item = null
        }
    }
    onDoubleClicked: (mouse) => {
        if (!lengthHint)
            return
        let item = viewModelComponent.createObject(null, {
            position: pressedPosition,
            length: lengthHint,
            [mappedYProperty]: pressedMappedYObject
        })
        sequenceViewModel.handle.insertItem(item)
        selectionManipulator.select(item, mouse.button, mouse.modifiers)
    }
}