import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    property var controller: null
    property int interactionFlag: 0
    property TimeManipulator timeManipulator: null
    property QtObject verticalManipulator: null
    property RubberBandLayer rubberBandLayer: null
    property SelectableViewModelManipulator selectionManipulator: null
    property Item target: parent
    property var mapPoint: p => p

    acceptedButtons: Qt.LeftButton
    anchors.fill: parent
    enabled: (controller?.interaction ?? 0) & interactionFlag
    focus: true
    focusPolicy: Qt.ClickFocus

    onCanceled: () => {
        rubberBandLayer.endSelection();
        rubberBandDragScroller.running = false;
    }
    onClicked: mouse => {
        if (!dragged) {
            selectionManipulator.select(null, mouse.button, mouse.modifiers);
        }
    }
    onPositionChanged: mouse => {
        helper.dragged = true;
        if (!rubberBandLayer.started) {
            selectionManipulator.select(null, Qt.RightButton, mouse.modifiers);
            controller.interactionOperationStarted(target, interactionFlag)
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
        controller.interactionOperationFinished(target, interactionFlag)
        rubberBandDragScroller.running = false;
    }

    QtObject {
        id: helper
        property bool dragged: false
        property point lastTargetPoint: Qt.point(0, 0)
        property point pressedPoint: Qt.point(0, 0)

        function doDragRubberBand(targetPoint) {
            mouseArea.rubberBandLayer.updateSelection(mouseArea.mapPoint(targetPoint));
            rubberBandUpdateRequired(targetPoint);
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
                helper.doDragRubberBand(Qt.point(deltaX > 0 ? mouseArea.width : 0, 0, helper.lastTargetPoint.y));
            }
            if (deltaY !== 0) {
                helper.doDragRubberBand(Qt.point(helper.lastTargetPoint.x, deltaY > 0 ? mouseArea.height : 0));
            }
        }
    }
}
