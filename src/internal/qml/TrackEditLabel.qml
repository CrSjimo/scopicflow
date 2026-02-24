import QtQml
import QtQuick
import QtQuick.Templates as T

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

T.Control {
    id: editLabel

    property bool center: false
    property string editText: text
    readonly property bool editing: popup.opened
    property var horizontalAlignment: undefined
    property string text: ""
    property QtObject validator: null
    property bool readOnly: false

    signal editingStarted()
    signal editingCommitted(text: string)
    signal editingAborted()

    function open() {
        if (editLabel.readOnly)
            return;
        popup.open();
        editingStarted()
    }

    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    implicitWidth: labelText.contentWidth + (editLabel.center ? 16 : 20)

    Text {
        id: labelText

        anchors.horizontalCenter: editLabel.center ? parent.horizontalCenter : undefined
        anchors.left: editLabel.center ? undefined : parent.left
        anchors.leftMargin: editLabel.center ? undefined : 4
        anchors.verticalCenter: parent.verticalCenter
        color: Theme.foregroundPrimaryColor
        horizontalAlignment: editLabel.center ? Text.AlignHCenter : undefined
        text: editLabel.text
        visible: !labelEdit.visible
        font: Theme.font
    }
    Popup {
        id: popup

        height: parent.height
        padding: 0
        width: parent.width

        background: Item {
        }

        onClosed: {
            if (!labelEdit.escaped) {
                editLabel.editingCommitted(labelEdit.text);
            } else {
                editLabel.editingAborted();
            }
            labelEdit.escaped = false;
        }
        onOpened: {
            labelEdit.text = editLabel.editText;
            labelEdit.forceActiveFocus();
        }

        TextField {
            id: labelEdit

            property bool escaped: false

            anchors.bottom: parent.bottom
            anchors.horizontalCenter: editLabel.center ? parent.horizontalCenter : undefined
            anchors.top: parent.top
            bottomPadding: 0
            horizontalAlignment: editLabel.center ? Text.AlignHCenter : undefined
            leftPadding: editLabel.center ? 8 : 4
            rightPadding: editLabel.center ? 8 : 16
            text: editLabel.editText
            topPadding: 0
            validator: editLabel.validator

            background: Rectangle {
                border.color: Theme.accentColor
                border.width: 1
                color: Theme.textFieldColor
                radius: 2
            }

            Keys.onEscapePressed: {
                escaped = true;
                popup.close();
            }
            Keys.onReturnPressed: {
                popup.close();
            }
        }
    }
    Rectangle {
        anchors.fill: parent
        border.color: Theme.navigationColor
        border.width: 2
        color: "transparent"
        radius: 2
        visible: editLabel.visualFocus
    }
    MouseArea {
        anchors.fill: parent
        onDoubleClicked: editLabel.open();
    }
    Accessible.onPressAction: editLabel.open();
    Keys.onSpacePressed: editLabel.open();
}
