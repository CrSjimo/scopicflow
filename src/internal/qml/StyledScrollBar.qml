import QtQuick

import SVSCraft.UIComponents

ScrollBar {
    id: scrollBar

    property bool allowDragAdjustment: true

    signal endDragged(pos: double)
    signal startDragged(pos: double)

    MouseArea {
        property double originPressedValue: 0
        property double originValue: 0

        anchors.left: parent.left
        anchors.top: parent.top
        cursorShape: scrollBar.orientation === Qt.Horizontal ? Qt.SizeHorCursor : Qt.SizeVerCursor
        height: 6
        parent: scrollBar.contentItem
        visible: scrollBar.allowDragAdjustment
        width: 6

        onPositionChanged: mouse => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y));
            let currentPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height;
            let currentValue = originValue + currentPressedValue - originPressedValue;
            currentValue = Math.max(0, Math.min(currentValue, scrollBar.position + scrollBar.size));
            if (currentValue !== scrollBar.position)
                scrollBar.startDragged(currentValue);
        }
        onPressed: mouse => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y));
            originValue = scrollBar.position;
            originPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height;
        }
    }
    MouseArea {
        property double originPressedValue: 0
        property double originValue: 0

        anchors.bottom: parent.bottom
        anchors.right: parent.right
        cursorShape: scrollBar.orientation === Qt.Horizontal ? Qt.SizeHorCursor : Qt.SizeVerCursor
        height: 6
        parent: scrollBar.contentItem
        visible: scrollBar.allowDragAdjustment
        width: 6

        onPositionChanged: mouse => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y));
            let currentPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height;
            let currentValue = originValue + currentPressedValue - originPressedValue;
            currentValue = Math.min(1, Math.max(currentValue, scrollBar.position));
            if (currentValue !== scrollBar.position)
                scrollBar.endDragged(currentValue);
        }
        onPressed: mouse => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y));
            originValue = scrollBar.position + scrollBar.size;
            originPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height;
        }
    }
}
