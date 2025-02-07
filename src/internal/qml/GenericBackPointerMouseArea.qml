import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: backPointerMouseArea
    anchors.fill: parent
    property bool dragged: false
    property double pressedX: 0
    property double pressedY: 0
    property point lastTargetPoint: Qt.point(0, 0)

    required property Item paneItem
    property QtObject verticalManipulator: null

    signal rubberBandStartRequired(p: point)
    signal rubberBandUpdateRequired(p: point)

    function doDragRubberBand(targetPoint) {
        rubberBandUpdateRequired(targetPoint)
        lastTargetPoint = targetPoint
    }
    DragScroller {
        id: rubberBandDragScroller
        onMoved: (deltaX, deltaY) => {
            timeManipulator.moveViewBy(deltaX)
            if (backPointerMouseArea.verticalManipulator)
                backPointerMouseArea.verticalManipulator.moveViewBy(deltaY)
            if (deltaX !== 0) {
                parent.doDragRubberBand(Qt.point(backPointerMouseArea.paneItem.mapToItem(backPointerMouseArea, deltaX > 0 ? backPointerMouseArea.paneItem.width : 0, 0).x, parent.lastTargetPoint.y))
            }
            if (deltaY !== 0) {
                parent.doDragRubberBand(Qt.point(parent.lastTargetPoint.x, backPointerMouseArea.paneItem.mapToItem(backPointerMouseArea,0, deltaY > 0 ? backPointerMouseArea.paneItem.height : 0).y))
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
            rubberBandStartRequired(Qt.point(pressedX, pressedY))
        }
        let parentPoint = viewport.mapToItem(paneItem, mouse.x, mouse.y)
        rubberBandDragScroller.determine(parentPoint.x, paneItem.width, parentPoint.y, paneItem.height, (triggeredX, triggeredY) => {
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
        if (!dragged) {
            selectionManipulator.select(null, mouse.button, mouse.modifiers)
        }
    }
}