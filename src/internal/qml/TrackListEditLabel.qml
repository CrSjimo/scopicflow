import QtQml
import QtQuick
import QtQuick.Templates as T

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Style

T.Control {
    id: editLabel
    width: labelText.width + 8
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    property string text: ""
    property bool center: false
    property var horizontalAlignment: undefined
    property string editText: text
    property QtObject validator: null
    readonly property bool editing: popup.opened

    signal editingFinished(text: string)
    signal pressed()
    signal released()
    signal canceled()
    signal clicked()
    signal doubleClicked()

    function open() {
        popup.open()
    }

    Text {
        id: labelText
        anchors.left: editLabel.center ? undefined : parent.left
        anchors.leftMargin: editLabel.center ? undefined : 4
        anchors.horizontalCenter: editLabel.center ? parent.horizontalCenter : undefined
        anchors.verticalCenter: parent.verticalCenter
        text: editLabel.text
        horizontalAlignment: editLabel.center ? Text.AlignHCenter : undefined
        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
        visible: !labelEdit.visible
    }
    Popup {
        id: popup
        padding: 0
        background: Item {}
        height: parent.height
        width: parent.width
        onOpened: {
            labelEdit.text = editLabel.editText
            labelEdit.forceActiveFocus()
        }
        onClosed: {
            if (!labelEdit.escaped)
                editLabel.editingFinished(labelEdit.text)
            labelEdit.escaped = false
        }
        TextField {
            id: labelEdit
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: editLabel.center ? parent.horizontalCenter : undefined
            validator: editLabel.validator
            background: Rectangle {
                color: Theme.textFieldColor
                radius: 2
                border.width: 1
                border.color: Theme.accentColor
            }
            text: editLabel.editText
            horizontalAlignment: editLabel.center ? Text.AlignHCenter : undefined
            leftPadding: editLabel.center ? 8 : 4
            topPadding: 0
            bottomPadding: 0
            rightPadding: editLabel.center ? 8 : 16
            property bool escaped: false
            Keys.onEscapePressed: {
                escaped = true
                popup.close()
            }
            Keys.onReturnPressed: {
                popup.close()
            }
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "transparent"
        border.color: Theme.navigationColor
        border.width: 2
        visible: editLabel.visualFocus
        radius: 2
    }

    MouseArea {
        anchors.fill: parent
        onPressed: editLabel.pressed()
        onReleased: editLabel.released()
        onCanceled: editLabel.canceled()
        onClicked: editLable.clicked()
        onDoubleClicked: () => {
            editLabel.open()
            editLabel.doubleClicked()
        }
    }

}