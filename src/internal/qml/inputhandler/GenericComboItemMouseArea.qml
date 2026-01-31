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
    property Item paneItem: null
    property var viewModel: null
    property var dispatchMap: {}

    GenericHoverMouseArea {
        controller: d.controller
        paneItem: d.paneItem
        viewModel: d.viewModel
    }

    GenericRightButtonMouseArea {
        controller: d.controller
        selectionController: d.selectionController
        paneItem: d.paneItem
        viewModel: d.viewModel
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
                    d.dispatchMap[d.controller.secondaryItemInteraction] :
                    d.dispatchMap[d.controller.primaryItemInteraction]) ?? null
            }

        }
        onClicked: (mouse) => {
            if (d.controller && d.controller.clickSelectable && d.selectionController) {
                d.selectionController.selectByMouse(d.viewModel, Qt.LeftButton, mouse.modifiers);
            }
        }
        onDoubleClicked: (mouse) => {
            if (d.controller) {
                d.controller.itemDoubleClicked(d.paneItem, d.viewModel)
            }
        }
    }
}