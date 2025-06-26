import QtQml
import QtQuick
import QtQuick.Templates as T

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Style

T.Control {
    id: editLabel

    property bool center: false
    property string editText: text
    readonly property bool editing: popup.opened
    property var horizontalAlignment: undefined
    property string text: ""
    property QtObject validator: null

    signal canceled
    signal clicked
    signal doubleClicked
    signal editingFinished(text: string)
    signal pressed
    signal released

    function open() {
        popup.open();
    }

    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    width: labelText.width + 8

    Text {
        id: labelText

        anchors.horizontalCenter: editLabel.center ? parent.horizontalCenter : undefined
        anchors.left: editLabel.center ? undefined : parent.left
        anchors.leftMargin: editLabel.center ? undefined : 4
        anchors.verticalCenter: parent.verticalCenter
        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
        horizontalAlignment: editLabel.center ? Text.AlignHCenter : undefined
        text: editLabel.text
        visible: !labelEdit.visible
    }
    Popup {
        id: popup

        height: parent.height
        padding: 0
        width: parent.width

        background: Item {
        }

        onClosed: {
            if (!labelEdit.escaped)
                editLabel.editingFinished(labelEdit.text);
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

        onCanceled: editLabel.canceled()
        onClicked: editLable.clicked()
        onDoubleClicked: () => {
            editLabel.open();
            editLabel.doubleClicked();
        }
        onPressed: editLabel.pressed()
        onReleased: editLabel.released()
    }
}
