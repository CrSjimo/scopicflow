import QtQuick
import QtQuick.Controls.Basic

ScrollBar {
    id: scrollBar

    property bool allowDragAdjustment: true
    required property color normalColor
    required property color pressedColor
    required property color hoveredColor
    required property QtObject animationViewModel

    signal startDragged(pos: double)
    signal endDragged(pos: double)

    contentItem: Rectangle {
        implicitWidth: 8
        implicitHeight: 8
        radius: 4
        color: scrollBar.pressed ? scrollBar.pressedColor : scrollBar.hovered ? scrollBar.hoveredColor : scrollBar.normalColor

        Behavior on color {
            ColorAnimation {
                duration: 250 * (scrollBar.animationViewModel?.colorAnimation ?? 1)
                easing.type: Easing.OutCubic
            }
        }

        MouseArea {
            width: 8
            height: 8
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
            width: 8
            height: 8
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
}
