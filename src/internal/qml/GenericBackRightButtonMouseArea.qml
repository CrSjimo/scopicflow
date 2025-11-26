import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    property TimeManipulator timeManipulator: null
    property int selectInteractionFlag: 0
    property var verticalManipulator: null
    property QtObject iSelectable: null
    property var controller: null
    property Item target: parent

    acceptedButtons: Qt.RightButton
    anchors.fill: parent
    focus: true
    focusPolicy: Qt.ClickFocus

    onClicked: (mouse) => {
        if (controller) {
            if (timeManipulator && verticalManipulator) {
                controller.contextMenuRequested(target, timeManipulator.mapToPosition(mouse.x), verticalManipulator.mapToPosition(mouse.y))
            } else if (timeManipulator) {
                controller.contextMenuRequested(target, timeManipulator.mapToPosition(mouse.x))
            }
        }
        if (iSelectable && controller && ((controller.itemInteraction ?? 0) & selectInteractionFlag)) {
            controller.itemInteractionOperationStarted(target, null, selectInteractionFlag)
            iSelectable.select(null, mouse.button, mouse.modifiers);
            controller.itemInteractionOperationFinished(target, null, selectInteractionFlag)
        }
    }

}