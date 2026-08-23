pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: parameterEditor

    property FreeParameterViewModel freeParameterViewModel: null
    property AnchorParameterViewModel anchorParameterViewModel: null
    property FreeParameterViewModel originalParameterViewModel: null
    property FreeParameterViewModel freeTransformParameterViewModel: null
    property AnchorParameterViewModel anchorTransformParameterViewModel: null
    property ParameterRangeSelectionViewModel freeParameterSelectionViewModel: null
    property SelectionController anchorSelectionController: null
    property TimeViewModel timeViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property ParameterEditorInteractionController interactionController: null
    property QtObject verticalManipulator: null
    property Item verticalViewport: null

    Accessible.name: qsTr("Parameter editor")
    Accessible.role: Accessible.Pane
    focus: true
    focusPolicy: Qt.TabFocus
    clip: true
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    TimeManipulator {
        id: timeManipulator
        target: parameterEditor
        timeViewModel: parameterEditor.timeViewModel
        timeLayoutViewModel: parameterEditor.timeLayoutViewModel
    }

    function mapLocalYToViewport(y: double): double {
        return verticalViewport ? parameterEditor.mapToItem(verticalViewport, 0, y).y : 0
    }

    function mapViewportEdgeToLocalY(atBottom: bool): double {
        return verticalViewport ? parameterEditor.mapFromItem(verticalViewport, 0, atBottom ? verticalViewport.height : 0).y : 0
    }

    ParameterEditorContent {
        id: content
        anchors.fill: parent

        freeParameterViewModel: parameterEditor.freeParameterViewModel
        anchorParameterViewModel: parameterEditor.anchorParameterViewModel
        originalParameterViewModel: parameterEditor.originalParameterViewModel
        freeTransformParameterViewModel: parameterEditor.freeTransformParameterViewModel
        anchorTransformParameterViewModel: parameterEditor.anchorTransformParameterViewModel
        timeViewModel: parameterEditor.timeViewModel
        timeLayoutViewModel: parameterEditor.timeLayoutViewModel
        fillMode: parameterEditor.interactionController?.fillMode ?? ParameterEditorInteractionController.NoFill
        fillBaseline: parameterEditor.interactionController?.fillBaseline ?? 0.0
        referenceVisible: parameterEditor.interactionController?.referenceVisible ?? false
        referenceBaseline: parameterEditor.interactionController?.referenceBaseline ?? 0.0
        defaultValueEnabled: parameterEditor.interactionController?.defaultValueEnabled ?? false
        defaultValue: parameterEditor.interactionController?.defaultValue ?? 0.0
        originalAndDefaultCurveDisplayMode: parameterEditor.interactionController?.originalAndDefaultCurveDisplayMode
                                            ?? ParameterEditorInteractionController.CurveDashed
        editLayer: helper.editLayer

        curveColor: Theme.foregroundPrimaryColor
        dimmedCurveColor: Theme.foregroundSecondaryColor
        fillColor: Qt.rgba(Theme.accentColor.r, Theme.accentColor.g, Theme.accentColor.b, Theme.accentColor.a * 0.25)
        dimmedFillColor: Qt.rgba(Theme.foregroundSecondaryColor.r, Theme.foregroundSecondaryColor.g, Theme.foregroundSecondaryColor.b, Theme.foregroundSecondaryColor.a * 0.1875)
        accentColor: Theme.accentColor
        referenceColor: SFPalette.scaleSecondaryColor
        selectedAnchorColor: Theme.foregroundPrimaryColor
    }

    Rectangle {
        id: freeSelectionRectangle
        visible: parameterEditor.freeParameterSelectionViewModel?.hasSelection ?? false
        x: ((parameterEditor.freeParameterSelectionViewModel?.start ?? 0) - (parameterEditor.timeViewModel?.start ?? 0))
            * (parameterEditor.timeLayoutViewModel?.pixelDensity ?? 0)
        width: ((parameterEditor.freeParameterSelectionViewModel?.end ?? 0)
            - (parameterEditor.freeParameterSelectionViewModel?.start ?? 0))
            * (parameterEditor.timeLayoutViewModel?.pixelDensity ?? 0)
        height: parent.height
        color: Qt.rgba(Theme.accentColor.r, Theme.accentColor.g, Theme.accentColor.b, Theme.accentColor.a * 0.1875)
        border.color: Theme.accentColor
        border.width: 1
        z: 1
    }

    RubberBandRectangle {
        id: anchorRubberBandRectangle
        visible: false
        z: 2
    }

    QtObject {
        id: helper

        property ParameterAnchorViewModel pressedAnchor: null
        property ParameterAnchorViewModel hoveredAnchor: null
        property int pressedInteraction: ParameterEditorInteractionController.None
        readonly property int editLayer: {
            const sceneInteraction = parameterEditor.interactionController?.primarySceneInteraction
                                     ?? ParameterEditorInteractionController.None
            const itemInteraction = parameterEditor.interactionController?.primaryItemInteraction
                                    ?? ParameterEditorInteractionController.None
            if (isFreeInteraction(sceneInteraction))
                return ParameterEditorContent.FreeLayer
            if (isAnchorInteraction(sceneInteraction) || isAnchorInteraction(itemInteraction))
                return ParameterEditorContent.AnchorLayer
            return ParameterEditorContent.FinalLayer
        }

        function isFreeInteraction(interaction: int): bool {
            return interaction === ParameterEditorInteractionController.Pencil
                || interaction === ParameterEditorInteractionController.Line
                || interaction === ParameterEditorInteractionController.Brush
                || interaction === ParameterEditorInteractionController.Eraser
                || interaction === ParameterEditorInteractionController.FreeRangeSelect
        }

        function isAnchorInteraction(interaction: int): bool {
            return interaction === ParameterEditorInteractionController.Pointer
                || interaction === ParameterEditorInteractionController.Pen
                || interaction === ParameterEditorInteractionController.ConvertAnchor
                || interaction === ParameterEditorInteractionController.AnchorRubberBandSelect
                || interaction === ParameterEditorInteractionController.AnchorTimeRangeSelect
        }

        function interactionFor(mouse, anchor): int {
            const controller = parameterEditor.interactionController
            if (!controller)
                return ParameterEditorInteractionController.None
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

        function nextInterpolationMode(mode: int): int {
            if (mode === ParameterAnchorViewModel.Hermite)
                return ParameterAnchorViewModel.Linear
            if (mode === ParameterAnchorViewModel.Linear)
                return ParameterAnchorViewModel.None
            return ParameterAnchorViewModel.Hermite
        }

        function anchorOperationItems(item: ParameterAnchorViewModel): var {
            if (!item)
                return []
            const selectionController = parameterEditor.anchorSelectionController
            if (parameterEditor.interactionController?.clickSelectable && selectionController)
                selectionController.selectByMouse(item, Qt.RightButton, 0)
            if (selectionController && item.selected) {
                const selectedItems = selectionController.getSelectedItems()
                if (selectedItems.length > 0)
                    return selectedItems
            }
            return [item]
        }

        function deleteAnchorItems(item: ParameterAnchorViewModel) {
            const controller = parameterEditor.interactionController
            const viewModel = parameterEditor.anchorParameterViewModel
            const selectionController = parameterEditor.anchorSelectionController
            if (!controller || !viewModel || !item)
                return
            const operationItems = anchorOperationItems(item)
            if (operationItems.length === 0)
                return

            controller.anchorDeletionStarted(parameterEditor, item)
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
                controller.anchorDeletionCommitted(parameterEditor, item)
            else
                controller.anchorDeletionAborted(parameterEditor, item)
        }

        function convertAnchorItems(item: ParameterAnchorViewModel) {
            const controller = parameterEditor.interactionController
            if (!controller || !item)
                return
            const targetMode = nextInterpolationMode(item.interpolationMode)
            const operationItems = anchorOperationItems(item)
            if (operationItems.length === 0)
                return

            controller.anchorInterpolationChangingStarted(parameterEditor, item)
            for (const operationItem of operationItems)
                operationItem.interpolationMode = targetMode
            controller.anchorInterpolationChangingCommitted(parameterEditor, item)
        }
    }

    component FreeEditDragHandler: DispatchedDragHandler {
        id: freeEditHandler
        property int operation: ParameterEditorInteractionController.DrawFree
        property point lastPoint: Qt.point(0, 0)
        startDraggingImmediately: true

        function editPositionAt(point): int {
            return Math.max(0, timeManipulator.mapToPosition(point.x))
        }

        function editValueAt(point): double {
            const value = content.transformedValueFromPoint(point)
            return value === undefined || value === null ? 0.0 : value
        }

        function drawTo(point) {
            if (operation === ParameterEditorInteractionController.BrushFree)
                content.brushFreeSegment(lastPoint, point)
            else
                content.drawFreeSegment(lastPoint, point,
                    operation === ParameterEditorInteractionController.EraseFree)
            lastPoint = point
            parameterEditor.interactionController?.freeEditingUpdated(
                parameterEditor, operation, editPositionAt(point), editValueAt(point))
        }

        onDragStarted: (x, y) => {
            lastPoint = Qt.point(x, y)
            parameterEditor.interactionController?.freeEditingStarted(
                parameterEditor, operation, editPositionAt(lastPoint), editValueAt(lastPoint))
            if (operation === ParameterEditorInteractionController.BrushFree)
                content.brushFreeSegment(lastPoint, lastPoint)
            else
                content.drawFreeSegment(lastPoint, lastPoint,
                    operation === ParameterEditorInteractionController.EraseFree)
        }
        onDragMoved: (x, y) => {
            freeEditDragScroller.determine(x, width, parameterEditor.mapLocalYToViewport(y), parameterEditor.verticalViewport?.height ?? 0, (triggeredX, triggeredY) => {
                if (!triggeredX && !triggeredY)
                    drawTo(Qt.point(x, y))
            })
        }
        onDragFinished: () => {
            freeEditDragScroller.running = false
            parameterEditor.interactionController?.freeEditingCommitted(parameterEditor, operation)
        }
        onDragCanceled: () => {
            freeEditDragScroller.running = false
            parameterEditor.interactionController?.freeEditingAborted(parameterEditor, operation)
        }

        DragScroller {
            id: freeEditDragScroller
            onMoved: (deltaX, deltaY) => {
                if (deltaX !== 0) {
                    timeManipulator.moveViewBy(deltaX)
                    freeEditHandler.lastPoint = Qt.point(freeEditHandler.lastPoint.x - deltaX,
                                                         freeEditHandler.lastPoint.y)
                }
                if (deltaY !== 0)
                    parameterEditor.verticalManipulator?.moveViewBy(deltaY)
                if (deltaX !== 0 || deltaY !== 0) {
                    freeEditHandler.drawTo(Qt.point(
                        deltaX > 0 ? freeEditHandler.width : deltaX < 0 ? 0 : freeEditHandler.lastPoint.x,
                        deltaY !== 0 ? parameterEditor.mapViewportEdgeToLocalY(deltaY > 0) : freeEditHandler.lastPoint.y))
                }
            }
        }
    }

    FreeEditDragHandler {
        id: pencilDragHandler
    }

    FreeEditDragHandler {
        id: brushDragHandler
        operation: ParameterEditorInteractionController.BrushFree
    }

    FreeEditDragHandler {
        id: eraserDragHandler
        operation: ParameterEditorInteractionController.EraseFree
    }

    DispatchedDragHandler {
        id: lineDrawDragHandler
        property point startPoint: Qt.point(0, 0)
        property point lastPoint: Qt.point(0, 0)
        readonly property int operation: ParameterEditorInteractionController.DrawLineFree
        startDraggingImmediately: true

        function editPositionAt(point): int {
            return Math.max(0, timeManipulator.mapToPosition(point.x))
        }

        function editValueAt(point): double {
            const value = content.transformedValueFromPoint(point)
            return value === undefined || value === null ? 0.0 : value
        }

        function drawTo(point) {
            content.updateFreeLine(point)
            lastPoint = point
            parameterEditor.interactionController?.freeEditingUpdated(
                parameterEditor, operation, editPositionAt(point), editValueAt(point))
        }

        onDragStarted: (x, y) => {
            startPoint = lastPoint = Qt.point(x, y)
            content.beginFreeLine(startPoint)
            parameterEditor.interactionController?.freeEditingStarted(
                parameterEditor, operation, editPositionAt(startPoint), editValueAt(startPoint))
        }
        onDragMoved: (x, y) => {
            lineDrawDragScroller.determine(x, width, parameterEditor.mapLocalYToViewport(y), parameterEditor.verticalViewport?.height ?? 0, (triggeredX, triggeredY) => {
                if (!triggeredX && !triggeredY)
                    drawTo(Qt.point(x, y))
            })
        }
        onDragFinished: () => {
            lineDrawDragScroller.running = false
            content.commitFreeLine()
            parameterEditor.interactionController?.freeEditingCommitted(parameterEditor, operation)
        }
        onDragCanceled: () => {
            lineDrawDragScroller.running = false
            content.abortFreeLine()
            parameterEditor.interactionController?.freeEditingAborted(parameterEditor, operation)
        }

        DragScroller {
            id: lineDrawDragScroller
            onMoved: (deltaX, deltaY) => {
                if (deltaX !== 0)
                    timeManipulator.moveViewBy(deltaX)
                if (deltaY !== 0)
                    parameterEditor.verticalManipulator?.moveViewBy(deltaY)
                if (deltaX !== 0 || deltaY !== 0)
                    lineDrawDragHandler.drawTo(Qt.point(
                        deltaX > 0 ? lineDrawDragHandler.width : deltaX < 0 ? 0 : lineDrawDragHandler.lastPoint.x,
                        deltaY !== 0 ? parameterEditor.mapViewportEdgeToLocalY(deltaY > 0) : lineDrawDragHandler.lastPoint.y))
            }
        }
    }

    DispatchedDragHandler {
        id: anchorMoveDragHandler
        property ParameterAnchorViewModel item: null
        property point lastPoint: Qt.point(0, 0)

        function moveTo(point) {
            if (item)
                content.updateAnchorMove(point)
            lastPoint = point
        }

        onDragStarted: (x, y) => {
            item = helper.pressedAnchor
            lastPoint = Qt.point(x, y)
            const movingItems = helper.anchorOperationItems(item)
            if (!item || !content.beginAnchorMove(movingItems, item, lastPoint)) {
                item = null
                return
            }
            parameterEditor.interactionController?.anchorMovingStarted(parameterEditor, item)
        }
        onDragMoved: (x, y) => {
            anchorMoveDragScroller.determine(x, width, parameterEditor.mapLocalYToViewport(y), parameterEditor.verticalViewport?.height ?? 0, (triggeredX, triggeredY) => {
                if (!triggeredX && !triggeredY)
                    moveTo(Qt.point(x, y))
            })
        }
        onDragFinished: () => {
            anchorMoveDragScroller.running = false
            if (item) {
                const removedItems = content.commitAnchorMove()
                if (parameterEditor.anchorSelectionController) {
                    for (const removedItem of removedItems)
                        parameterEditor.anchorSelectionController.select(removedItem, SelectionController.Deselect)
                }
                parameterEditor.interactionController?.anchorMovingCommitted(parameterEditor, item)
            }
            item = null
        }
        onDragCanceled: () => {
            anchorMoveDragScroller.running = false
            if (item) {
                content.abortAnchorMove()
                parameterEditor.interactionController?.anchorMovingAborted(parameterEditor, item)
            }
            item = null
        }

        DragScroller {
            id: anchorMoveDragScroller
            onMoved: (deltaX, deltaY) => {
                if (deltaX !== 0)
                    timeManipulator.moveViewBy(deltaX)
                if (deltaY !== 0)
                    parameterEditor.verticalManipulator?.moveViewBy(deltaY)
                if (deltaX !== 0 || deltaY !== 0)
                    anchorMoveDragHandler.moveTo(Qt.point(
                        deltaX > 0 ? anchorMoveDragHandler.width : deltaX < 0 ? 0 : anchorMoveDragHandler.lastPoint.x,
                        deltaY !== 0 ? parameterEditor.mapViewportEdgeToLocalY(deltaY > 0) : anchorMoveDragHandler.lastPoint.y))
            }
        }
    }

    DispatchedDragHandler {
        id: freeRangeDragHandler
        property int originPosition: 0
        property bool oldHasSelection: false
        property int oldStart: 0
        property int oldEnd: 0

        function updateRange(x) {
            const position = Math.max(0, timeManipulator.mapToPosition(x))
            parameterEditor.freeParameterSelectionViewModel?.setRange(originPosition, position)
        }

        onDragStarted: x => {
            oldHasSelection = parameterEditor.freeParameterSelectionViewModel?.hasSelection ?? false
            oldStart = parameterEditor.freeParameterSelectionViewModel?.start ?? 0
            oldEnd = parameterEditor.freeParameterSelectionViewModel?.end ?? 0
            originPosition = Math.max(0, timeManipulator.mapToPosition(x))
            parameterEditor.interactionController?.freeRangeSelectingStarted(parameterEditor)
            updateRange(x)
        }
        onDragMoved: x => {
            freeRangeDragScroller.determine(x, width, 0, 0, triggeredX => {
                if (!triggeredX)
                    updateRange(x)
            })
        }
        onDragFinished: () => {
            freeRangeDragScroller.running = false
            parameterEditor.interactionController?.freeRangeSelectingCommitted(
                parameterEditor,
                parameterEditor.freeParameterSelectionViewModel?.start ?? 0,
                parameterEditor.freeParameterSelectionViewModel?.end ?? 0)
        }
        onDragCanceled: () => {
            freeRangeDragScroller.running = false
            if (oldHasSelection)
                parameterEditor.freeParameterSelectionViewModel?.setRange(oldStart, oldEnd)
            else
                parameterEditor.freeParameterSelectionViewModel?.clear()
            parameterEditor.interactionController?.freeRangeSelectingAborted(parameterEditor)
        }

        DragScroller {
            id: freeRangeDragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX)
                freeRangeDragHandler.updateRange(deltaX > 0 ? freeRangeDragHandler.width : 0)
            }
        }
    }

    component AnchorSelectionDragHandler: DispatchedDragHandler {
        id: anchorSelectionDragHandler
        property bool timeRange: false
        property int originPosition: 0
        property double originY: 0.0
        property point currentPoint: Qt.point(0, 0)
        property var oldSelection: []

        function updateRectangle(point) {
            currentPoint = point
            const originX = timeManipulator.mapToX(originPosition)
            anchorRubberBandRectangle.x = Math.min(originX, point.x)
            anchorRubberBandRectangle.width = Math.abs(originX - point.x)
            if (timeRange) {
                anchorRubberBandRectangle.y = 0
                anchorRubberBandRectangle.height = parameterEditor.height
            } else {
                anchorRubberBandRectangle.y = Math.min(originY, point.y)
                anchorRubberBandRectangle.height = Math.abs(originY - point.y)
            }
        }

        onDragStarted: (x, y, modifiers) => {
            originPosition = Math.max(0, timeManipulator.mapToPosition(x))
            originY = y
            currentPoint = Qt.point(x, y)
            oldSelection = parameterEditor.anchorSelectionController?.getSelectedItems() ?? []
            parameterEditor.anchorSelectionController?.selectByMouse(null, Qt.LeftButton, modifiers)
            anchorRubberBandRectangle.visible = true
            updateRectangle(currentPoint)
            parameterEditor.interactionController?.anchorRubberBandDraggingStarted(parameterEditor)
        }
        onDragMoved: (x, y) => {
            anchorSelectionDragScroller.determine(x, width, 0, 0, triggeredX => {
                if (!triggeredX)
                    updateRectangle(Qt.point(x, y))
            })
        }
        onDragFinished: () => {
            anchorSelectionDragScroller.running = false
            const items = timeRange
                ? content.anchorsInTimeRange(originPosition,
                                             Math.max(0, timeManipulator.mapToPosition(currentPoint.x)))
                : content.anchorsInRect(Qt.rect(anchorRubberBandRectangle.x,
                                                anchorRubberBandRectangle.y,
                                                anchorRubberBandRectangle.width,
                                                anchorRubberBandRectangle.height))
            if (parameterEditor.anchorSelectionController) {
                for (const item of items)
                    parameterEditor.anchorSelectionController.select(item, SelectionController.Toggle)
            }
            anchorRubberBandRectangle.visible = false
            oldSelection = []
            parameterEditor.interactionController?.anchorRubberBandDraggingCommitted(parameterEditor)
        }
        onDragCanceled: () => {
            anchorSelectionDragScroller.running = false
            anchorRubberBandRectangle.visible = false
            if (parameterEditor.anchorSelectionController) {
                parameterEditor.anchorSelectionController.select(null, SelectionController.ClearPreviousSelection)
                for (const item of oldSelection)
                    parameterEditor.anchorSelectionController.select(item, SelectionController.Select)
            }
            oldSelection = []
            parameterEditor.interactionController?.anchorRubberBandDraggingAborted(parameterEditor)
        }

        DragScroller {
            id: anchorSelectionDragScroller
            onMoved: deltaX => {
                timeManipulator.moveViewBy(deltaX)
                anchorSelectionDragHandler.updateRectangle(Qt.point(
                    deltaX > 0 ? anchorSelectionDragHandler.width : 0,
                    anchorSelectionDragHandler.currentPoint.y))
            }
        }
    }

    AnchorSelectionDragHandler {
        id: anchorRubberBandDragHandler
    }

    AnchorSelectionDragHandler {
        id: anchorTimeRangeDragHandler
        timeRange: true
    }

    DispatcherMouseArea {
        id: dispatcher
        anchors.fill: parent
        z: 3

        determineDragHandler: mouse => {
            helper.pressedAnchor = content.anchorAt(Qt.point(mouse.x, mouse.y), 8)
            helper.pressedInteraction = helper.interactionFor(mouse, helper.pressedAnchor)
            switch (helper.pressedInteraction) {
            case ParameterEditorInteractionController.Pencil:
                return pencilDragHandler
            case ParameterEditorInteractionController.Line:
                return lineDrawDragHandler
            case ParameterEditorInteractionController.Brush:
                return brushDragHandler
            case ParameterEditorInteractionController.Eraser:
                return eraserDragHandler
            case ParameterEditorInteractionController.Pointer:
                return helper.pressedAnchor ? anchorMoveDragHandler : anchorRubberBandDragHandler
            case ParameterEditorInteractionController.Pen:
                return helper.pressedAnchor ? anchorMoveDragHandler : null
            case ParameterEditorInteractionController.FreeRangeSelect:
                return freeRangeDragHandler
            case ParameterEditorInteractionController.AnchorRubberBandSelect:
                return anchorRubberBandDragHandler
            case ParameterEditorInteractionController.AnchorTimeRangeSelect:
                return anchorTimeRangeDragHandler
            default:
                return null
            }
        }

        onClicked: mouse => {
            const controller = parameterEditor.interactionController
            const item = content.anchorAt(Qt.point(mouse.x, mouse.y), 8)
            switch (helper.pressedInteraction) {
            case ParameterEditorInteractionController.Pointer:
                if (controller?.clickSelectable && parameterEditor.anchorSelectionController)
                    parameterEditor.anchorSelectionController.selectByMouse(item, Qt.LeftButton, mouse.modifiers)
                break
            case ParameterEditorInteractionController.Pen:
                if (item) {
                    helper.deleteAnchorItems(item)
                } else {
                    const position = Math.max(0, timeManipulator.mapToPosition(mouse.x))
                    const value = content.transformedValueFromPoint(Qt.point(mouse.x, mouse.y))
                    if (value === undefined || value === null)
                        break
                    controller?.anchorInsertionStarted(parameterEditor, position, value)
                    if (controller?.clickSelectable && parameterEditor.anchorSelectionController)
                        parameterEditor.anchorSelectionController.selectByMouse(null,
                                                                                 Qt.LeftButton,
                                                                                 0)
                    const newItem = controller?.createAndInsertAnchor(parameterEditor.anchorParameterViewModel,
                                                                      position, value) ?? null
                    if (newItem) {
                        controller?.anchorInsertionCommitted(parameterEditor, newItem)
                        if (controller?.clickSelectable && parameterEditor.anchorSelectionController)
                            parameterEditor.anchorSelectionController.selectByMouse(newItem,
                                                                                     Qt.LeftButton,
                                                                                     0)
                    } else {
                        controller?.anchorInsertionAborted(parameterEditor)
                    }
                }
                break
            case ParameterEditorInteractionController.ConvertAnchor:
                if (item)
                    helper.convertAnchorItems(item)
                break
            case ParameterEditorInteractionController.AnchorRubberBandSelect:
            case ParameterEditorInteractionController.AnchorTimeRangeSelect:
                if (controller?.clickSelectable && parameterEditor.anchorSelectionController)
                    parameterEditor.anchorSelectionController.selectByMouse(item, Qt.LeftButton, mouse.modifiers)
                break
            case ParameterEditorInteractionController.FreeRangeSelect:
                parameterEditor.freeParameterSelectionViewModel?.clear()
                break
            default:
                break
            }
            helper.pressedAnchor = null
        }
    }

    HoverHandler {
        id: hoverHandler
        enabled: parameterEditor.enabled
        onHoveredChanged: {
            if (!hovered) {
                if (helper.hoveredAnchor)
                    parameterEditor.interactionController?.itemHoverExited(parameterEditor, helper.hoveredAnchor)
                helper.hoveredAnchor = null
                parameterEditor.interactionController?.hoverExited(parameterEditor)
            } else {
                const position = Math.max(0, timeManipulator.mapToPosition(point.position.x))
                const transformedValue = 1.0 - point.position.y / parameterEditor.height
                parameterEditor.interactionController?.hoverEntered(parameterEditor, position, transformedValue)
            }
        }
        onPointChanged: {
            if (!hovered)
                return
            const item = content.anchorAt(point.position, 8)
            if (item !== helper.hoveredAnchor) {
                if (helper.hoveredAnchor)
                    parameterEditor.interactionController?.itemHoverExited(parameterEditor, helper.hoveredAnchor)
                helper.hoveredAnchor = item
                if (helper.hoveredAnchor)
                    parameterEditor.interactionController?.itemHoverEntered(parameterEditor, helper.hoveredAnchor)
            }
            parameterEditor.interactionController?.hoverMoved(
                parameterEditor,
                Math.max(0, timeManipulator.mapToPosition(point.position.x)),
                1.0 - point.position.y / parameterEditor.height)
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        z: 4
        onClicked: mouse => {
            const item = content.anchorAt(Qt.point(mouse.x, mouse.y), 8)
            if (item) {
                if (parameterEditor.interactionController?.clickSelectable && parameterEditor.anchorSelectionController)
                    parameterEditor.anchorSelectionController.selectByMouse(item, Qt.RightButton, mouse.modifiers)
                parameterEditor.interactionController?.itemContextMenuRequested(parameterEditor, item)
            } else {
                parameterEditor.interactionController?.contextMenuRequested(
                    parameterEditor,
                    Math.max(0, timeManipulator.mapToPosition(mouse.x)),
                    1.0 - mouse.y / parameterEditor.height)
            }
        }
    }
}
