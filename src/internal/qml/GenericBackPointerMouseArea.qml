import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    property var controller: null
    property int selectInteractionFlag: 0
    property int selectByRubberBandInteractionFlag: 0
    property TimeManipulator timeManipulator: null
    property QtObject verticalManipulator: null
    property RubberBandLayer rubberBandLayer: null
    property SelectionController selectionController: null
    property Item target: parent
    property var mapPoint: p => p

    acceptedButtons: Qt.LeftButton
    anchors.fill: parent
    enabled: (controller?.interaction ?? 0) & selectByRubberBandInteractionFlag
    focus: true
    focusPolicy: Qt.ClickFocus

    onCanceled: () => {
        rubberBandLayer.endSelection();
        rubberBandDragScroller.running = false;
    }
    onClicked: mouse => {
        if (!helper.dragged) {
            if (controller.itemInteraction & selectInteractionFlag) {
                controller.itemInteractionOperationStarted(target, null, selectInteractionFlag)
                selectionController.selectByMouse(null, mouse.button, mouse.modifiers);
                controller.itemInteractionOperationFinished(target, null, selectInteractionFlag)
            }
        }
    }
    onDoubleClicked: mouse => {
        if (!helper.dragged) {
            if (controller.itemInteraction & selectInteractionFlag) {
                controller.itemInteractionOperationStarted(target, null, selectInteractionFlag)
                selectionController.selectByMouse(null, SelectionController.ClearPreviousSelection);
                controller.itemInteractionOperationFinished(target, null, selectInteractionFlag)
            }
            if (timeManipulator && verticalManipulator) {
                controller.doubleClicked(target, timeManipulator.mapToPosition(mouse.x), verticalManipulator.mapToPosition(mouse.y))
            } else if (timeManipulator) {
                controller.doubleClicked(target, timeManipulator.mapToPosition(mouse.x))
            } else if (verticalManipulator) {
                controller.doubleClicked(target, verticalManipulator.mapToPosition(mouse.y))
            } else {
                controller.doubleClicked(target)
            }

        }
    }
    onPositionChanged: mouse => {
        helper.dragged = true;
        if (!rubberBandLayer.started) {
            if (controller.itemInteraction & selectInteractionFlag) {
                controller.itemInteractionOperationStarted(target, null, mouseArea.selectInteractionFlag)
                selectionController.selectByMouse(null, SelectionController.ClearPreviousSelection);
                controller.itemInteractionOperationFinished(target, null, mouseArea.selectInteractionFlag)
            }
            controller.interactionOperationStarted(target, selectByRubberBandInteractionFlag)
            rubberBandLayer.startSelection(helper.pressedPoint);
        }
        rubberBandDragScroller.determine(mouse.x, width, mouse.y, height, (triggeredX, triggeredY) => {
            if (!triggeredX) {
                helper.doDragRubberBand(Qt.point(mouse.x, helper.lastTargetPoint.y));
            }
            if (!triggeredY) {
                helper.doDragRubberBand(Qt.point(helper.lastTargetPoint.x, mouse.y));
            }
        });
    }
    onPressed: mouse => {
        helper.dragged = false;
        helper.pressedPoint = mapPoint(Qt.point(mouse.x, mouse.y))
    }
    onReleased: () => {
        rubberBandLayer.endSelection();
        controller.interactionOperationFinished(target, selectByRubberBandInteractionFlag)
        rubberBandDragScroller.running = false;
    }

    QtObject {
        id: helper
        property bool dragged: false
        property point lastTargetPoint: Qt.point(0, 0)
        property point pressedPoint: Qt.point(0, 0)

        function doDragRubberBand(targetPoint) {
            mouseArea.rubberBandLayer.updateSelection(mouseArea.mapPoint(targetPoint));
            lastTargetPoint = targetPoint;
        }
    }

    DragScroller {
        id: rubberBandDragScroller

        onMoved: (deltaX, deltaY) => {
            if (mouseArea.timeManipulator) {
                mouseArea.timeManipulator.moveViewBy(deltaX);
            }
            if (mouseArea.verticalManipulator) {
                mouseArea.verticalManipulator.moveViewBy(deltaY);
            }
            if (deltaX !== 0) {
                helper.doDragRubberBand(Qt.point(deltaX > 0 ? mouseArea.width : 0, helper.lastTargetPoint.y));
            }
            if (deltaY !== 0) {
                helper.doDragRubberBand(Qt.point(helper.lastTargetPoint.x, deltaY > 0 ? mouseArea.height : 0));
            }
        }
    }
}
