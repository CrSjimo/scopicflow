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
    property int freeEditPointerPosition: 0
    property double freeEditPointerValue: 0.0
    readonly property alias pointerTimePosition: helper.pointerTimePosition
    readonly property alias pointerTransformedValue: helper.pointerTransformedValue

    onFreeParameterViewModelChanged: pointerRouter.cancel()
    onAnchorParameterViewModelChanged: pointerRouter.cancel()

    function hasPointerHover(): bool {
        return pointerRouter.hasHover()
    }

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

        property int pressedInteraction: ParameterEditorInteractionController.None
        property int pointerTimePosition: 0
        property double pointerTransformedValue: 0.0

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

        function freeEditPointerAt(point: point): var {
            const value = content.transformedValueFromPoint(point)
            return {
                position: Math.max(0, timeManipulator.mapToPosition(point.x)),
                value: value === undefined || value === null ? 0.0 : value,
            }
        }

        function setFreeEditPointer(pointer) {
            parameterEditor.freeEditPointerPosition = pointer.position
            parameterEditor.freeEditPointerValue = pointer.value
        }

        function updateFreeEditPointer(point: point) {
            setFreeEditPointer(freeEditPointerAt(point))
        }

        function updateHoverPointer(event) {
            pointerTimePosition = timeManipulator.mapToPosition(event.position.x)
            pointerTransformedValue =
                1.0 - event.position.y / parameterEditor.height
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
                selectionController.selectByPointer(
                    item, SelectionController.ContextSelection, 0)
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
        activationPolicy: DispatchedDragHandler.Immediately

        function drawTo(point) {
            if (operation === ParameterEditorInteractionController.BrushFree)
                content.brushFreeSegment(lastPoint, point)
            else
                content.drawFreeSegment(lastPoint, point,
                    operation === ParameterEditorInteractionController.EraseFree)
            lastPoint = point
            helper.updateFreeEditPointer(point)
            parameterEditor.interactionController?.freeEditingUpdated(
                parameterEditor, operation,
                parameterEditor.freeEditPointerPosition,
                parameterEditor.freeEditPointerValue)
        }

        onStarted: event => {
            lastPoint = event.position
            const pointer = helper.freeEditPointerAt(lastPoint)
            parameterEditor.interactionController?.freeEditingStarted(
                parameterEditor, operation, pointer.position, pointer.value)
            helper.setFreeEditPointer(pointer)
            if (operation === ParameterEditorInteractionController.BrushFree)
                content.brushFreeSegment(lastPoint, lastPoint)
            else
                content.drawFreeSegment(lastPoint, lastPoint,
                    operation === ParameterEditorInteractionController.EraseFree)
        }
        onMoved: event => {
            const point = event.position
            freeEditDragScroller.determine(point.x, event.surfaceRect.width,
                                           parameterEditor.mapLocalYToViewport(point.y),
                                           parameterEditor.verticalViewport?.height ?? 0,
                                           (triggeredX, triggeredY) => {
                if (!triggeredX && !triggeredY)
                    drawTo(point)
            })
        }
        onFinished: {
            freeEditDragScroller.running = false
            parameterEditor.interactionController?.freeEditingCommitted(parameterEditor, operation)
        }
        onCanceled: {
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
                        deltaX > 0 ? freeEditHandler.surfaceRect.width : deltaX < 0 ? 0 : freeEditHandler.lastPoint.x,
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
        property point lineStartPoint: Qt.point(0, 0)
        property point lastPoint: Qt.point(0, 0)
        readonly property int operation: ParameterEditorInteractionController.DrawLineFree
        activationPolicy: DispatchedDragHandler.Immediately

        function drawTo(point) {
            content.updateFreeLine(point)
            lastPoint = point
            helper.updateFreeEditPointer(point)
            parameterEditor.interactionController?.freeEditingUpdated(
                parameterEditor, operation,
                parameterEditor.freeEditPointerPosition,
                parameterEditor.freeEditPointerValue)
        }

        onStarted: event => {
            lineStartPoint = lastPoint = event.position
            const pointer = helper.freeEditPointerAt(lineStartPoint)
            content.beginFreeLine(lineStartPoint)
            parameterEditor.interactionController?.freeEditingStarted(
                parameterEditor, operation, pointer.position, pointer.value)
            helper.setFreeEditPointer(pointer)
        }
        onMoved: event => {
            const point = event.position
            lineDrawDragScroller.determine(point.x, event.surfaceRect.width,
                                           parameterEditor.mapLocalYToViewport(point.y),
                                           parameterEditor.verticalViewport?.height ?? 0,
                                           (triggeredX, triggeredY) => {
                if (!triggeredX && !triggeredY)
                    drawTo(point)
            })
        }
        onFinished: {
            lineDrawDragScroller.running = false
            content.commitFreeLine()
            parameterEditor.interactionController?.freeEditingCommitted(parameterEditor, operation)
        }
        onCanceled: {
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
                        deltaX > 0 ? lineDrawDragHandler.surfaceRect.width : deltaX < 0 ? 0 : lineDrawDragHandler.lastPoint.x,
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

        onStarted: (event, hit) => {
            item = hit.target
            lastPoint = event.position
            const movingItems = helper.anchorOperationItems(item)
            if (!item || !content.beginAnchorMove(movingItems, item, lastPoint)) {
                item = null
                return
            }
            parameterEditor.interactionController?.anchorMovingStarted(parameterEditor, item)
        }
        onMoved: event => {
            const point = event.position
            anchorMoveDragScroller.determine(point.x, event.surfaceRect.width,
                                             parameterEditor.mapLocalYToViewport(point.y),
                                             parameterEditor.verticalViewport?.height ?? 0,
                                             (triggeredX, triggeredY) => {
                if (!triggeredX && !triggeredY)
                    moveTo(point)
            })
        }
        onFinished: {
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
        onCanceled: {
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
                        deltaX > 0 ? anchorMoveDragHandler.surfaceRect.width : deltaX < 0 ? 0 : anchorMoveDragHandler.lastPoint.x,
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

        onStarted: event => {
            oldHasSelection = parameterEditor.freeParameterSelectionViewModel?.hasSelection ?? false
            oldStart = parameterEditor.freeParameterSelectionViewModel?.start ?? 0
            oldEnd = parameterEditor.freeParameterSelectionViewModel?.end ?? 0
            originPosition = Math.max(0, timeManipulator.mapToPosition(event.position.x))
            parameterEditor.interactionController?.freeRangeSelectingStarted(parameterEditor)
            updateRange(event.position.x)
        }
        onMoved: event => {
            freeRangeDragScroller.determine(
                event.position.x, event.surfaceRect.width, 0, 0, triggeredX => {
                if (!triggeredX)
                    updateRange(event.position.x)
            })
        }
        onFinished: {
            freeRangeDragScroller.running = false
            parameterEditor.interactionController?.freeRangeSelectingCommitted(
                parameterEditor,
                parameterEditor.freeParameterSelectionViewModel?.start ?? 0,
                parameterEditor.freeParameterSelectionViewModel?.end ?? 0)
        }
        onCanceled: {
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
                freeRangeDragHandler.updateRange(
                    deltaX > 0 ? freeRangeDragHandler.surfaceRect.width : 0)
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

        onStarted: event => {
            originPosition = Math.max(0, timeManipulator.mapToPosition(event.position.x))
            originY = event.position.y
            currentPoint = event.position
            oldSelection = parameterEditor.anchorSelectionController?.getSelectedItems() ?? []
            parameterEditor.anchorSelectionController?.selectByPointer(
                null, SelectionController.PrimarySelection, event.modifiers)
            anchorRubberBandRectangle.visible = true
            updateRectangle(currentPoint)
            parameterEditor.interactionController?.anchorRubberBandDraggingStarted(parameterEditor)
        }
        onMoved: event => {
            anchorSelectionDragScroller.determine(
                event.position.x, event.surfaceRect.width, 0, 0, triggeredX => {
                if (!triggeredX)
                    updateRectangle(event.position)
            })
        }
        onFinished: {
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
        onCanceled: {
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
                    deltaX > 0 ? anchorSelectionDragHandler.surfaceRect.width : 0,
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

    PointerInteractionRouter {
        id: pointerRouter
    }

    PointerInputArea {
        id: pointerInputArea

        anchors.fill: parent
        router: pointerRouter
        coordinateSpace: parameterEditor
        z: 4

        hitResolver: (point, _) => ({
            valid: true,
            target: content.anchorAt(point, 8),
            targetRect: Qt.rect(0, 0, 0, 0),
            hoverRegion: 0,
            payload: undefined,
        })

        handlerResolver: (event, hit) => {
            const interaction = helper.interactionFor(event, hit.target)
            helper.pressedInteraction = interaction
            hit.payload = interaction
            switch (interaction) {
            case ParameterEditorInteractionController.Pencil:
                return pencilDragHandler
            case ParameterEditorInteractionController.Line:
                return lineDrawDragHandler
            case ParameterEditorInteractionController.Brush:
                return brushDragHandler
            case ParameterEditorInteractionController.Eraser:
                return eraserDragHandler
            case ParameterEditorInteractionController.Pointer:
                return hit.target ? anchorMoveDragHandler : anchorRubberBandDragHandler
            case ParameterEditorInteractionController.Pen:
                return hit.target ? anchorMoveDragHandler : null
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
    }

    Connections {
        target: pointerRouter

        function onClicked(event, hit) {
            const controller = parameterEditor.interactionController
            const item = hit.target
            const point = event.position
            const interaction = hit.payload
                ?? helper.interactionFor(event, item)
            switch (interaction) {
            case ParameterEditorInteractionController.Pointer:
                if (controller?.clickSelectable && parameterEditor.anchorSelectionController)
                    parameterEditor.anchorSelectionController.selectByPointer(
                        item, SelectionController.PrimarySelection, event.modifiers)
                break
            case ParameterEditorInteractionController.Pen:
                if (item) {
                    helper.deleteAnchorItems(item)
                } else {
                    const position = Math.max(0, timeManipulator.mapToPosition(point.x))
                    const value = content.transformedValueFromPoint(point)
                    if (value === undefined || value === null)
                        break
                    controller?.anchorInsertionStarted(parameterEditor, position, value)
                    if (controller?.clickSelectable && parameterEditor.anchorSelectionController)
                        parameterEditor.anchorSelectionController.selectByPointer(
                            null, SelectionController.PrimarySelection, 0)
                    const newItem = controller?.createAndInsertAnchor(parameterEditor.anchorParameterViewModel,
                                                                      position, value) ?? null
                    if (newItem) {
                        controller?.anchorInsertionCommitted(parameterEditor, newItem)
                        if (controller?.clickSelectable && parameterEditor.anchorSelectionController)
                            parameterEditor.anchorSelectionController.selectByPointer(
                                newItem, SelectionController.PrimarySelection, 0)
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
                    parameterEditor.anchorSelectionController.selectByPointer(
                        item, SelectionController.PrimarySelection, event.modifiers)
                break
            case ParameterEditorInteractionController.FreeRangeSelect:
                parameterEditor.freeParameterSelectionViewModel?.clear()
                break
            default:
                break
            }
        }

        function onContextMenuRequested(event, hit) {
            const controller = parameterEditor.interactionController
            if (hit.target) {
                if (controller?.clickSelectable && parameterEditor.anchorSelectionController) {
                    parameterEditor.anchorSelectionController.selectByPointer(
                        hit.target, SelectionController.ContextSelection, event.modifiers)
                }
                controller?.itemContextMenuRequested(parameterEditor, hit.target)
            } else {
                parameterEditor.anchorSelectionController?.select(
                    null, SelectionController.ClearPreviousSelection)
                controller?.contextMenuRequested(
                    parameterEditor,
                    Math.max(0, timeManipulator.mapToPosition(event.position.x)),
                    1.0 - event.position.y / parameterEditor.height)
            }
        }

        function onHoverEntered(event, hit) {
            helper.updateHoverPointer(event)
            if (hit.target) {
                parameterEditor.interactionController?.itemHoverEntered(
                    parameterEditor, hit.target)
            } else {
                parameterEditor.interactionController?.hoverEntered(
                    parameterEditor,
                    timeManipulator.mapToPosition(event.position.x),
                    1.0 - event.position.y / parameterEditor.height)
            }
        }

        function onHoverMoved(event, hit) {
            helper.updateHoverPointer(event)
            if (!hit.target) {
                parameterEditor.interactionController?.hoverMoved(
                    parameterEditor,
                    timeManipulator.mapToPosition(event.position.x),
                    1.0 - event.position.y / parameterEditor.height)
            }
        }

        function onHoverExited(hit) {
            if (hit.target) {
                parameterEditor.interactionController?.itemHoverExited(
                    parameterEditor, hit.target)
            } else {
                parameterEditor.interactionController?.hoverExited(parameterEditor)
            }
        }
    }
}
