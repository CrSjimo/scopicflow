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
        labelRect.current: labelRect.labelSequenceViewModel && labelRect.labelSequenceViewModel.handle.currentItem === labelRect.model
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
        if (current && labelSequenceBehaviorViewModel) {
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
    ItemPopupEdit {
        id: popup
        model: labelRect.model
        containerModel: labelRect.labelSequenceViewModel
        targetProperty: "content"
        styleItem: labelRect.popupEditStyleItem
        radius: labelRect.radius
        removeIfEmpty: true
    }

}