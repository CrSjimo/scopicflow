import QtQml
import QtQuick
import QtQuick.Templates as T

import SVSCraft.UIComponents

T.Popup {
    id: popup

    required property QtObject containerModel
    required property string targetProperty

    property QtObject model: null
    property double radius: 0

    padding: 0

    onClosed: {
        if (!textField.escaped) {
            model[targetProperty] = textField.text;
            accepted()
        } else {
            rejected()
        }
    }
    onOpened: {
        textField.text = model[targetProperty];
        textField.escaped = false;
        textField.selectAll();
        textField.forceActiveFocus();
    }

    signal editPreviousRequested()
    signal editNextRequested()
    signal editHomeRequested()
    signal editEndRequested()
    signal accepted()
    signal rejected()

    T.TextField {
        id: textField

        property bool escaped: false

        leftPadding: 4
        rightPadding: 4
        text: popup.model?.[popup.targetProperty] ?? ""
        topPadding: 0.5 * (height - contentHeight)
        width: Math.max(popup.width, contentWidth + 8)
        height: popup.height
        color: Theme.foregroundPrimaryColor

        background: Rectangle {
            border.color: Theme.accentColor
            border.width: 1
            color: Theme.textFieldColor
            radius: 2
        }

        Keys.onBacktabPressed: editPreviousRequested()
        Keys.onEscapePressed: {
            escaped = true;
            popup.close();
        }
        Keys.onPressed: event => {
            if (event.key === Qt.Key_Home && (event.modifiers & Qt.ControlModifier)) {
                popup.editHomeRequested()
            } else if (event.key === Qt.Key_End && (event.modifiers & Qt.ControlModifier)) {
                popup.editEndRequested()
            } else {
                event.accepted = false;
                return;
            }
            event.accepted = true;
        }
        Keys.onReturnPressed: {
            popup.close();
        }
        Keys.onTabPressed: editNextRequested()
    }
}
