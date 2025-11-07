import QtQuick

Item {
    id: handler

    property int movableOrientation: Qt.Horizontal | Qt.Vertical
    property int pinchZoomOrientationHint: zoomableOrientation
    property QtObject viewModel: null
    property int zoomableOrientation: movableOrientation

    signal moved(x: double, y: double, isPhysicalWheel: bool)
    signal zoomed(ratioX: double, ratioY: double, x: double, y: double, isPhysicalWheel: bool)

    MouseArea {
        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        cursorShape: undefined

        onWheel: function (wheel) {
            let isAlternateAxis = handler.viewModel?.isAlternateAxis(wheel.modifiers) ?? false
            let isZoom = handler.viewModel?.isZoom(wheel.modifiers) ?? false
            let isPage = handler.viewModel?.isPage(wheel.modifiers) ?? false

            let deltaPixelX = isAlternateAxis ? wheel.pixelDelta.y : wheel.pixelDelta.x
            let deltaPixelY = isAlternateAxis ? wheel.pixelDelta.x : wheel.pixelDelta.y

            let deltaX = (isAlternateAxis ? wheel.angleDelta.y : wheel.angleDelta.x) / 120
            let deltaY = (isAlternateAxis ? wheel.angleDelta.x : wheel.angleDelta.y) / 120

            let wheelHint = (!deltaPixelX && Math.abs(deltaX - Math.floor(deltaX)) < Number.EPSILON) && (!deltaPixelY && Math.abs(deltaY - Math.floor(deltaY)) < Number.EPSILON);

            if (isZoom) {
                handler.zoomed(
                    (handler.zoomableOrientation & Qt.Horizontal) ? Math.pow(1 + (isPage ? 2.5 : 0.25) * Math.abs(deltaX), Math.sign(deltaX)) : 1,
                    (handler.zoomableOrientation & Qt.Vertical) ? Math.pow(1 + (isPage ? 3 : 0.3) * Math.abs(deltaY), Math.sign(deltaY)) : 1,
                    wheel.x,
                    wheel.y,
                    wheelHint
                );
            } else {
                if (!deltaPixelX)
                    deltaPixelX = isPage ? Math.sign(deltaX) * handler.width : 0.125 * deltaX * handler.width;
                if (!deltaPixelY)
                    deltaPixelY = isPage ? Math.sign(deltaY) * handler.height : 0.2 * deltaY * handler.height;
                handler.moved(
                    (handler.movableOrientation & Qt.Horizontal) ? -deltaPixelX : 0,
                    (handler.movableOrientation & Qt.Vertical) ? -deltaPixelY : 0,
                    wheelHint
                );
            }
        }
    }
    PinchArea {
        anchors.fill: parent

        onPinchUpdated: pinch => {
            let scale = pinch.scale / pinch.previousScale;
            let acceptHorizontal = (handler.zoomableOrientation & Qt.Horizontal);
            let acceptVertical = (handler.zoomableOrientation & Qt.Vertical);
            if (acceptHorizontal && acceptVertical) {
                if (handler.viewModel?.pinchDecomposed)
                    handler.zoomed(scale, scale, pinch.center.x, pinch.center.y, false);
                else
                    handler.zoomed((handler.pinchZoomOrientationHint & Qt.Horizontal) ? scale : 1, (handler.pinchZoomOrientationHint & Qt.Vertical) ? scale : 1, pinch.center.x, pinch.center.y, false);
            } else if (acceptHorizontal) {
                handler.zoomed(scale, 1, pinch.center.x, pinch.center.y, false);
            } else if (acceptVertical) {
                handler.zoomed(1, scale, pinch.center.x, pinch.center.y, false);
            }
        }
    }
}
