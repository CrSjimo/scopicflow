import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

DispatchedDragHandler {
    id: handler

    enum Mode {
        Normal,
        TimeRange
    }

    property var controller: null
    property TimeManipulator timeManipulator: null
    property QtObject verticalManipulator: null
    property RubberBandLayer rubberBandLayer: null
    property SelectionController selectionController: null
    property Item target: parent
    property int mode: RubberBandDragHandler.Normal

    onDragStarted: () => {
        controller.rubberBandDraggingStarted(target)
        selectionController.selectByMouse(null, Qt.LeftButton, modifiers)
        let p = mapToItem(rubberBandLayer, startPoint)
        if (mode === RubberBandDragHandler.Normal) {
            rubberBandLayer.startSelection(p)
        } else if (mode === RubberBandDragHandler.TimeRange) {
            rubberBandLayer.startSelection(Qt.point(p.x, 0))
        }

    }

    onDragMoved: (x, y) => {
        dragScroller.determine(x, width, y, height, (triggeredX, triggeredY) => {
            if (!triggeredX) {
                dragScroller.doDragRubberBand(Qt.point(x, dragScroller.lastTargetPoint.y))
            }
            if (!triggeredY) {
                dragScroller.doDragRubberBand(Qt.point(dragScroller.lastTargetPoint.x, y))
            }
        })
    }

    onDragFinished: () => {
        rubberBandLayer.endSelection()
        dragScroller.running = false
        controller.rubberBandDraggingCommitted(target)
    }

    onDragCanceled: () => {
        rubberBandLayer.endSelection(true)
        dragScroller.running = false
        controller.rubberBandDraggingAborted(target)
    }

    DragScroller {
        id: dragScroller

        property point lastTargetPoint: Qt.point(0, 0)
        function doDragRubberBand(targetPoint) {
            let p = mapToItem(handler.rubberBandLayer, targetPoint)
            if (handler.mode === RubberBandDragHandler.Normal) {
                handler.rubberBandLayer.updateSelection(p)
            } else if (handler.mode === RubberBandDragHandler.TimeRange) {
                handler.rubberBandLayer.updateSelection(Qt.point(p.x, handler.height))
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
                doDragRubberBand(Qt.point(deltaX > 0 ? handler.width : 0, lastTargetPoint.y))
            }
            if (deltaY !== 0) {
                doDragRubberBand(Qt.point(lastTargetPoint.x, deltaY > 0 ? handler.height : 0))
            }
        }
    }

}