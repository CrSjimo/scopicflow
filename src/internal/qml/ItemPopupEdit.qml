import QtQml
import QtQuick

import SVSCraft.UIComponents

Popup {
    id: popup

    required property QtObject containerModel
    required property QtObject model
    property double radius: 0
    property bool removeIfEmpty: false
    required property string targetProperty

    height: parent.height
    padding: 0

    background: Item {
    }

    onClosed: {
        if (!noteEdit.escaped)
            model[targetProperty] = noteEdit.text;
        if (removeIfEmpty && !model[targetProperty].length)
            containerModel.handle.removeItem(model);
    }
    onOpened: {
        noteEdit.text = model[targetProperty];
        noteEdit.escaped = false;
        noteEdit.selectAll();
        noteEdit.forceActiveFocus();
    }

    TextField {
        id: noteEdit

        property bool escaped: false

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        bottomPadding: 0
        leftPadding: 4
        rightPadding: 4
        text: popup.model[popup.targetProperty]
        topPadding: 0
        width: Math.max(popup.width, implicitWidth)

        background: Rectangle {
            border.color: Theme.accentColor
            border.width: 1
            color: Theme.textFieldColor
            radius: 2
        }

        Keys.onBacktabPressed: {
            popup.containerModel.handle.currentItem = popup.containerModel.handle.previousItem(popup.model);
        }
        Keys.onEscapePressed: {
            escaped = true;
            popup.close();
        }
        Keys.onPressed: event => {
            if (event.key === Qt.Key_Home && event.modifiers === Qt.ControlModifier) {
                popup.containerModel.handle.currentItem = popup.containerModel.handle.firstItem();
            } else if (event.key === Qt.Key_End && event.modifiers === Qt.ControlModifier) {
                popup.containerModel.handle.currentItem = popup.containerModel.handle.lastItem();
            } else {
                event.accepted = false;
                return;
            }
            event.accepted = true;
        }
        Keys.onReturnPressed: {
            popup.close();
        }
        Keys.onTabPressed: {
            popup.containerModel.handle.currentItem = popup.containerModel.handle.nextItem(popup.model);
        }
    }
}
