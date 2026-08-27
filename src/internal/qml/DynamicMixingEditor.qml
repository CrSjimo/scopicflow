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

    onDynamicMixingViewModelChanged: pointerRouter.cancel()

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
                selectionController.selectByPointer(
                    item, SelectionController.ContextSelection, 0)
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
                dynamicMixingEditor.selectionController.selectByPointer(
                    null, SelectionController.PrimarySelection, 0)
            const newItem = controller.createAndInsertAnchor(viewModel, position, ratio)
            if (!newItem) {
                controller.anchorInsertionAborted(dynamicMixingEditor)
                return
            }
            controller.anchorInsertionCommitted(dynamicMixingEditor, newItem)
            if (controller.clickSelectable && dynamicMixingEditor.selectionController)
                dynamicMixingEditor.selectionController.selectByPointer(
                    newItem, SelectionController.PrimarySelection, 0)
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

        onStarted: (event, hit) => {
            item = hit.target
            handleIndex = hit.payload?.handleIndex ?? -1
            lastPoint = event.position
            positionOffset = item
                ? timeManipulator.mapToPosition(event.position.x) - item.position : 0
            const movingItems = helper.anchorOperationItems(item)
            if (!item || !content.beginAnchorMove(movingItems, item, handleIndex, lastPoint)) {
                item = null
                handleIndex = -1
                return
            }
            dynamicMixingEditor.interactionController?.anchorMovingStarted(dynamicMixingEditor, item)
        }
        onMoved: event => {
            anchorMoveDragScroller.determine(
                event.position.x, event.surfaceRect.width, 0, 0, triggeredX => {
                if (!triggeredX)
                    moveTo(event.position)
            })
        }
        onFinished: {
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
        onCanceled: {
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
                    deltaX > 0 ? anchorMoveDragHandler.surfaceRect.width : 0,
                    anchorMoveDragHandler.lastPoint.y))
            }
        }
    }

    Connections {
        target: dynamicMixingEditor.dynamicMixingViewModel

        function onVoiceCountChanged() {
            pointerRouter.cancel()
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

        onStarted: event => {
            originPosition = Math.max(0, timeManipulator.mapToPosition(event.position.x))
            currentPoint = event.position
            oldSelection = dynamicMixingEditor.selectionController?.getSelectedItems() ?? []
            dynamicMixingEditor.selectionController?.selectByPointer(
                null, SelectionController.PrimarySelection, event.modifiers)
            rubberBandRectangle.visible = true
            updateRectangle(currentPoint)
            dynamicMixingEditor.interactionController?.rubberBandDraggingStarted(
                dynamicMixingEditor)
        }
        onMoved: event => {
            rubberBandDragScroller.determine(
                event.position.x, event.surfaceRect.width, 0, 0, triggeredX => {
                if (!triggeredX)
                    updateRectangle(event.position)
            })
        }
        onFinished: {
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
        onCanceled: {
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
                    deltaX > 0 ? rubberBandDragHandler.surfaceRect.width : 0,
                    rubberBandDragHandler.currentPoint.y))
            }
        }
    }

    PointerInteractionRouter {
        id: pointerRouter
    }

    PointerInputArea {
        anchors.fill: parent
        router: pointerRouter
        coordinateSpace: dynamicMixingEditor
        z: 4

        hitResolver: (point, _) => {
            const hit = content.hitTest(point, 8)
            return {
                valid: true,
                target: hit.anchor ?? null,
                targetRect: Qt.rect(0, 0, 0, 0),
                hoverRegion: 0,
                payload: {
                    handleIndex: hit.handleIndex ?? -1,
                    interaction: DynamicMixingEditorInteractionController.None,
                },
            }
        }

        handlerResolver: (event, hit) => {
            const interaction = helper.interactionFor(event, hit.target)
            hit.payload.interaction = interaction
            switch (interaction) {
            case DynamicMixingEditorInteractionController.Pointer:
                return hit.target
                    ? anchorMoveDragHandler : rubberBandDragHandler
            case DynamicMixingEditorInteractionController.RubberBandSelect:
                return rubberBandDragHandler
            default:
                return null
            }
        }
    }

    Connections {
        target: pointerRouter

        function onClicked(event, hit) {
            const controller = dynamicMixingEditor.interactionController
            const item = hit.target
            const interaction = hit.payload?.interaction
                ?? helper.interactionFor(event, item)
            switch (interaction) {
            case DynamicMixingEditorInteractionController.Pointer:
            case DynamicMixingEditorInteractionController.RubberBandSelect:
                if (controller?.clickSelectable && dynamicMixingEditor.selectionController)
                    dynamicMixingEditor.selectionController.selectByPointer(
                        item, SelectionController.PrimarySelection, event.modifiers)
                break
            case DynamicMixingEditorInteractionController.AddAnchor:
                helper.insertAnchorAt(event.position.x)
                break
            case DynamicMixingEditorInteractionController.DeleteAnchor:
                if (item)
                    helper.deleteAnchorItems(item)
                break
            default:
                break
            }
        }

        function onContextMenuRequested(event, hit) {
            const controller = dynamicMixingEditor.interactionController
            if (hit.target) {
                if (controller?.clickSelectable && dynamicMixingEditor.selectionController) {
                    dynamicMixingEditor.selectionController.selectByPointer(
                        hit.target, SelectionController.ContextSelection, event.modifiers)
                }
                controller?.itemContextMenuRequested(dynamicMixingEditor, hit.target)
            } else {
                dynamicMixingEditor.selectionController?.select(
                    null, SelectionController.ClearPreviousSelection)
                controller?.contextMenuRequested(
                    dynamicMixingEditor,
                    Math.max(0, timeManipulator.mapToPosition(event.position.x)))
            }
        }

        function onHoverEntered(event, hit) {
            if (hit.target) {
                dynamicMixingEditor.interactionController?.itemHoverEntered(
                    dynamicMixingEditor, hit.target)
            } else {
                dynamicMixingEditor.interactionController?.hoverEntered(
                    dynamicMixingEditor,
                    Math.max(0, timeManipulator.mapToPosition(event.position.x)))
            }
        }

        function onHoverMoved(event, hit) {
            if (!hit.target) {
                dynamicMixingEditor.interactionController?.hoverMoved(
                    dynamicMixingEditor,
                    Math.max(0, timeManipulator.mapToPosition(event.position.x)))
            }
        }

        function onHoverExited(hit) {
            if (hit.target) {
                dynamicMixingEditor.interactionController?.itemHoverExited(
                    dynamicMixingEditor, hit.target)
            } else {
                dynamicMixingEditor.interactionController?.hoverExited(
                    dynamicMixingEditor)
            }
        }
    }
}
