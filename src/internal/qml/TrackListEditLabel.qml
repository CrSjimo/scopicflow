import QtQml
import QtQuick
import QtQuick.Controls.Basic

Item {
    id: editLabel
    width: labelText.width + 8
    required property QtObject trackStyleItem
    required property QtObject popupEditStyleItem
    property string text: ""
    property string editText: text
    property QtObject validator: null
    readonly property bool editing: popup.opened

    signal editingFinished(text: string)

    Text {
        id: labelText
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        text: editLabel.text
        color: editLabel.trackStyleItem.foreground
        visible: !labelEdit.visible
    }
    Popup {
        id: popup
        padding: 0
        background: Item {}
        height: parent.height
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
            validator: editLabel.validator
            background: Rectangle {
                color: editLabel.popupEditStyleItem.background
                radius: 2
                border.width: 1
                border.color: editLabel.popupEditStyleItem.border
            }
            color: editLabel.popupEditStyleItem.foreground
            text: editLabel.editText
            leftPadding: 4
            topPadding: 0
            bottomPadding: 0
            rightPadding: 16
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