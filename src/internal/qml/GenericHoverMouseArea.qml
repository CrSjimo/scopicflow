import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    property QtObject controller: null
    property Item paneItem: null
    property QtObject viewModel: null

    anchors.fill: parent
    acceptedButtons: Qt.NoButton
    hoverEnabled: true

    onEntered: () => {
        if (controller) {
            controller.itemHoverEntered(paneItem, viewModel)
        }
    }

    onExited: () => {
        if (controller) {
            controller.itemHoverExited(paneItem, viewModel)
        }
    }

}