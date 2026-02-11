import QtQml
import QtQuick
import QtQuick.Templates as T
import QtQuick.Shapes

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

FocusScope {
    id: handler

    anchors.fill: parent

    property ScrollBehaviorViewModel viewModel: null
    property int movableOrientation: Qt.Horizontal | Qt.Vertical
    property int zoomableOrientation: movableOrientation
    property int pinchOrientationHint: Qt.Horizontal
    property bool horizontalScrollBarEnabled: false
    property double horizontalScrollX: 0
    property double horizontalScrollWidth: 0
    property bool verticalScrollBarEnabled: false
    property double verticalScrollY: 0
    property double verticalScrollHeight: 0

    readonly property T.ScrollBar horizontalScrollBar: horizontalScrollBar
    readonly property T.ScrollBar verticalScrollBar: verticalScrollBar

    signal moved(x: double, y: double, isPhysicalWheel: bool)
    signal zoomed(ratioX: double, ratioY: double, x: double, y: double, isPhysicalWheel: bool)

    // Wheel
    MouseArea {
        acceptedButtons: Qt.NoButton
        anchors.fill: parent
        cursorShape: undefined
        enabled: (handler.viewModel?.scrollTypes ?? 0) & ScrollBehaviorViewModel.Wheel

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

    // Pinch
    PinchArea {
        anchors.fill: parent
        enabled: (handler.viewModel?.scrollTypes ?? 0) & ScrollBehaviorViewModel.Pinch

        onPinchUpdated: pinch => {
            let scale = pinch.scale / pinch.previousScale
            let acceptHorizontal = (handler.zoomableOrientation & Qt.Horizontal)
            let acceptVertical = (handler.zoomableOrientation & Qt.Vertical)
            if (acceptHorizontal && acceptVertical) {
                handler.zoomed(
                    pinchOrientationHint === Qt.Horizontal ? scale : 1,
                    pinchOrientationHint === Qt.Vertical ? scale : 1,
                    pinch.center.x,
                    pinch.center.y,
                    false
                )
            } else if (acceptHorizontal) {
                handler.zoomed(scale, 1, pinch.center.x, pinch.center.y, false)
            } else if (acceptVertical) {
                handler.zoomed(1, scale, pinch.center.x, pinch.center.y, false)
            }
        }
    }

    // Middle button
    MouseArea {
        id: m

        property bool isZoom: false
        property double originalX: 0
        property double originalY: 0
        property double deltaTickingX: 0
        property double deltaTickingY: 0
        property double previousScaleX: 1
        property double previousScaleY: 1

        function calculateScrollingSpeed(x) {
            if (Math.abs(x) < 8)
                return 0;
            return Math.sign(x) * Math.abs(x / 256);
        }

        function setCursorShape(s) {
            if (s === undefined) {
                CursorBinding.enabled = false
            } else {
                CursorBinding.enabled = true
                CursorBinding.cursorShape = s;
            }
        }

        acceptedButtons:
            (((handler.viewModel?.scrollTypes ?? 0) & ScrollBehaviorViewModel.MiddleButton) ? Qt.MiddleButton : Qt.NoButton) |
            (((handler.viewModel?.scrollTypes ?? 0) & ScrollBehaviorViewModel.LeftButton) ? Qt.LeftButton : Qt.NoButton)
        anchors.fill: parent
        cursorShape: undefined
        focus: true
        focusPolicy: Qt.ClickFocus

        property bool altPressed: false

        onCanceled: {
            tickingTimer.stop();
            setCursorShape(undefined)
            deltaTickingX = 0;
            deltaTickingY = 0;
            previousScaleX = 1;
            previousScaleY = 1;
            indicator.close();
        }
        onPositionChanged: function (mouse) {
            let autoScroll = Boolean(handler.viewModel?.autoScroll) !== altPressed;
            if (isZoom) {
                let deltaX = (mouse.x - originalX) / 32;
                let deltaY = (mouse.y - originalY) / 32;
                let scaleX = Math.pow(1 + 0.25 * Math.abs(deltaX), Math.sign(deltaX))
                let scaleY = Math.pow(1 + 0.3 * Math.abs(deltaY), Math.sign(deltaY))
                handler.zoomed(
                    (handler.zoomableOrientation & Qt.Horizontal) ? scaleX / previousScaleX : 1,
                    (handler.zoomableOrientation & Qt.Vertical) ? scaleY / previousScaleY : 1,
                    originalX,
                    originalY,
                    false
                )
                previousScaleX = scaleX;
                previousScaleY = scaleY;
            } else if (autoScroll) {
                deltaTickingX = calculateScrollingSpeed(mouse.x - originalX) * tickingTimer.interval;
                deltaTickingY = calculateScrollingSpeed(mouse.y - originalY) * tickingTimer.interval;
                tickingTimer.start();
            } else {
                handler.moved(originalX - mouse.x, originalY - mouse.y, false);
                originalX = mouse.x;
                originalY = mouse.y;
            }
        }
        onPressed: function (mouse) {
            altPressed = mouse.modifiers & Qt.AltModifier;
            let autoScroll = Boolean(handler.viewModel?.autoScroll) !== altPressed;
            isZoom = handler.viewModel?.isZoom(mouse.modifiers) ?? false;
            if (autoScroll || isZoom) {
                indicator.x = mouse.x - indicator.width / 2;
                indicator.y = mouse.y - indicator.height / 2;
                indicator.open();
                setCursorShape(Qt.ArrowCursor);
            } else {
                setCursorShape(Qt.OpenHandCursor);
            }
            originalX = mouse.x;
            originalY = mouse.y;
        }
        onReleased: canceled()

        Timer {
            id: tickingTimer

            interval: 10
            repeat: true

            onTriggered: {
                handler.moved(m.deltaTickingX, m.deltaTickingY, false);
            }
        }

        T.Popup {
            id: indicator

            closePolicy: T.Popup.NoAutoClose
            height: 24
            padding: 0
            width: 24

            Shape {
                id: centerMagnifier

                anchors.fill: parent
                visible: m.isZoom

                ShapePath {
                    fillColor: "white"
                    strokeColor: "black"
                    strokeWidth: 1
                    startX: indicator.width * 0.5
                    startY: indicator.height * 0.25
                    PathArc {
                        x: indicator.width * 0.7152
                        y: indicator.height * 0.6271
                        radiusX: indicator.width * 0.25
                        radiusY: indicator.height * 0.25
                    }
                    PathLine {
                        x: indicator.width * 1.0052
                        y: indicator.height * 0.9271
                    }
                    PathLine {
                        x: indicator.width * 0.9271
                        y: indicator.height * 1.0052
                    }
                    PathLine {
                        x: indicator.width * 0.6271
                        y: indicator.height * 0.7152
                    }
                    PathArc {
                        x: indicator.width * 0.5
                        y: indicator.height * 0.25
                        radiusX: indicator.width * 0.25
                        radiusY: indicator.height * 0.25
                        useLargeArc: true
                    }
                    PathMove {
                        x: indicator.width * 0.5
                        y: indicator.height * 0.375
                    }
                    PathArc {
                        x: indicator.width * 0.5
                        y: indicator.height * 0.625
                        radiusX: indicator.width * 0.125
                        radiusY: indicator.height * 0.125
                    }
                    PathArc {
                        x: indicator.width * 0.5
                        y: indicator.height * 0.375
                        radiusX: indicator.width * 0.125
                        radiusY: indicator.height * 0.125
                    }
                }
            }

            Shape {
                id: centerPoint

                anchors.fill: parent
                visible: !m.isZoom

                ShapePath {
                    fillColor: "white"
                    strokeColor: "black"
                    strokeWidth: 1

                    PathAngleArc {
                        centerX: indicator.width * 0.5
                        centerY: indicator.height * 0.5
                        radiusX: indicator.width * 0.125
                        radiusY: indicator.height * 0.125
                        startAngle: 0
                        sweepAngle: 360
                    }
                }
            }
            Shape {
                id: topTriangle

                anchors.fill: parent
                visible: (handler.movableOrientation & Qt.Vertical) && (m.deltaTickingY < 0 || (m.deltaTickingX === 0 || !(handler.movableOrientation & Qt.Horizontal)) && m.deltaTickingY === 0)

                ShapePath {
                    fillColor: "white"
                    strokeColor: "black"
                    strokeWidth: 1

                    PathMove { x: indicator.width * 0.5; y: 0 }
                    PathLine { x: indicator.width * 0.375; y: indicator.height * 0.25 }
                    PathLine { x: indicator.width * 0.625; y: indicator.height * 0.25 }
                    PathLine { x: indicator.width * 0.5; y: 0 }
                }
            }
            Shape {
                id: rightTriangle

                anchors.fill: parent
                visible: (handler.movableOrientation & Qt.Horizontal) && (m.deltaTickingX > 0 || m.deltaTickingX === 0 && (m.deltaTickingY === 0 || !(handler.movableOrientation & Qt.Vertical)))

                ShapePath {
                    fillColor: "white"
                    strokeColor: "black"
                    strokeWidth: 1

                    PathMove { x: indicator.width; y: indicator.height * 0.5 }
                    PathLine { x: indicator.width * 0.75; y: indicator.height * 0.375 }
                    PathLine { x: indicator.width * 0.75; y: indicator.height * 0.625 }
                    PathLine { x: indicator.width; y: indicator.height * 0.5 }
                }
            }
            Shape {
                id: bottomTriangle

                anchors.fill: parent
                visible: (handler.movableOrientation & Qt.Vertical) && (m.deltaTickingY > 0 || (m.deltaTickingX === 0 || !(handler.movableOrientation & Qt.Horizontal)) && m.deltaTickingY === 0)

                ShapePath {
                    fillColor: "white"
                    strokeColor: "black"
                    strokeWidth: 1

                    PathMove { x: indicator.width * 0.5; y: indicator.height }
                    PathLine { x: indicator.width * 0.375; y: indicator.height * 0.75 }
                    PathLine { x: indicator.width * 0.625; y: indicator.height * 0.75 }
                    PathLine { x: indicator.width / 2; y: indicator.height }
                }
            }
            Shape {
                id: leftTriangle

                anchors.fill: parent
                visible: (handler.movableOrientation & Qt.Horizontal) && (m.deltaTickingX < 0 || m.deltaTickingX === 0 && (m.deltaTickingY === 0 || !(handler.movableOrientation & Qt.Vertical)))

                ShapePath {
                    fillColor: "white"
                    strokeColor: "black"
                    strokeWidth: 1

                    PathMove { x: 0; y: indicator.height * 0.5 }
                    PathLine { x: indicator.width * 0.25; y: indicator.height * 0.375 }
                    PathLine { x: indicator.width * 0.25; y: indicator.height * 0.625 }
                    PathLine { x: 0; y: indicator.height * 0.5 }
                }
            }
            MouseArea {
                id: popupMouseArea
                acceptedButtons: Qt.NoButton
                anchors.fill: parent
                cursorShape: m.cursorShape
            }
        }
    }

    StyledScrollBar {
        id: horizontalScrollBar

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 6
        orientation: Qt.Horizontal
        position: handler.horizontalScrollX / handler.horizontalScrollWidth
        size: handler.width / handler.horizontalScrollWidth
        visible: handler.horizontalScrollBarEnabled && (handler.movableOrientation & Qt.Horizontal)
        allowDragAdjustment: handler.zoomableOrientation & Qt.Horizontal

        onEndDragged: pos => {
            let ratioX = size / (pos - position)
            handler.zoomed(ratioX, 1, 0, 0, false)
        }
        onPositionChanged: () => {
            if (!pressed)
                return
            let deltaX = position * handler.horizontalScrollWidth - handler.horizontalScrollX
            handler.moved(deltaX, 0, false)
        }
        onStartDragged: pos => {
            let ratioX = size / (position - pos + size)
            handler.zoomed(ratioX, 1, handler.width, 0, false)
        }
    }

    StyledScrollBar {
        id: verticalScrollBar

        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        orientation: Qt.Vertical
        position: handler.verticalScrollY / handler.verticalScrollHeight
        size: handler.height / handler.verticalScrollHeight
        visible: handler.verticalScrollBarEnabled && (handler.movableOrientation & Qt.Vertical)
        allowDragAdjustment: handler.zoomableOrientation & Qt.Vertical

        onEndDragged: pos => {
            let ratioY = size / (pos - position)
            handler.zoomed(1, ratioY, 0, handler.height, false)
        }
        onPositionChanged: () => {
            if (!pressed)
                return
            let deltaY = position * handler.verticalScrollHeight - handler.verticalScrollY
            handler.moved(0, deltaY, false)
        }
        onStartDragged: pos => {
            let ratioY = size / (position - pos + size)
            handler.zoomed(1, ratioY, 0, handler.height, false)
        }
    }
}
