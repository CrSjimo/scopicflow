import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow

Rectangle {
    id: labelRect

    required property QtObject animationViewModel
    property bool current: {
        current = labelSequenceViewModel.handle.currentItem === model;
    }
    property bool editing: popup.opened
    property bool editingRequired: {
        editingRequired = (labelRect.labelSequenceBehaviorViewModel?.editing ?? false) && labelRect.current;
    }
    required property QtObject labelSequenceBehaviorViewModel
    required property QtObject labelSequenceViewModel
    required property QtObject model

    border.color: Theme.borderColor
    border.width: 1
    clip: true
    color: labelRect.model.selected ? SFPalette.noteSelectedColorChange.apply(labelSequenceBehaviorViewModel?.color ?? Theme.accentColor) : (labelSequenceBehaviorViewModel?.color ?? Theme.accentColor)
    implicitWidth: labelText.width + 8
    opacity: editing ? 0 : 1
    radius: 2

    Behavior on border.color {
        ColorAnimation {
            duration: Theme.colorAnimationDuration
            easing.type: Easing.OutCubic
        }
    }
    Behavior on color {
        ColorAnimation {
            duration: Theme.colorAnimationDuration
            easing.type: Easing.OutCubic
        }
    }

    onEditingChanged: {
        if (current && labelSequenceBehaviorViewModel) {
            labelSequenceBehaviorViewModel.editing = editing;
        }
    }
    onEditingRequiredChanged: {
        if (editingRequired)
            popup.open();
        else
            popup.close();
    }

    Binding {
        labelRect.current: labelRect.labelSequenceViewModel && labelRect.labelSequenceViewModel.handle.currentItem === labelRect.model
        labelRect.editingRequired: (labelRect.labelSequenceBehaviorViewModel?.editing ?? false) && labelRect.current
        when: labelRect.visible
    }
    Text {
        id: labelText

        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        color: SFPalette.suitableForegroundColor(labelSequenceBehaviorViewModel?.color ?? Theme.accentColor)
        text: labelRect.model.content

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
    ItemPopupEdit {
        id: popup

        containerModel: labelRect.labelSequenceViewModel
        model: labelRect.model
        radius: labelRect.radius
        removeIfEmpty: true
        targetProperty: "content"
    }
}
