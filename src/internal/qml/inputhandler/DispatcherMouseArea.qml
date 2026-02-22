import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents
import SVSCraft.UIComponents.impl

import dev.sjimo.ScopicFlow

FocusScope {
    id: dispatcherMouseArea
    anchors.fill: parent
    property var determineDragHandler: mouse => null
    signal clicked(MouseEvent event)
    signal doubleClicked(MouseEvent event)
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        cursorShape: undefined
        focus: true
        focusPolicy: Qt.ClickFocus
        property bool dragged: false
        property bool isDoubleClick: false
        property DispatchedDragHandler currentHandler: null
        property point pressedPoint: Qt.point(0, 0)
        onPressed: (mouse) => {
            dragged = false
            isDoubleClick = false
            currentHandler = dispatcherMouseArea.determineDragHandler(mouse)
            pressedPoint = Qt.point(mouse.x, mouse.y)
            if (currentHandler?.startDraggingImmediately) {
                dragged = true
                let handlerPoint = mapToItem(currentHandler, pressedPoint)
                currentHandler._startPoint = handlerPoint
                currentHandler._dragged = true
                currentHandler._modifiers = mouse.modifiers
                currentHandler.dragStarted(handlerPoint.x, handlerPoint.y, mouse.modifiers)
            }
        }
        onPositionChanged: (mouse) => {
            if (isDoubleClick)
                return
            if (!dragged) {
                if (currentHandler) {
                    dragged = true
                    let handlerPoint = mapToItem(currentHandler, pressedPoint)
                    currentHandler._startPoint = handlerPoint
                    currentHandler._dragged = true
                    currentHandler._modifiers = mouse.modifiers
                    currentHandler.dragStarted(handlerPoint.x, handlerPoint.y, mouse.modifiers)
                }
            }
            if (currentHandler) {
                let handlerPoint = mapToItem(currentHandler, mouse.x, mouse.y)
                currentHandler.dragMoved(handlerPoint.x, handlerPoint.y)
            }
        }
        onReleased: () => {
            if (dragged && currentHandler) {
                currentHandler._startPoint = Qt.point(0, 0)
                currentHandler._dragged = false
                currentHandler._modifiers = 0
                currentHandler.dragFinished()
            }
        }
        onCanceled: () => {
            if (dragged && currentHandler) {
                currentHandler._startPoint = Qt.point(0, 0)
                currentHandler._dragged = false
                currentHandler._modifiers = 0
                currentHandler.dragCanceled()
            }
        }
        onClicked: (mouse) => {
            if (!dragged) {
                dispatcherMouseArea.clicked(mouse)
            }
        }
        onDoubleClicked: (mouse) => {
            isDoubleClick = true
            if (!dragged) {
                dispatcherMouseArea.doubleClicked(mouse)
            }
        }
        Keys.onEscapePressed: () => {
            if (pressed) {
                GlobalHelper.ungrabMouse(mouseArea)
            }
        }
    }
}
