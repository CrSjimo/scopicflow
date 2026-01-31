import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: d
    anchors.fill: parent

    property var controller: null
    property SelectionController selectionController: null
    property Item target: parent
    property TimeManipulator timeManipulator: null
    property var verticalManipulator: null
    property var dispatchMap: {}

    GenericBackHoverMouseArea {
        controller: d.controller
        target: d.target
        timeManipulator: d.timeManipulator
        verticalManipulator: d.verticalManipulator
    }

    GenericBackRightButtonMouseArea {
        controller: d.controller
        selectionController: d.selectionController
        target: d.target
        timeManipulator: d.timeManipulator
        verticalManipulator: d.verticalManipulator
    }

    DispatcherMouseArea {
        determineDragHandler: (mouse) => {
            if (!d.controller)
                return null
            if (mouse.modifiers & Qt.ControlModifier) {
                return ((mouse.modifiers & Qt.AltModifier) ?
                    d.dispatchMap[d.controller.secondarySelectInteraction] :
                    d.dispatchMap[d.controller.primarySelectInteraction]) ?? null
            } else {
                return ((mouse.modifiers & Qt.AltModifier) ?
                    d.dispatchMap[d.controller.secondarySceneInteraction] :
                    d.dispatchMap[d.controller.primarySceneInteraction]) ?? null
            }

        }
        onClicked: (mouse) => {
            if (d.controller && d.controller.clickSelectable && d.selectionController) {
                d.selectionController.selectByMouse(null, Qt.LeftButton, mouse.modifiers);
            }
        }
        onDoubleClicked: (mouse) => {
            if (d.controller) {
                if (d.timeManipulator && d.verticalManipulator) {
                    d.controller.doubleClicked(d.target, d.timeManipulator.mapToPosition(mouse.x), d.verticalManipulator.mapToPosition(mouse.y))
                } else if (d.timeManipulator) {
                    d.controller.doubleClicked(d.target, d.timeManipulator.mapToPosition(mouse.x))
                } else if (d.verticalManipulator) {
                    d.controller.doubleClicked(d.target, d.verticalManipulator.mapToPosition(mouse.y))
                } else {
                    d.controller.doubleClicked(d.target)
                }
            }
        }
    }
}