import QtQml
import QtQuick
import QtQuick.Shapes
import QtQuick.Templates as T

import SVSCraft

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

DispatchedDragHandler {
    id: handler

    activationPolicy: DispatchedDragHandler.Immediately

    property var controller: null
    property SelectionController selectionController: null
    property Item paneItem: null
    property TimeManipulator timeManipulator: null
    property QtObject activeViewModel: null
    property int splitPosition: 0
    property double indicatorHeight: 0

    onStarted: (event, hit) => {
        handler.activeViewModel = hit.target
        if (!handler.controller || !handler.activeViewModel) {
            cancel()
            return
        }
        handler.splitPosition = handler.timeManipulator.alignPosition(
            handler.timeManipulator.mapToPosition(event.position.x),
            ScopicFlow.AO_Visible)
        handler.indicatorHeight = hit.targetRect.height
        indicator.x = handler.timeManipulator.mapToX(handler.splitPosition)
        indicator.y = hit.targetRect.y
        if (handler.controller.clickSelectable && handler.selectionController) {
            handler.selectionController.selectByPointer(
                handler.activeViewModel, SelectionController.ContextSelection, 0)
        }
        handler.controller.splitStarted(handler.paneItem, handler.splitPosition)
    }

    onFinished: {
        if (handler.controller && handler.activeViewModel)
            handler.controller.splitCommitted(handler.paneItem, handler.splitPosition)
        handler.activeViewModel = null
    }

    onCanceled: {
        if (handler.controller && handler.activeViewModel)
            handler.controller.splitAborted(handler.paneItem)
        handler.activeViewModel = null
    }

    T.Popup {
        id: indicator

        parent: handler.paneItem
        visible: handler.active

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
                PathLine { x: 0; y: handler.indicatorHeight + 4 }
                PathMove { x: -4; y: handler.indicatorHeight + 8 }
                PathLine { x: 0; y: handler.indicatorHeight + 4 }
                PathLine { x: 4; y: handler.indicatorHeight + 8 }
            }
        }
    }
}
