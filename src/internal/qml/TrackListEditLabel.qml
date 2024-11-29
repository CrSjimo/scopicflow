import QtQml
import QtQuick
import QtQuick.Controls.Basic

Item {
    id: editLabel
    width: labelText.width + 8
    required property QtObject palette
    property string text: ""
    property string editText: text
    property QtObject validator: null

    signal editingFinished(text: string)

    Text {
        id: labelText
        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        text: editLabel.text
        color: editLabel.palette.foregroundColor
        visible: !labelEdit.visible
    }
    TextField {
        id: labelEdit
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        validator: editLabel.validator
        background: Rectangle {
            color: editLabel.palette.textEditingBackgroundColor
            radius: 2
            border.width: 1
            border.color: editLabel.palette.textEditingBorderColor
        }
        color: editLabel.palette.textEditingForegroundColor
        text: editLabel.editText
        leftPadding: 4
        topPadding: 0
        bottomPadding: 0
        rightPadding: 16
        visible: false
        property bool escaped: false
        Keys.onEscapePressed: {
            escaped = true
            visible = false
            focus = false
        }
        Keys.onReturnPressed: {
            visible = false
            focus = false
        }
        onVisibleChanged: {
            if (!visible) {
                if (!escaped)
                    editLabel.editingFinished(text)
                else
                    escaped = false
            }
        }
        onActiveFocusChanged: {
            if (!activeFocus)
                visible = false
        }
    }

    MouseArea {
        anchors.fill: parent
        focusPolicy: Qt.StrongFocus
        onDoubleClicked: {
            labelEdit.visible = true
            labelEdit.focus = true
        }
    }

}