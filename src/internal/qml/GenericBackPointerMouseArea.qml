import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: backPointerMouseArea
    anchors.fill: parent
    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.RightButton
    property bool dragged: false
    property double pressedX: 0
    property double pressedY: 0
    property point lastTargetPoint: Qt.point(0, 0)

    required property Item paneItem
    property QtObject verticalManipulator: null

    property var handleBeforeInteractionNotificationCallback: (..._) => true
    property var emitInteractionNotificationSignalCallback: (..._) => {}

    function sendInteractionNotification(interactionType) {
        if (!handleBeforeInteractionNotification(interactionType))
            return false
        emitInteractionNotificationSignal(interactionType)
        return true
    }
    function handleBeforeInteractionNotification(interactionType) {
        return handleBeforeInteractionNotificationCallback(interactionType)
    }
    function emitInteractionNotificationSignal(interactionType) {
        return emitInteractionNotificationSignalCallback(interactionType)
    }

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
        if (!sendInteractionNotification(ScopicFlow.II_Pressed))
            mouse.accepted = false
    }
    onReleased: () => {
        rubberBandLayer.endSelection()
        rubberBandDragScroller.running = false
        sendInteractionNotification(ScopicFlow.II_Released)
    }
    onCanceled: () => {
        rubberBandLayer.endSelection()
        rubberBandDragScroller.running = false
        sendInteractionNotification(ScopicFlow.II_Canceled)
    }
    onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
    onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
    onClicked: (mouse) => {
        if (mouse.button === Qt.LeftButton && !dragged) {
            if (!handleBeforeInteractionNotification(ScopicFlow.II_Clicked))
                return
            selectionManipulator.select(null, mouse.button, mouse.modifiers)
            emitInteractionNotificationSignal(ScopicFlow.II_Clicked)
        } else if (mouse.button === Qt.RightButton) {
            let interactionType = dragged || (mouse.modifiers & Qt.ControlModifier) ? ScopicFlow.II_ItemContextMenu : ScopicFlow.II_ContextMenu
            if (!handleBeforeInteractionNotification(interactionType))
                return
            selectionManipulator.select(null, mouse.button, mouse.modifiers | (dragged ? Qt.ControlModifier : 0))
            emitInteractionNotificationSignal(interactionType)
        }
    }
    onPositionChanged: (mouse) => {
        if (!pressed)
            return
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
}