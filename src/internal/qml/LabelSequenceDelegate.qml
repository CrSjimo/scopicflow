import QtQml
import QtQuick
import QtQuick.Controls.Basic
import dev.sjimo.ScopicFlow.Internal

Rectangle {
    id: labelRect
    required property QtObject model
    required property QtObject animationViewModel
    required property QtObject labelSequenceViewModel
    required property QtObject labelSequenceBehaviorViewModel

    required property QtObject stylesheet

    Binding {
        when: labelRect.visible
        labelRect.current: labelRect.labelSequenceViewModel.handle.currentItem === labelRect.model
        labelRect.labelStyleItem: labelRect.stylesheet.labelSequenceDelegate.createObject(labelRect, {labelViewModel: labelRect.model, current: labelRect.current})
        labelRect.popupEditStyleItem: labelRect.stylesheet.popupEdit.createObject(labelRect)
        labelRect.editingRequired: (labelRect.labelSequenceBehaviorViewModel?.editing ?? false) && labelRect.current
    }

    property bool current: {current = labelSequenceViewModel.handle.currentItem === model}
    property QtObject labelStyleItem: {labelStyleItem = stylesheet.labelSequenceDelegate.createObject(labelRect, {labelViewModel: model, current})}
    property QtObject popupEditStyleItem: {popupEditStyleItem = stylesheet.popupEdit.createObject(labelRect)}
    property bool editing: popup.opened

    implicitWidth: labelText.width + 8
    opacity: editing ? 0 : 1

    property bool editingRequired: {editingRequired = (labelRect.labelSequenceBehaviorViewModel?.editing ?? false) && labelRect.current}

    border.width: 1
    radius: 2
    border.color: labelStyleItem.border
    Behavior on border.color {
        ColorAnimation {
            duration: (labelRect.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }
    clip: true
    color: labelStyleItem.background
    Behavior on color {
        ColorAnimation {
            duration: (labelRect.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }

    onEditingChanged: {
        if (labelSequenceViewModel.handle.currentItem === model && labelSequenceBehaviorViewModel) {
            labelSequenceBehaviorViewModel.editing = editing
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
        color: labelRect.labelStyleItem.foreground
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
                color: labelRect.popupEditStyleItem.background
                radius: 2
                border.width: 1
                border.color: labelRect.popupEditStyleItem.border
            }
            color: labelRect.popupEditStyleItem.foreground
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