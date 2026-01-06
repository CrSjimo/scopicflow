import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    property var controller: null
    property Item target: parent
    property QtObject timeManipulator: null
    property QtObject verticalManipulator: null

    acceptedButtons: Qt.NoButton
    anchors.fill: parent
    hoverEnabled: true
    cursorShape: undefined

    onEntered: () => {
        if (controller) {
            if (timeManipulator && verticalManipulator) {
                controller.hoverEntered(target, timeManipulator.mapToPosition(mouseX), verticalManipulator.mapToPosition(mouseY))
            } else if (timeManipulator) {
                controller.hoverEntered(target, timeManipulator.mapToPosition(mouseX))
            } else if (verticalManipulator) {
                controller.hoverEntered(target, verticalManipulator.mapToPosition(mouseY))
            } else {
                controller.hoverEntered(target)
            }

        }
    }

    onPositionChanged: () => {
        if (controller) {
            if (timeManipulator && verticalManipulator) {
                controller.hoverMoved(target, timeManipulator.mapToPosition(mouseX), verticalManipulator.mapToPosition(mouseY))
            } else if (timeManipulator) {
                controller.hoverMoved(target, timeManipulator.mapToPosition(mouseX))
            } else if (verticalManipulator) {
                controller.hoverMoved(target, verticalManipulator.mapToPosition(mouseY))
            }

        }
    }

    onExited: () => {
        if (controller) {
            controller.hoverExited(target)
        }
    }

}