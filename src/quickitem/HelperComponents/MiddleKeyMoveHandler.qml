import QtQuick

Item {

    signal moved(x: double, y: double)

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.MiddleButton
        cursorShape: undefined

        property double originalX: 0
        property double originalY: 0

        onPressed: function (mouse) {
            originalX = mouse.x
            originalY = mouse.y
        }
        onPositionChanged: function (mouse) {
            cursorShape = Qt.ClosedHandCursor
            parent.moved(originalX - mouse.x, originalY - mouse.y)
            originalX = mouse.x
            originalY = mouse.y
        }
        onReleased: function (mouse) {
            cursorShape = undefined
        }

    }
}

