import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

DispatchedDragHandler {
    id: handler

    property var controller: null
    property SelectionController selectionController: null
    property Item paneItem: null
    property var viewModel: null
    property TimeManipulator timeManipulator: null
    property int splitPosition: 0
    property bool splitStarted: false
    property int splitThreshold: 24

    onDragStarted: () => {
        splitStarted = false
        splitPosition = timeManipulator.alignPosition(
            timeManipulator.mapToPosition(mapToItem(paneItem, startPoint.x, startPoint.y).x),
            ScopicFlow.AO_Visible
        )
        if (controller.clickSelectable) {
            selectionController.selectByMouse(viewModel, Qt.RightButton, modifiers)
        }
        controller.splitAboutToStart(paneItem)
    }

    onDragMoved: (x, y) => {
        const distance = Math.max(Math.abs(x - startPoint.x), Math.abs(y - startPoint.y))
        if (!splitStarted && distance >= splitThreshold) {
            splitStarted = true
            controller.splitStarted(paneItem, splitPosition)
        } else if (splitStarted && distance < splitThreshold) {
            splitStarted = false
            controller.splitAborted(paneItem)
        }
    }

    onDragFinished: () => {
        if (splitStarted) {
            controller.splitCommitted(paneItem, splitPosition)
        }
    }

    onDragCanceled: () => {
        if (splitStarted) {
            controller.splitAborted(paneItem)
        }
    }
}
