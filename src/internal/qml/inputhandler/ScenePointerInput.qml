import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: sceneInput

    anchors.fill: parent

    property var controller: null
    property SelectionController selectionController: null
    property Item paneItem: parent
    property Item coordinateSpace: paneItem
    property TimeManipulator timeManipulator: null
    property var verticalManipulator: null
    property var dispatchMap: ({})
    property var clickInterceptor: null
    property bool freezeCursorOnPress: false
    readonly property alias router: router

    function resolveHandler(event, hit): DispatchedDragHandler {
        if (!controller)
            return null
        let interaction
        if (event.modifiers & Qt.ControlModifier) {
            interaction = event.modifiers & Qt.AltModifier
                ? controller.secondarySelectInteraction
                : controller.primarySelectInteraction
        } else if (hit.target) {
            interaction = event.modifiers & Qt.AltModifier
                ? controller.secondaryItemInteraction
                : controller.primaryItemInteraction
        } else {
            interaction = event.modifiers & Qt.AltModifier
                ? controller.secondarySceneInteraction
                : controller.primarySceneInteraction
        }
        return dispatchMap[interaction] ?? null
    }

    function scenePosition(point: point): var {
        if (timeManipulator && verticalManipulator) {
            return [timeManipulator.mapToPosition(point.x), verticalManipulator.mapToPosition(point.y)]
        }
        if (timeManipulator)
            return [timeManipulator.mapToPosition(point.x)]
        if (verticalManipulator)
            return [verticalManipulator.mapToPosition(point.y)]
        return []
    }

    PointerInteractionRouter {
        id: router
    }

    PointerInputArea {
        anchors.fill: parent
        router: sceneInput.router
        coordinateSpace: sceneInput.coordinateSpace
        handlerResolver: sceneInput.resolveHandler
        freezeCursorOnPress: sceneInput.freezeCursorOnPress
        hitResolver: (surfacePoint, _) => ({
            valid: true,
            target: null,
            targetRect: Qt.rect(0, 0, sceneInput.coordinateSpace?.width ?? 0,
                                sceneInput.coordinateSpace?.height ?? 0),
            hoverRegion: 0,
            payload: undefined,
        })
    }

    Connections {
        target: sceneInput.router

        function onClicked(event, hit) {
            if (sceneInput.clickInterceptor
                    && sceneInput.clickInterceptor(event, hit, false))
                return
            if (sceneInput.controller?.clickSelectable && sceneInput.selectionController) {
                sceneInput.selectionController.selectByPointer(
                    hit.target, SelectionController.PrimarySelection, event.modifiers)
            }
        }

        function onDoubleClicked(event, hit) {
            if (sceneInput.clickInterceptor
                    && sceneInput.clickInterceptor(event, hit, true))
                return
            if (!sceneInput.controller)
                return
            if (hit.target) {
                sceneInput.controller.itemDoubleClicked(sceneInput.paneItem, hit.target)
                return
            }
            const position = sceneInput.scenePosition(event.position)
            if (position.length === 2) {
                sceneInput.controller.doubleClicked(sceneInput.paneItem, position[0], position[1])
            } else if (position.length === 1) {
                sceneInput.controller.doubleClicked(sceneInput.paneItem, position[0])
            } else {
                sceneInput.controller.doubleClicked(sceneInput.paneItem)
            }
        }

        function onContextMenuRequested(event, hit) {
            if (hit.target) {
                if (sceneInput.controller?.clickSelectable
                        && sceneInput.selectionController) {
                    sceneInput.selectionController.selectByPointer(
                        hit.target, SelectionController.ContextSelection, event.modifiers)
                }
            } else {
                sceneInput.selectionController?.select(
                    null, SelectionController.ClearPreviousSelection)
            }
            if (!sceneInput.controller)
                return
            if (hit.target) {
                sceneInput.controller.itemContextMenuRequested(sceneInput.paneItem, hit.target)
                return
            }
            const position = sceneInput.scenePosition(event.position)
            if (position.length === 2) {
                sceneInput.controller.contextMenuRequested(sceneInput.paneItem, position[0], position[1])
            } else if (position.length === 1) {
                sceneInput.controller.contextMenuRequested(sceneInput.paneItem, position[0])
            } else {
                sceneInput.controller.contextMenuRequested(sceneInput.paneItem)
            }
        }

        function onHoverEntered(event, hit) {
            if (!sceneInput.controller)
                return
            if (hit.target) {
                sceneInput.controller.itemHoverEntered(sceneInput.paneItem, hit.target)
                return
            }
            const position = sceneInput.scenePosition(event.position)
            if (position.length === 2) {
                sceneInput.controller.hoverEntered(sceneInput.paneItem, position[0], position[1])
            } else if (position.length === 1) {
                sceneInput.controller.hoverEntered(sceneInput.paneItem, position[0])
            } else {
                sceneInput.controller.hoverEntered(sceneInput.paneItem)
            }
        }

        function onHoverMoved(event, hit) {
            if (!sceneInput.controller || hit.target)
                return
            const position = sceneInput.scenePosition(event.position)
            if (position.length === 2) {
                sceneInput.controller.hoverMoved(sceneInput.paneItem, position[0], position[1])
            } else if (position.length === 1) {
                sceneInput.controller.hoverMoved(sceneInput.paneItem, position[0])
            }
        }

        function onHoverExited(hit) {
            if (!sceneInput.controller)
                return
            if (hit.target) {
                sceneInput.controller.itemHoverExited(sceneInput.paneItem, hit.target)
            } else {
                sceneInput.controller.hoverExited(sceneInput.paneItem)
            }
        }
    }
}
