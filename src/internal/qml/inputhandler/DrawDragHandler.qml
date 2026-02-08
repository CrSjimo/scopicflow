import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

DispatchedDragHandler {
    id: handler

    property var controller: null
    property TimeManipulator timeManipulator: null
    property QtObject verticalManipulator: null
    property SelectionController selectionController: null
    property Item target: parent
    property int initialPosition: 0
    property int initialYIndex: 0
    property var viewModel: null
    property Item viewportContainer: null

    signal createViewModelRequested(position: int, yIndex: int)
    signal updateViewModelRequested(length: int, yIndex: int)

    onDragStarted: () => {
        controller.drawingStarted(target)
        selectionController.selectByMouse(null, Qt.LeftButton, modifiers)
        dragScroller.lastTargetPoint = startPoint
        initialPosition = handler.timeManipulator.alignPosition(handler.timeManipulator.mapToPosition(startPoint.x), ScopicFlow.AO_Visible)
        initialYIndex = handler.verticalManipulator?.mapToPosition(startPoint.y) ?? 0
    }

    onDragMoved: (x, y) => {
        dragScroller.determine(x, width, y, height, (triggeredX, triggeredY) => {
            if (!triggeredX) {
                dragScroller.draw(Qt.point(x, dragScroller.lastTargetPoint.y))
            }
            if (!triggeredY) {
                dragScroller.draw(Qt.point(dragScroller.lastTargetPoint.x, y))
            }
        })
    }

    onDragFinished: () => {
        dragScroller.running = dragScroller.drawingStarted = dragScroller.realDrawingStarted = false
        if (viewModel) {
            controller.drawingCommitted(target)
        } else {
            controller.drawingAborted(target)
        }
        viewModel = null
    }

    onDragCanceled: () => {
        dragScroller.running = dragScroller.drawingStarted = dragScroller.realDrawingStarted = false
        controller.drawingAborted(target)
        viewModel = null
    }

    DragScroller {
        id: dragScroller

        property point lastTargetPoint: Qt.point(0, 0)
        property bool drawingStarted: false
        property bool realDrawingStarted: false
        function draw(targetPoint) {
            if (!drawingStarted) {
                if (targetPoint.x - handler.startPoint.x < 4) {
                    return
                }
            }
            drawingStarted = true
            let position = handler.timeManipulator.alignPosition(handler.timeManipulator.mapToPosition(targetPoint.x), ScopicFlow.AO_Visible)
            let yIndex = handler.verticalManipulator?.mapToPosition(targetPoint.y) ?? 0
            let length = Math.max(realDrawingStarted ? handler.timeManipulator.timeLayoutViewModel.positionAlignment : 0, position - initialPosition)
            if (!handler.viewModel) {
                handler.createViewModelRequested(initialPosition, initialYIndex)
            } else {
                if (length > 0) {
                    realDrawingStarted = true
                    handler.updateViewModelRequested(length, yIndex)
                }
            }
            lastTargetPoint = targetPoint
        }

        onMoved: (deltaX, deltaY) => {
            if (handler.timeManipulator) {
                handler.timeManipulator.moveViewBy(deltaX)
            }
            if (handler.verticalManipulator) {
                handler.verticalManipulator.moveViewBy(deltaY)
            }
            if (deltaX !== 0) {
                draw(Qt.point(deltaX > 0 ? handler.width : 0, lastTargetPoint.y))
            }
            if (deltaY !== 0) {
                draw(Qt.point(lastTargetPoint.x, deltaY > 0 ? handler.height : 0))
            }
        }
    }

}