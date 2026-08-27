import QtQml
import QtQuick

import SVSCraft.UIComponents
import SVSCraft.UIComponents.impl

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: mouseArea

    required property PointerInteractionRouter router
    property Item coordinateSpace: parent
    property var hitResolver: (surfacePoint, localPoint) => ({
        valid: true,
        target: null,
        targetRect: Qt.rect(0, 0, 0, 0),
        hoverRegion: 0,
        payload: undefined,
    })
    property var handlerResolver: (event, hit) => null
    property var cursorResolver: null
    property var pressCursorResolver: cursorResolver
    property bool freezeCursorOnPress: false
    property bool clearHoverOnInvalidHit: false
    property var resolvedCursor: undefined
    property var pressCursor: undefined
    property bool pressCursorActive: false

    acceptedButtons: Qt.LeftButton | Qt.RightButton
    enabled: parent.enabled
    hoverEnabled: true
    focusPolicy: Qt.ClickFocus
    cursorShape: pressCursorActive ? pressCursor : resolvedCursor
    CursorBinding.enabled: freezeCursorOnPress && pressCursorActive
    CursorBinding.cursorShape: pressCursor ?? Qt.ArrowCursor

    function pointerButton(button): int {
        if (button === Qt.LeftButton)
            return 1
        if (button === Qt.RightButton)
            return 2
        if (button === Qt.MiddleButton)
            return 3
        return 0
    }

    function surfacePoint(x: double, y: double): point {
        return coordinateSpace ? mapToItem(coordinateSpace, x, y) : Qt.point(x, y)
    }

    function createEvent(mouse): var {
        return router.createPointerEvent(
            0,
            0,
            pointerButton(mouse.button),
            surfacePoint(mouse.x, mouse.y),
            coordinateSpace
                ? Qt.rect(0, 0, coordinateSpace.width, coordinateSpace.height)
                : Qt.rect(0, 0, width, height),
            mouse.modifiers)
    }

    function resolveHit(mouse): var {
        return resolveLocalHit(Qt.point(mouse.x, mouse.y))
    }

    function resolveLocalHit(localPoint: point): var {
        return hitResolver(
            surfacePoint(localPoint.x, localPoint.y), localPoint)
    }

    function createHit(hit): var {
        return router.createPointerHit(
            hit.valid ?? false,
            hit.target ?? null,
            hit.targetRect ?? Qt.rect(0, 0, 0, 0),
            hit.hoverRegion ?? 0,
            hit.payload ?? null)
    }

    function resolveCursor(hit): var {
        return cursorResolver ? cursorResolver(hit) : undefined
    }

    function resolvePressCursor(hit): var {
        return pressCursorResolver ? pressCursorResolver(hit) : undefined
    }

    function updateHover(mouse, event = undefined, resolvedHit = undefined) {
        event = event ?? createEvent(mouse)
        resolvedHit = resolvedHit ?? resolveHit(mouse)
        if (!(resolvedHit.valid ?? false) && clearHoverOnInvalidHit) {
            resolvedCursor = undefined
            router.leaveHover(mouseArea)
            return
        }
        if (!pressed)
            resolvedCursor = resolveCursor(resolvedHit)
        router.updateHover(mouseArea, event, createHit(resolvedHit))
    }

    function restoreHover() {
        pressCursorActive = false
        pressCursor = undefined
        if (containsMouse) {
            const mouse = {
                x: mouseX,
                y: mouseY,
                button: Qt.NoButton,
                modifiers: Qt.NoModifier,
            }
            const resolvedHit = resolveHit(mouse)
            resolvedCursor = resolveCursor(resolvedHit)
            updateHover(mouse, undefined, resolvedHit)
        } else {
            resolvedCursor = undefined
            router.leaveHover(mouseArea)
        }
    }

    onPressed: mouse => {
        const event = createEvent(mouse)
        const resolvedHit = resolveHit(mouse)
        resolvedCursor = resolveCursor(resolvedHit)
        pressCursor = resolvePressCursor(resolvedHit)
        pressCursorActive = true
        updateHover(mouse, event, resolvedHit)
        const handler = handlerResolver(event, resolvedHit)
        mouse.accepted = router.press(
            mouseArea, event, createHit(resolvedHit), handler)
        if (!mouse.accepted)
            restoreHover()
    }

    onPositionChanged: mouse => {
        if (containsMouse && !pressed)
            updateHover(mouse)
        if (pressed)
            router.move(mouseArea, createEvent(mouse))
    }

    onReleased: mouse => {
        router.release(mouseArea, createEvent(mouse))
        restoreHover()
    }
    onCanceled: {
        router.cancel(mouseArea)
        restoreHover()
    }
    onClicked: mouse => router.click(
        mouseArea, createEvent(mouse), createHit(resolveHit(mouse)))
    onDoubleClicked: mouse => router.doubleClick(
        mouseArea, createEvent(mouse), createHit(resolveHit(mouse)))
    onEntered: updateHover({
        x: mouseX,
        y: mouseY,
        button: Qt.NoButton,
        modifiers: Qt.NoModifier,
    })
    onExited: {
        if (!pressed)
            resolvedCursor = undefined
        router.leaveHover(mouseArea)
    }

    Keys.onEscapePressed: {
        router.cancel(mouseArea)
    }

    Connections {
        target: mouseArea.router

        function onInputCancellationRequested(source) {
            if (source === mouseArea) {
                mouseArea.restoreHover()
                GlobalHelper.ungrabMouse(mouseArea)
            }
        }
    }

    Component.onDestruction: {
        router.cancel(mouseArea)
        router.leaveHover(mouseArea)
    }
}
