import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Style

Item {
    id: editLabel
    width: labelText.width + 8
    property string text: ""
    property bool center: false
    property var horizontalAlignment: undefined
    property string editText: text
    property QtObject validator: null
    readonly property bool editing: popup.opened

    signal editingFinished(text: string)

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

    MouseArea {
        anchors.fill: parent
        focusPolicy: Qt.StrongFocus
        onDoubleClicked: {
            popup.open()
        }
    }

}