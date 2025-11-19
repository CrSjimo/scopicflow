import QtQml
import QtQuick
import QtQuick.Templates as T

import SVSCraft.UIComponents

T.Popup {
    id: popup

    required property QtObject containerModel
    required property QtObject model
    property double radius: 0
    property bool removeIfEmpty: false
    required property string targetProperty

    height: parent.height
    width: parent.width
    padding: 0

    onClosed: {
        if (!textField.escaped)
            model[targetProperty] = textField.text;
        if (removeIfEmpty && !model[targetProperty].length)
            containerModel.removeItem(model);
    }
    onOpened: {
        textField.text = model[targetProperty];
        textField.escaped = false;
        textField.selectAll();
        textField.forceActiveFocus();
    }

    T.TextField {
        id: textField

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
            popup.containerModel.currentItem = popup.containerModel.iSelectable.previousItem(popup.model);
        }
        Keys.onEscapePressed: {
            escaped = true;
            popup.close();
        }
        Keys.onPressed: event => {
            if (event.key === Qt.Key_Home && (event.modifiers & Qt.ControlModifier)) {
                popup.containerModel.currentItem = popup.containerModel.iSelectable.firstItem();
            } else if (event.key === Qt.Key_End && (event.modifiers & Qt.ControlModifier)) {
                popup.containerModel.currentItem = popup.containerModel.iSelectable.lastItem();
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
            popup.containerModel.currentItem = popup.containerModel.iSelectable.nextItem(popup.model);
        }
    }
}
