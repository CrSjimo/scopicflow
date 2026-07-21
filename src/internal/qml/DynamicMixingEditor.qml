pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: dynamicMixingEditor

    property DynamicMixingViewModel dynamicMixingViewModel: null
    property TimeViewModel timeViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property DynamicMixingEditorInteractionController interactionController: null
    property SelectionController selectionController: null
    property list<color> colors: [Theme.accentColor]

    Accessible.name: qsTr("Dynamic mixing editor")
    Accessible.role: Accessible.Pane
    focus: true
    focusPolicy: Qt.TabFocus
    clip: true
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    onDynamicMixingViewModelChanged: anchorMoveDragHandler.abortForModelChange()

    TimeManipulator {
        id: timeManipulator
        target: dynamicMixingEditor
        timeViewModel: dynamicMixingEditor.timeViewModel
        timeLayoutViewModel: dynamicMixingEditor.timeLayoutViewModel
    }

    DynamicMixingEditorContent {
        id: content
        anchors.fill: parent

        dynamicMixingViewModel: dynamicMixingEditor.dynamicMixingViewModel
        timeViewModel: dynamicMixingEditor.timeViewModel
        timeLayoutViewModel: dynamicMixingEditor.timeLayoutViewModel
        colors: helper.effectiveColors
        boundaryLineColor: Theme.foregroundPrimaryColor
        anchorLineColor: Theme.foregroundSecondaryColor
        selectedAnchorLineColor: Theme.accentColor
        handleColor: Theme.accentColor
        selectedHandleBorderColor: Theme.foregroundPrimaryColor
    }

    RubberBandRectangle {
        id: rubberBandRectangle
        visible: false
        z: 2
    }

    QtObject {
        id: helper

        readonly property list<color> effectiveColors: dynamicMixingEditor.colors.length > 0
                                                       ? dynamicMixingEditor.colors
                                                       : [Theme.accentColor]
        property DynamicMixingAnchorViewModel pressedAnchor: null
        property int pressedHandleIndex: -1
        property int pressedInteraction: DynamicMixingEditorInteractionController.None
        property DynamicMixingAnchorViewModel hoveredAnchor: null

        function interactionFor(mouse, anchor): int {
            const controller = dynamicMixingEditor.interactionController
            if (!controller)
                return DynamicMixingEditorInteractionController.None
            if (mouse.modifiers & Qt.ControlModifier) {
                return mouse.modifiers & Qt.AltModifier
                    ? controller.secondarySelectInteraction
                    : controller.primarySelectInteraction
            }
            if (anchor) {
                return mouse.modifiers & Qt.AltModifier
                    ? controller.secondaryItemInteraction
                    : controller.primaryItemInteraction
            }
            return mouse.modifiers & Qt.AltModifier
                ? controller.secondarySceneInteraction
                : controller.primarySceneInteraction
        }

        function anchorOperationItems(item: DynamicMixingAnchorViewModel): var {
            if (!item)
                return []
            const controller = dynamicMixingEditor.interactionController
            const selectionController = dynamicMixingEditor.selectionController
            if (controller?.clickSelectable && selectionController)
                selectionController.selectByMouse(item, Qt.RightButton, 0)
            if (selectionController && item.selected) {
                const selectedItems = selectionController.getSelectedItems()
                if (selectedItems.length > 0)
                    return selectedItems
            }
            return [item]
        }

        function deleteAnchorItems(item: DynamicMixingAnchorViewModel) {
            const controller = dynamicMixingEditor.interactionController
            const viewModel = dynamicMixingEditor.dynamicMixingViewModel
            const selectionController = dynamicMixingEditor.selectionController
            if (!controller || !viewModel || !item)
                return
            const operationItems = anchorOperationItems(item)
            if (operationItems.length === 0)
                return

            controller.anchorDeletionStarted(dynamicMixingEditor, item)
            let succeeded = true
            const removedItems = []
            for (const operationItem of operationItems) {
                selectionController?.select(operationItem, SelectionController.Deselect)
                if (controller.removeAnchor(viewModel, operationItem)) {
                    removedItems.push(operationItem)
                } else {
                    succeeded = false
                    selectionController?.select(operationItem, SelectionController.Select)
                }
            }
            if (selectionController && removedItems.indexOf(selectionController.currentItem) >= 0)
                selectionController.select(null, SelectionController.SetCurrentItem)
            if (succeeded)
                controller.anchorDeletionCommitted(dynamicMixingEditor, item)
            else
                controller.anchorDeletionAborted(dynamicMixingEditor, item)
        }

        function insertAnchorAt(x: double) {
            const controller = dynamicMixingEditor.interactionController
            const viewModel = dynamicMixingEditor.dynamicMixingViewModel
            if (!controller || !viewModel)
                return
            const position = Math.max(0, timeManipulator.alignPosition(
                timeManipulator.mapToPosition(x), ScopicFlow.AO_Visible))
            const ratio = viewModel.ratioAt(position)
            controller.anchorInsertionStarted(dynamicMixingEditor, position, ratio)
            if (viewModel.itemsAtPosition(position).length > 0) {
                controller.anchorInsertionAborted(dynamicMixingEditor)
                return
            }
            if (controller.clickSelectable && dynamicMixingEditor.selectionController)
                dynamicMixingEditor.selectionController.selectByMouse(null, Qt.LeftButton, 0)
            const newItem = controller.createAndInsertAnchor(viewModel, position, ratio)
            if (!newItem) {
                controller.anchorInsertionAborted(dynamicMixingEditor)
                return
            }
            controller.anchorInsertionCommitted(dynamicMixingEditor, newItem)
            if (controller.clickSelectable && dynamicMixingEditor.selectionController)
                dynamicMixingEditor.selectionController.selectByMouse(newItem, Qt.LeftButton, 0)
        }
    }

    DispatchedDragHandler {
        id: anchorMoveDragHandler

        property DynamicMixingAnchorViewModel item: null
        property int handleIndex: -1
        property int positionOffset: 0
        property point lastPoint: Qt.point(0, 0)

        function moveTo(point) {
            if (!item)
                return
            const targetPosition = timeManipulator.alignPosition(
                timeManipulator.mapToPosition(point.x) - positionOffset)
            content.updateAnchorMove(point, targetPosition)
            lastPoint = point
        }

        function abortForModelChange() {
            if (!item)
                return
            anchorMoveDragScroller.running = false
            content.abortAnchorMove()
            dynamicMixingEditor.interactionController?.anchorMovingAborted(
                dynamicMixingEditor, item)
            item = null
            handleIndex = -1
        }

        onDragStarted: (x, y) => {
            item = helper.pressedAnchor
            handleIndex = helper.pressedHandleIndex
            lastPoint = Qt.point(x, y)
            positionOffset = item ? timeManipulator.mapToPosition(x) - item.position : 0
            const movingItems = helper.anchorOperationItems(item)
            if (!item || !content.beginAnchorMove(movingItems, item, handleIndex, lastPoint)) {
                item = null
                handleIndex = -1
                return
            }
            dynamicMixingEditor.interactionController?.anchorMovingStarted(dynamicMixingEditor, item)
        }
        onDragMoved: (x, y) => {
            anchorMoveDragScroller.determine(x, width, 0, 0, triggeredX => {
                if (!triggeredX)
                    moveTo(Qt.point(x, y))
            })
        }
        onDragFinished: () => {
            anchorMoveDragScroller.running = false
            if (item) {
                const removedItems = content.commitAnchorMove()
                if (dynamicMixingEditor.selectionController) {
                    for (const removedItem of removedItems)
                        dynamicMixingEditor.selectionController.select(removedItem,
                                                                          SelectionController.Deselect)
                }
                dynamicMixingEditor.interactionController?.anchorMovingCommitted(
                    dynamicMixingEditor, item)
            }
            item = null
            handleIndex = -1
        }
        onDragCanceled: () => {
            anchorMoveDragScroller.running = false
            if (item) {
                content.abortAnchorMove()
                dynamicMixingEditor.interactionController?.anchorMovingAborted(
                    dynamicMixingEditor, item)
            }
            item = null
            handleIndex = -1
        }

        DragScroller {
            id: anchorMoveDragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX)
                anchorMoveDragHandler.moveTo(Qt.point(
                    deltaX > 0 ? anchorMoveDragHandler.width : 0,
                    anchorMoveDragHandler.lastPoint.y))
            }
        }
    }

    Connections {
        target: dynamicMixingEditor.dynamicMixingViewModel

        function onVoiceCountChanged() {
            anchorMoveDragHandler.abortForModelChange()
        }
    }

    DispatchedDragHandler {
        id: rubberBandDragHandler

        property int originPosition: 0
        property point currentPoint: Qt.point(0, 0)
        property var oldSelection: []

        function updateRectangle(point) {
            currentPoint = point
            const originX = timeManipulator.mapToX(originPosition)
            rubberBandRectangle.x = Math.min(originX, point.x)
            rubberBandRectangle.width = Math.abs(originX - point.x)
            rubberBandRectangle.y = 0
            rubberBandRectangle.height = dynamicMixingEditor.height
        }

        onDragStarted: (x, y, modifiers) => {
            originPosition = Math.max(0, timeManipulator.mapToPosition(x))
            currentPoint = Qt.point(x, y)
            oldSelection = dynamicMixingEditor.selectionController?.getSelectedItems() ?? []
            dynamicMixingEditor.selectionController?.selectByMouse(null, Qt.LeftButton, modifiers)
            rubberBandRectangle.visible = true
            updateRectangle(currentPoint)
            dynamicMixingEditor.interactionController?.rubberBandDraggingStarted(
                dynamicMixingEditor)
        }
        onDragMoved: (x, y) => {
            rubberBandDragScroller.determine(x, width, 0, 0, triggeredX => {
                if (!triggeredX)
                    updateRectangle(Qt.point(x, y))
            })
        }
        onDragFinished: () => {
            rubberBandDragScroller.running = false
            const items = content.anchorsInTimeRange(
                originPosition, Math.max(0, timeManipulator.mapToPosition(currentPoint.x)))
            if (dynamicMixingEditor.selectionController) {
                for (const item of items)
                    dynamicMixingEditor.selectionController.select(item, SelectionController.Toggle)
            }
            rubberBandRectangle.visible = false
            oldSelection = []
            dynamicMixingEditor.interactionController?.rubberBandDraggingCommitted(
                dynamicMixingEditor)
        }
        onDragCanceled: () => {
            rubberBandDragScroller.running = false
            rubberBandRectangle.visible = false
            if (dynamicMixingEditor.selectionController) {
                dynamicMixingEditor.selectionController.select(
                    null, SelectionController.ClearPreviousSelection)
                for (const item of oldSelection)
                    dynamicMixingEditor.selectionController.select(item, SelectionController.Select)
            }
            oldSelection = []
            dynamicMixingEditor.interactionController?.rubberBandDraggingAborted(
                dynamicMixingEditor)
        }

        DragScroller {
            id: rubberBandDragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX)
                rubberBandDragHandler.updateRectangle(Qt.point(
                    deltaX > 0 ? rubberBandDragHandler.width : 0,
                    rubberBandDragHandler.currentPoint.y))
            }
        }
    }

    DispatcherMouseArea {
        id: dispatcher
        anchors.fill: parent
        z: 3

        determineDragHandler: mouse => {
            const hit = content.hitTest(Qt.point(mouse.x, mouse.y), 8)
            helper.pressedAnchor = hit.anchor ?? null
            helper.pressedHandleIndex = hit.handleIndex ?? -1
            helper.pressedInteraction = helper.interactionFor(mouse, helper.pressedAnchor)
            switch (helper.pressedInteraction) {
            case DynamicMixingEditorInteractionController.Pointer:
                return helper.pressedAnchor
                    ? anchorMoveDragHandler : rubberBandDragHandler
            case DynamicMixingEditorInteractionController.RubberBandSelect:
                return rubberBandDragHandler
            default:
                return null
            }
        }

        onClicked: mouse => {
            const controller = dynamicMixingEditor.interactionController
            const item = content.anchorAt(Qt.point(mouse.x, mouse.y), 8)
            switch (helper.pressedInteraction) {
            case DynamicMixingEditorInteractionController.Pointer:
            case DynamicMixingEditorInteractionController.RubberBandSelect:
                if (controller?.clickSelectable && dynamicMixingEditor.selectionController)
                    dynamicMixingEditor.selectionController.selectByMouse(
                        item, Qt.LeftButton, mouse.modifiers)
                break
            case DynamicMixingEditorInteractionController.AddAnchor:
                helper.insertAnchorAt(mouse.x)
                break
            case DynamicMixingEditorInteractionController.DeleteAnchor:
                if (item)
                    helper.deleteAnchorItems(item)
                break
            default:
                break
            }
            helper.pressedAnchor = null
            helper.pressedHandleIndex = -1
        }
    }

    HoverHandler {
        id: hoverHandler
        onHoveredChanged: {
            if (!hovered) {
                if (helper.hoveredAnchor)
                    dynamicMixingEditor.interactionController?.itemHoverExited(
                        dynamicMixingEditor, helper.hoveredAnchor)
                helper.hoveredAnchor = null
                dynamicMixingEditor.interactionController?.hoverExited(dynamicMixingEditor)
            } else {
                dynamicMixingEditor.interactionController?.hoverEntered(
                    dynamicMixingEditor,
                    Math.max(0, timeManipulator.mapToPosition(point.position.x)))
            }
        }
        onPointChanged: {
            if (!hovered)
                return
            const item = content.anchorAt(point.position, 8)
            if (item !== helper.hoveredAnchor) {
                if (helper.hoveredAnchor)
                    dynamicMixingEditor.interactionController?.itemHoverExited(
                        dynamicMixingEditor, helper.hoveredAnchor)
                helper.hoveredAnchor = item
                if (helper.hoveredAnchor)
                    dynamicMixingEditor.interactionController?.itemHoverEntered(
                        dynamicMixingEditor, helper.hoveredAnchor)
            }
            dynamicMixingEditor.interactionController?.hoverMoved(
                dynamicMixingEditor,
                Math.max(0, timeManipulator.mapToPosition(point.position.x)))
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        z: 4
        onClicked: mouse => {
            const item = content.anchorAt(Qt.point(mouse.x, mouse.y), 8)
            if (item) {
                if (dynamicMixingEditor.interactionController?.clickSelectable
                    && dynamicMixingEditor.selectionController) {
                    dynamicMixingEditor.selectionController.selectByMouse(
                        item, Qt.RightButton, mouse.modifiers)
                }
                dynamicMixingEditor.interactionController?.itemContextMenuRequested(
                    dynamicMixingEditor, item)
            } else {
                dynamicMixingEditor.interactionController?.contextMenuRequested(
                    dynamicMixingEditor,
                    Math.max(0, timeManipulator.mapToPosition(mouse.x)))
            }
        }
    }
}
