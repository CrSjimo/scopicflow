import QtQuick

Item {

    property var viewModel: null

    signal zoomed(ratioX: double, ratioY: double, x: double, y: double, isPhysicalWheel: bool)
    signal moved(x: double, y: double, isPhysicalWheel: bool)

    MouseArea {
        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        cursorShape: undefined

        readonly property var alternateAxisModifier: parent.viewModel? parent.viewModel.alternateAxisModifier : Qt.AltModifier
        readonly property var zoomModifier: parent.viewModel? parent.viewModel.zoomModifier : Qt.ControlModifier
        readonly property var pageModifier: parent.viewModel? parent.viewModel.pageModifier : Qt.ShiftModifier

        onWheel: function (wheel) {
            let isAxisRevert = wheel.modifiers & Qt.AltModifier
            let isAlternateAxis = Boolean(wheel.modifiers & alternateAxisModifier) || Boolean(wheel.modifiers & pageModifier) && parent.viewModel?.usePageModifierAsAlternateAxisZoom
            let isZoom = Boolean(wheel.modifiers & zoomModifier) || Boolean(wheel.modifiers & pageModifier) && parent.viewModel?.usePageModifierAsAlternateAxisZoom
            let isPage = Boolean(wheel.modifiers & pageModifier) && !parent.viewModel?.usePageModifierAsAlternateAxisZoom

            let deltaPixelX = isAlternateAxis ? (isAxisRevert ? wheel.pixelDelta.x : wheel.pixelDelta.y) : (isAxisRevert ? wheel.pixelDelta.y : wheel.pixelDelta.x)
            let deltaPixelY = !isAlternateAxis ? (isAxisRevert ? wheel.pixelDelta.x : wheel.pixelDelta.y) : (isAxisRevert ? wheel.pixelDelta.y : wheel.pixelDelta.x)

            let deltaX = (isAlternateAxis ? (isAxisRevert ? wheel.angleDelta.x : wheel.angleDelta.y) : (isAxisRevert ? wheel.angleDelta.y : wheel.angleDelta.x)) / 120
            let deltaY = (!isAlternateAxis ? (isAxisRevert ? wheel.angleDelta.x : wheel.angleDelta.y) : (isAxisRevert ? wheel.angleDelta.y : wheel.angleDelta.x)) / 120

            let wheelHint = (!deltaPixelX && Math.abs(deltaX - Math.floor(deltaX)) < Number.EPSILON) && (!deltaPixelY && Math.abs(deltaY - Math.floor(deltaY)) < Number.EPSILON)

            if (isZoom) {
                parent.zoomed(
                    Math.pow(1 + (isPage ? 2.5 : 0.25) * Math.abs(deltaX), Math.sign(deltaX)),
                    Math.pow(1 + (isPage ? 3 : 0.3) * Math.abs(deltaY), Math.sign(deltaY)),
                    wheel.x, wheel.y, wheelHint)
            } else {
                if (!deltaPixelX)
                    deltaPixelX = isPage ? Math.sign(deltaX) * parent.width : 0.125 * deltaX * parent.width
                if (!deltaPixelY)
                    deltaPixelY = isPage ? Math.sign(deltaY) * parent.height : 0.2 * deltaY * parent.height
                parent.moved(-deltaPixelX, -deltaPixelY, wheelHint)
            }

        }
    }

    PinchArea {
        anchors.fill: parent
        property real sensitivity: 0.2

        onPinchUpdated: pinch => {
            let horizontalScale = Math.pow(pinch.scale, sensitivity)
            let verticalScale = 1
            parent.zoomed(horizontalScale, verticalScale, pinch.center.x, pinch.center.y, false)
        }
    }
}