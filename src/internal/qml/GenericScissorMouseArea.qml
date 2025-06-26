import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

MouseArea {
    id: scissorMouseArea

    readonly property int cutPosition: dragged && pressed ? pressedPosition : -1
    property bool dragged: false
    required property QtObject model
    required property Item paneItem
    property int pressedPosition: 0
    property double pressedY

    anchors.fill: parent
    cursorShape: Qt.UpArrowCursor // TODO scissor cursor

    onPositionChanged: mouse => {
        if (Math.abs(mouse.y - pressedY) > 8 && pressedPosition !== model.position + model.length) {
            dragged = true;
        } else {
            dragged = false;
        }
    }
    onPressed: mouse => {
        pressedPosition = Math.max(model.position + timeLocator.timeLayoutViewModel.positionAlignment, Math.min(timeLocator.alignTick(timeLocator.mapToTick(mapToItem(paneItem, mouse.x, 0).x)), model.position + model.length - timeLocator.timeLayoutViewModel.positionAlignment));
        pressedY = mouse.y;
        dragged = false;
    }
}
