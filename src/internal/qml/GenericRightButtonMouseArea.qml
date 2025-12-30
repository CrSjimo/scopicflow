import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    property QtObject controller: null
    property int selectInteractionFlag: 0
    property Item paneItem: null
    property QtObject viewModel: null
    property SelectionController selectionController: null

    anchors.fill: parent
    acceptedButtons: Qt.RightButton
    focus: true
    focusPolicy: Qt.ClickFocus

    onClicked: (mouse) => {
        if (selectionController && (controller.itemInteraction & mouseArea.selectInteractionFlag)) {
            selectionController.selectByMouse(viewModel, mouse.button, mouse.modifiers);
        }
        controller.itemContextMenuRequested(paneItem, viewModel)
    }

}