import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: pointerMouseArea

    property bool dragged: false
    readonly property bool dragging: dragged & pressed
    property var emitInteractionNotificationSignalCallback: () => {}
    property var handleBeforeInteractionNotificationCallback: () => true
    required property QtObject model
    required property Item paneItem
    property double pressedDeltaX: 0
    property double pressedDeltaY: 0
    required property QtObject sequenceViewModel
    property QtObject transactionControllerNotifier: null
    property QtObject verticalManipulator: null

    signal moveSelectedNotesToY(y: double)

    function emitInteractionNotificationSignal(interactionType) {
        return emitInteractionNotificationSignalCallback(interactionType);
    }
    function handleBeforeInteractionNotification(interactionType) {
        return handleBeforeInteractionNotificationCallback(interactionType);
    }
    function moveSelectedNotesToX(x) {
        moveSelectionTo(timeLocator.alignTick(timeLocator.mapToTick(x)));
    }
    function moveSelectionTo(position) {
        if (position !== model.position) {
            let deltaPosition = position - model.position;
            for (let note of sequenceViewModel.handle.selection) {
                if (note.position + deltaPosition < 0)
                    return;
            }
            for (let note of sequenceViewModel.handle.selection) {
                note.position = note.position + deltaPosition;
            }
        }
    }
    function sendInteractionNotification(interactionType) {
        if (!handleBeforeInteractionNotification(interactionType))
            return false;
        emitInteractionNotificationSignal(interactionType);
        return true;
    }

    anchors.fill: parent
    hoverEnabled: true

    onCanceled: () => {
        if (dragged) {
            for (let note of sequenceViewModel.handle.selection) {
                note.intermediate = false;
            }
            pointerMouseArea.transactionControllerNotifier?.transactionAborted();
        }
        dragScroller.running = false;
        sendInteractionNotification(ScopicFlow.II_Canceled);
    }
    onClicked: mouse => {
        if (!dragged) {
            if (!handleBeforeInteractionNotification(ScopicFlow.II_Clicked))
                return;
            selectionManipulator.select(model, mouse.button, mouse.modifiers);
            emitInteractionNotificationSignal(ScopicFlow.II_Clicked);
        }
    }
    onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
    onPositionChanged: mouse => {
        if (!pressed)
            return;
        if (!dragged) {
            dragged = true;
            pointerMouseArea.transactionControllerNotifier?.transactionAboutToBegin();
            for (let note of sequenceViewModel.handle.selection) {
                note.intermediate = true;
            }
        }
        selectionManipulator.select(model, Qt.RightButton, mouse.modifiers);
        let parentPoint = mapToItem(paneItem, mouse.x, mouse.y);
        dragScroller.determine(parentPoint.x, paneItem.width, parentPoint.y, paneItem.height, (triggeredX, triggeredY) => {
            if (!triggeredX) {
                moveSelectedNotesToX(parentPoint.x - pressedDeltaX);
            }
            if (!triggeredY) {
                moveSelectedNotesToY(parentPoint.y - pressedDeltaY);
            }
        });
    }
    onPressed: mouse => {
        dragged = false;
        pressedDeltaX = mouse.x;
        pressedDeltaY = mouse.y;
        if (!sendInteractionNotification(ScopicFlow.II_Pressed))
            return false;
    }
    onReleased: () => {
        if (dragged) {
            for (let note of sequenceViewModel.handle.selection) {
                note.intermediate = false;
            }
            pointerMouseArea.transactionControllerNotifier?.transactionCommitted();
        }
        dragScroller.running = false;
        sendInteractionNotification(ScopicFlow.II_Released);
    }

    DragScroller {
        id: dragScroller

        onMoved: function (deltaX, deltaY) {
            timeManipulator.moveViewBy(deltaX);
            if (pointerMouseArea.verticalManipulator)
                pointerMouseArea.verticalManipulator.moveViewBy(deltaY);
            let parentX = undefined;
            if (deltaX > 0)
                parentX = pointerMouseArea.paneItem.width;
            else if (deltaX < 0)
                parentX = 0;
            let parentY = undefined;
            if (deltaY > 0)
                parentY = pointerMouseArea.paneItem.height;
            else if (deltaY < 0)
                parentY = 0;
            if (parentX !== undefined)
                pointerMouseArea.moveSelectedNotesToX(parentX - pointerMouseArea.pressedDeltaX);
            if (parentY !== undefined)
                pointerMouseArea.moveSelectedNotesToY(parentY - pointerMouseArea.pressedDeltaY);
        }
    }
}
