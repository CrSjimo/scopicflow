import QtQml
import QtQuick
import QtQuick.Controls.Basic

Popup {
    id: popup
    required property QtObject model
    required property QtObject containerModel
    required property string targetProperty
    required property QtObject styleItem
    property bool removeIfEmpty: false

    padding: 0
    background: Item {}
    height: parent.height
    onOpened: {
        noteEdit.text = model[targetProperty]
        noteEdit.escaped = false
        noteEdit.selectAll()
        noteEdit.forceActiveFocus()
    }
    onClosed: {
        if (!noteEdit.escaped)
            model[targetProperty] = noteEdit.text
        if (removeIfEmpty && !model[targetProperty].length)
            containerModel.handle.removeItem(model)
    }
    TextField {
        id: noteEdit
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: Math.max(popup.width, implicitWidth)
        background: Rectangle {
            color: popup.styleItem.background
            radius: 4
            border.width: 1
            border.color: popup.styleItem.border
        }
        color: popup.styleItem.foreground
        text: popup.model[popup.targetProperty]
        leftPadding: 4
        topPadding: 0
        bottomPadding: 0
        rightPadding: 4
        property bool escaped: false
        Keys.onEscapePressed: {
            escaped = true
            popup.close()
        }
        Keys.onReturnPressed: {
            popup.close()
        }
        Keys.onTabPressed: {
            popup.containerModel.handle.currentItem = popup.containerModel.handle.nextItem(popup.model)
        }
        Keys.onBacktabPressed: {
            popup.containerModel.handle.currentItem = popup.containerModel.handle.previousItem(popup.model)
        }
        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_Home && event.modifiers === Qt.ControlModifier) {
                popup.containerModel.handle.currentItem = popup.containerModel.handle.firstItem()
            } else if (event.key === Qt.Key_End && event.modifiers === Qt.ControlModifier) {
                popup.containerModel.handle.currentItem = popup.containerModel.handle.lastItem()
            } else {
                event.accepted = false
                return
            }
            event.accepted = true
        }
    }
}