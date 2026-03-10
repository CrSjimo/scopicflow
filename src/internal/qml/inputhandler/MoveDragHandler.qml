import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

DispatchedDragHandler {
    id: handler

    property QtObject controller: null
    property Item paneItem: null
    property QtObject viewModel: null
    property TimeManipulator timeManipulator: null
    property QtObject verticalManipulator: null
    property SelectionController selectionController: null
    property int moveFlag: 0

    signal moveSelectionToYRequested(y: double)

    onDragStarted: () => {
        controller.movingStarted(paneItem, viewModel, moveFlag)
        if (selectionController && controller.clickSelectable) {
            selectionController.selectByMouse(viewModel, Qt.RightButton, 0)
        }
    }

    onDragMoved: (x, y) => {
        const parentPoint = mapToItem(paneItem, x, y)
        dragScroller.determine(parentPoint.x, paneItem.width, parentPoint.y, paneItem.height, (triggeredX, triggeredY) => {
            if (!triggeredX) {
                dragScroller.moveSelectionToX(parentPoint.x - startPoint.x)
            }
            if (!triggeredY) {
                moveSelectionToYRequested(parentPoint.y - startPoint.y)
            }
        })
    }

    onDragFinished: () => {
        controller.movingCommitted(paneItem, viewModel)
        dragScroller.running = false
    }

    onDragCanceled: () => {
        controller.movingAborted(paneItem, viewModel)
        dragScroller.running = false
    }

    DragScroller {
        id: dragScroller

        function moveSelectionToX(x) {
            let position = handler.timeManipulator.alignPosition(handler.timeManipulator.mapToPosition(x))
            let deltaPosition = position - handler.viewModel.position
            if (deltaPosition === 0)
                return
            let selection = handler.selectionController.getSelectedItems()
            for (let note of selection) {
                if (note.position + deltaPosition < 0)
                    return
            }
            for (let note of selection) {
                note.position = note.position + deltaPosition
            }
        }

        onMoved: (deltaX, deltaY) => {
            handler.timeManipulator.moveViewBy(deltaX)
            if (handler.verticalManipulator)
                handler.verticalManipulator.moveViewBy(deltaY)
            let parentX = undefined
            if (deltaX > 0)
                parentX = handler.paneItem.width
            else if (deltaX < 0)
                parentX = 0
            let parentY = undefined
            if (deltaY > 0)
                parentY = handler.paneItem.height
            else if (deltaY < 0)
                parentY = 0
            if (parentX !== undefined)
                moveSelectionToX(parentX - handler.startPoint.x)
            if (parentY !== undefined)
                handler.moveSelectionToYRequested(parentY - handler.startPoint.y)
        }
    }
}