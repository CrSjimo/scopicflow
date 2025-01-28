import QtQml
import QtQuick
import QtQuick.Controls.Basic
import dev.sjimo.ScopicFlow.Internal

Rectangle {
    id: labelRect
    required property QtObject model
    required property QtObject palette
    required property QtObject animationViewModel
    required property QtObject labelSequenceViewModel
    required property QtObject labelSequenceLayoutViewModel

    readonly property bool editing: popup.opened

    implicitWidth: labelText.width + 8
    opacity: editing ? 0 : 1

    readonly property bool editingRequired: (labelSequenceLayoutViewModel?.editing ?? false) && labelSequenceViewModel.handle.currentItem === model

    border.width: 1
    radius: 2
    border.color: model.selected ? palette.labelSelectedBorderColor : palette.labelBorderColor
    Behavior on border.color {
        ColorAnimation {
            duration: (labelRect.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }
    clip: true
    color: model.selected ? palette.labelSelectedColor: palette.labelColor
    Behavior on color {
        ColorAnimation {
            duration: (labelRect.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }

    onEditingChanged: {
        if (labelSequenceViewModel.handle.currentItem === model && labelSequenceLayoutViewModel) {
            labelSequenceLayoutViewModel.editing = editing
        }
    }

    onEditingRequiredChanged: {
        if (editingRequired)
            popup.open()
        else
            popup.close()
    }

    Text {
        id: labelText
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 4
        text: labelRect.model.content
        color: labelRect.model.selected ? labelRect.palette.labelSelectedTextColor : labelRect.palette.labelTextColor
        Behavior on color {
            ColorAnimation {
                duration: (labelRect.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }
    }
    Popup {
        id: popup
        padding: 0
        background: Item {}
        height: parent.height
        onOpened: {
            labelEdit.text = labelRect.model.content
            labelEdit.escaped = false
            labelEdit.forceActiveFocus()
        }
        onClosed: {
            if (!labelEdit.escaped)
                labelRect.model.content = labelEdit.text
            if (!labelRect.model.content.length)
                labelRect.labelSequenceViewModel.handle.removeItem(labelRect.model)
        }
        TextField {
            id: labelEdit
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            background: Rectangle {
                color: labelRect.palette.labelEditingColor
                radius: 2
                border.width: 1
                border.color: labelRect.palette.labelEditingBorderColor
            }
            color: labelRect.palette.labelEditingTextColor
            text: labelRect.model.content
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
                labelRect.labelSequenceViewModel.handle.currentItem = labelRect.labelSequenceViewModel.handle.nextItem(labelRect.model)
            }
            Keys.onBacktabPressed: {
                labelRect.labelSequenceViewModel.handle.currentItem = labelRect.labelSequenceViewModel.handle.previousItem(labelRect.model)
            }
            Keys.onPressed: (event) => {
                if (event.key === Qt.Key_Home && event.modifiers === Qt.ControlModifier) {
                    labelRect.labelSequenceViewModel.handle.currentItem = labelRect.labelSequenceViewModel.handle.firstItem()
                } else if (event.key === Qt.Key_End && event.modifiers === Qt.ControlModifier) {
                    labelRect.labelSequenceViewModel.handle.currentItem = labelRect.labelSequenceViewModel.handle.lastItem()
                } else {
                    event.accepted = false
                    return
                }
                event.accepted = true
            }
        }
    }

}