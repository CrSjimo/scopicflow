import QtQml
import QtQuick
import QtQuick.Controls
import QtQuick.Templates as T
import QtQuick.Shapes

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

    startDraggingImmediately: true

    T.Popup {
        id: indicator
        x: 0
        y: 0
        Shape {
            ShapePath {
                strokeWidth: 2
                strokeColor: SFPalette.scissorIndicatorColor
                fillColor: "transparent"
                capStyle: ShapePath.RoundCap
                startX: -4
                startY: -8
                PathLine { x: 0; y: -4 }
                PathLine { x: 4; y: -8 }
                PathMove { x: 0; y: -4 }
                PathLine { x: 0; y: handler.height + 4 }
                PathMove { x: -4; y: handler.height + 8 }
                PathLine { x: 0; y: handler.height + 4 }
                PathLine { x: 4; y: handler.height + 8 }
            }
        }
        visible: handler.dragged
    }

    onDragStarted: () => {
        splitPosition = timeManipulator.alignPosition(
            timeManipulator.mapToPosition(mapToItem(paneItem, startPoint.x, startPoint.y).x),
            ScopicFlow.AO_Visible
        )
        indicator.x = mapFromItem(paneItem, timeManipulator.mapToX(splitPosition), 0).x
        if (controller.clickSelectable) {
            selectionController.selectByMouse(viewModel, Qt.RightButton, modifiers)
        }
        controller.splitStarted(paneItem, splitPosition)
    }

    onDragFinished: () => {
        controller.splitCommitted(paneItem, splitPosition)
    }

    onDragCanceled: () => {
        controller.splitAborted(paneItem)
    }
}
