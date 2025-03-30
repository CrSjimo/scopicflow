import QtQuick

Item {
    id: handler

    property QtObject viewModel: null
    property int movableOrientation: Qt.Horizontal | Qt.Vertical
    property int zoomableOrientation: movableOrientation
    property int pinchZoomOrientationHint: zoomableOrientation

    signal zoomed(ratioX: double, ratioY: double, x: double, y: double, isPhysicalWheel: bool)
    signal moved(x: double, y: double, isPhysicalWheel: bool)

    MouseArea {
        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        cursorShape: undefined

        readonly property var alternateAxisModifier: handler.viewModel? handler.viewModel.alternateAxisModifier : Qt.AltModifier
        readonly property var zoomModifier: handler.viewModel? handler.viewModel.zoomModifier : Qt.ControlModifier
        readonly property var pageModifier: handler.viewModel? handler.viewModel.pageModifier : Qt.ShiftModifier

        onWheel: function (wheel) {
            let isWindows = Qt.platform.os === "windows"
            let isAxisRevert = isWindows && (wheel.modifiers & Qt.AltModifier)
            let isAlternateAxis = Boolean(wheel.modifiers & alternateAxisModifier) || Boolean(wheel.modifiers & pageModifier) && handler.viewModel?.usePageModifierAsAlternateAxisZoom
            let isZoom = Boolean(wheel.modifiers & zoomModifier) || Boolean(wheel.modifiers & pageModifier) && handler.viewModel?.usePageModifierAsAlternateAxisZoom
            let isPage = Boolean(wheel.modifiers & pageModifier) && !handler.viewModel?.usePageModifierAsAlternateAxisZoom

            let deltaPixelX = isAlternateAxis ? (isAxisRevert ? wheel.pixelDelta.x : wheel.pixelDelta.y) : (isAxisRevert ? wheel.pixelDelta.y : wheel.pixelDelta.x)
            let deltaPixelY = !isAlternateAxis ? (isAxisRevert ? wheel.pixelDelta.x : wheel.pixelDelta.y) : (isAxisRevert ? wheel.pixelDelta.y : wheel.pixelDelta.x)

            let deltaX = (isAlternateAxis ? (isAxisRevert ? wheel.angleDelta.x : wheel.angleDelta.y) : (isAxisRevert ? wheel.angleDelta.y : wheel.angleDelta.x)) / 120
            let deltaY = (!isAlternateAxis ? (isAxisRevert ? wheel.angleDelta.x : wheel.angleDelta.y) : (isAxisRevert ? wheel.angleDelta.y : wheel.angleDelta.x)) / 120

            let wheelHint = (!deltaPixelX && Math.abs(deltaX - Math.floor(deltaX)) < Number.EPSILON) && (!deltaPixelY && Math.abs(deltaY - Math.floor(deltaY)) < Number.EPSILON)

            if (isZoom) {
                handler.zoomed(
                    Math.pow(1 + (isPage ? 2.5 : 0.25) * Math.abs(deltaX), Math.sign(deltaX)),
                    Math.pow(1 + (isPage ? 3 : 0.3) * Math.abs(deltaY), Math.sign(deltaY)),
                    wheel.x, wheel.y, wheelHint)
            } else {
                if (!deltaPixelX)
                    deltaPixelX = isPage ? Math.sign(deltaX) * handler.width : 0.125 * deltaX * handler.width
                if (!deltaPixelY)
                    deltaPixelY = isPage ? Math.sign(deltaY) * handler.height : 0.2 * deltaY * handler.height
                handler.moved(-deltaPixelX, -deltaPixelY, wheelHint)
            }

        }
    }

}