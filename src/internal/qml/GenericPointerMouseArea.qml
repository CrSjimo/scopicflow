import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    readonly property bool dragging: helper.dragged & pressed

    required property Item paneItem
    required property QtObject viewModel
    required property QtObject sequenceViewModel
    required property TimeManipulator timeManipulator
    property QtObject verticalManipulator: null

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

            for (let note of mouseArea.sequenceViewModel.selection) {
                if (note.position + deltaPosition < 0)
                    return;
            }
            for (let note of mouseArea.sequenceViewModel.selection) {
                note.position = note.position + deltaPosition;
            }
        }
    }

    anchors.fill: parent

    onCanceled: () => {
        dragScroller.running = false;
    }
    onClicked: mouse => {
        if (!helper.dragged) {
            mouseArea.sequenceViewModel.iSelectable.select(viewModel, mouse.button, mouse.modifiers);
        }
    }
    onPositionChanged: mouse => {
        if (!pressed)
            return;
        if (!helper.dragged) {
            helper.dragged = true;
        }
        mouseArea.sequenceViewModel.iSelectable.select(viewModel, Qt.RightButton, mouse.modifiers);
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
        helper.dragged = false;
        helper.pressedDeltaX = mouse.x;
        helper.pressedDeltaY = mouse.y;
    }
    onReleased: () => {
        dragScroller.running = false;
    }

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
