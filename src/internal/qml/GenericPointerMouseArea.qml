import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    readonly property bool dragging: helper.dragged & pressed

    property QtObject controller: null
    property int moveInteractionFlag: 0
    property int selectInteractionFlag: 0
    property Item target: parent
    property Item paneItem: null
    property QtObject viewModel: null
    property QtObject sequenceViewModel: null
    property TimeManipulator timeManipulator: null
    property QtObject verticalManipulator: null
    property SelectionController selectionController: null

    acceptedButtons: Qt.LeftButton
    anchors.fill: parent
    focus: true
    focusPolicy: Qt.ClickFocus

    signal moveSelectedNotesToYRequested(y: double)

    QtObject {
        id: helper

        property bool dragged: false
        property double pressedDeltaX: 0
        property double pressedDeltaY: 0

        function moveSelectedNotesToX(x) {
            moveSelectionTo(mouseArea.timeManipulator.alignPosition(mouseArea.timeManipulator.mapToPosition(x)));
        }
        function moveSelectionTo(position) {
            let deltaPosition = position - mouseArea.viewModel.position
            if (deltaPosition === 0)
                return;
            let selection = mouseArea.selectionController.getSelectedItems()
            for (let note of selection) {
                if (note.position + deltaPosition < 0)
                    return;
            }
            for (let note of selection) {
                note.position = note.position + deltaPosition;
            }
        }
    }

    onCanceled: () => {
        dragScroller.running = false;
        if (helper.dragged) {
            helper.dragged = false;
            controller.itemInteractionOperationFinished(paneItem, viewModel, moveInteractionFlag)
        }
    }
    onClicked: mouse => {
        if (!helper.dragged) {
            if (selectionController && (controller.itemInteraction & mouseArea.selectInteractionFlag)) {
                controller.itemInteractionOperationStarted(paneItem, viewModel, mouseArea.selectInteractionFlag)
                selectionController.selectByMouse(viewModel, mouse.button, mouse.modifiers);
                controller.itemInteractionOperationFinished(paneItem, viewModel, mouseArea.selectInteractionFlag)
            }
        }
    }
    onDoubleClicked: mouse => {
        if (!helper.dragged) {
            controller.itemDoubleClicked(paneItem, viewModel)
        }
    }
    onPositionChanged: mouse => {
        if (!(controller?.itemInteraction & moveInteractionFlag)) {
            return;
        }
        if (!helper.dragged) {
            helper.dragged = true;
            controller.itemInteractionOperationStarted(paneItem, viewModel, moveInteractionFlag)
            if (selectionController && (controller.itemInteraction & mouseArea.selectInteractionFlag)) {
                controller.itemInteractionOperationStarted(paneItem, viewModel, mouseArea.selectInteractionFlag)
                selectionController.selectByMouse(viewModel, Qt.RightButton, mouse.modifiers);
                controller.itemInteractionOperationFinished(paneItem, viewModel, mouseArea.selectInteractionFlag)
            }
        }
        let parentPoint = mapToItem(paneItem, mouse.x, mouse.y);
        dragScroller.determine(parentPoint.x, paneItem.width, parentPoint.y, paneItem.height, (triggeredX, triggeredY) => {
            if (!triggeredX) {
                helper.moveSelectedNotesToX(parentPoint.x - helper.pressedDeltaX);
            }
            if (!triggeredY) {
                moveSelectedNotesToYRequested(parentPoint.y - helper.pressedDeltaY);
            }
        });
    }
    onPressed: mouse => {
        if ((mouse.modifiers & Qt.AltModifier)) {
            mouse.accepted = false
            return
        }
        helper.dragged = false;
        helper.pressedDeltaX = mouse.x;
        helper.pressedDeltaY = mouse.y;
    }
    onReleased: canceled()

    DragScroller {
        id: dragScroller

        onMoved: function (deltaX, deltaY) {
            mouseArea.timeManipulator.moveViewBy(deltaX);
            if (mouseArea.verticalManipulator)
                mouseArea.verticalManipulator.moveViewBy(deltaY);
            let parentX = undefined;
            if (deltaX > 0)
                parentX = mouseArea.paneItem.width;
            else if (deltaX < 0)
                parentX = 0;
            let parentY = undefined;
            if (deltaY > 0)
                parentY = mouseArea.paneItem.height;
            else if (deltaY < 0)
                parentY = 0;
            if (parentX !== undefined)
                helper.moveSelectedNotesToX(parentX - helper.pressedDeltaX);
            if (parentY !== undefined)
                mouseArea.moveSelectedNotesToYRequested(parentY - helper.pressedDeltaY);
        }
    }
}
