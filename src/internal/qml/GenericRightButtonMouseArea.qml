import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    property QtObject controller: null
    property Item paneItem: null
    property var viewModel: null
    property SelectionController selectionController: null

    anchors.fill: parent
    acceptedButtons: Qt.RightButton
    focus: true
    focusPolicy: Qt.ClickFocus

    onClicked: (mouse) => {
        if (selectionController && controller && controller.clickSelectable) {
            selectionController.selectByMouse(viewModel, mouse.button, mouse.modifiers);
        }
        controller.itemContextMenuRequested(paneItem, viewModel)
    }

}