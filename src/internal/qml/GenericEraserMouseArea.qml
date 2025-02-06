import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: eraserMouseArea
    anchors.fill: parent
    cursorShape: Qt.CrossCursor // TODO erasor cursor
    property double pressedX: 0
    property double pressedY: 0
    property bool dragged: false

    readonly property bool willBeErased: dragged && pressed

    onPressed: (mouse) => {
        pressedX = mouse.x
        pressedY = mouse.y
        dragged = false
    }
    onPositionChanged: (mouse) => {
        if (Math.abs(mouse.x - pressedX) > 8 || Math.abs(mouse.y - pressedY) > 8) {
            dragged = true
        } else {
            dragged = false
        }
    }
}