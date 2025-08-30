import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: eraserMouseArea

    property bool dragged: false
    property double pressedX: 0
    property double pressedY: 0
    readonly property bool willBeErased: dragged && pressed

    anchors.fill: parent
    cursorShape: Qt.CrossCursor // TODO erasor cursor

    onPositionChanged: mouse => {
        if (Math.abs(mouse.x - pressedX) > 8 || Math.abs(mouse.y - pressedY) > 8) {
            dragged = true;
        } else {
            dragged = false;
        }
    }
    onPressed: mouse => {
        pressedX = mouse.x;
        pressedY = mouse.y;
        dragged = false;
    }
}
