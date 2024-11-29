import QtQuick

Item {
    id: handler

    property var viewModel: null

    signal moved(x: double, y: double)

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton
        cursorShape: undefined
        focusPolicy: Qt.StrongFocus

        property double originalX: 0
        property double originalY: 0

        property double deltaTickingX: 0
        property double deltaTickingY: 0
        Timer {
            id: tickingTimer
            interval: 10
            repeat: true

            onTriggered: {
                handler.moved(parent.deltaTickingX, parent.deltaTickingY)
            }
        }
        function calculateScrollingSpeed(x) {
            return Math.sign(x) * Math.min(1, Math.abs(x / 256))
        }

        onPressed: function (mouse) {
            if (viewModel?.affectVelocity)
                cursorShape = Qt.OpenHandCursor
            else
                cursorShape = Qt.ClosedHandCursor
            originalX = mouse.x
            originalY = mouse.y
        }
        onPositionChanged: function (mouse) {
            if (viewModel?.affectVelocity) {
                deltaTickingX = calculateScrollingSpeed(mouse.x - originalX) * tickingTimer.interval
                deltaTickingY = calculateScrollingSpeed(mouse.y - originalY) * tickingTimer.interval
                tickingTimer.start()
            } else {
                handler.moved(originalX - mouse.x, originalY - mouse.y)
                originalX = mouse.x
                originalY = mouse.y
            }
        }
        onReleased: function (mouse) {
            tickingTimer.stop()
            cursorShape = undefined
        }

    }
}

