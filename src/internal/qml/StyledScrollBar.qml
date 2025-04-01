import QtQuick

import SVSCraft.UIComponents

ScrollBar {
    id: scrollBar

    property bool allowDragAdjustment: true

    signal startDragged(pos: double)
    signal endDragged(pos: double)

    MouseArea {
        parent: scrollBar.contentItem
        width: 6
        height: 6
        anchors.left: parent.left
        anchors.top: parent.top
        cursorShape: scrollBar.orientation === Qt.Horizontal ? Qt.SizeHorCursor : Qt.SizeVerCursor
        visible: scrollBar.allowDragAdjustment
        property double originValue: 0
        property double originPressedValue: 0
        onPressed: (mouse) => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y))
            originValue = scrollBar.position
            originPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height
        }
        onPositionChanged: (mouse) => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y))
            let currentPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height
            let currentValue = originValue + currentPressedValue - originPressedValue
            currentValue = Math.max(0, Math.min(currentValue, scrollBar.position + scrollBar.size))
            if (currentValue !== scrollBar.position)
                scrollBar.startDragged(currentValue)
        }
    }

    MouseArea {
        parent: scrollBar.contentItem
        width: 6
        height: 6
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        cursorShape: scrollBar.orientation === Qt.Horizontal ? Qt.SizeHorCursor : Qt.SizeVerCursor
        visible: scrollBar.allowDragAdjustment
        property double originValue: 0
        property double originPressedValue: 0
        onPressed: (mouse) => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y))
            originValue = scrollBar.position + scrollBar.size
            originPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height
        }
        onPositionChanged: (mouse) => {
            let p = mapToItem(scrollBar, Qt.point(mouse.x, mouse.y))
            let currentPressedValue = scrollBar.orientation === Qt.Horizontal ? p.x / scrollBar.width : p.y / scrollBar.height
            let currentValue = originValue + currentPressedValue - originPressedValue
            currentValue = Math.min(1, Math.max(currentValue, scrollBar.position))
            if (currentValue !== scrollBar.position)
                scrollBar.endDragged(currentValue)
        }
    }
}
