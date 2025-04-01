import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Rectangle {
    id: labelRect
    required property QtObject model
    required property QtObject animationViewModel
    required property QtObject labelSequenceViewModel
    required property QtObject labelSequenceBehaviorViewModel

    Binding {
        when: labelRect.visible
        labelRect.current: labelRect.labelSequenceViewModel && labelRect.labelSequenceViewModel.handle.currentItem === labelRect.model
        labelRect.editingRequired: (labelRect.labelSequenceBehaviorViewModel?.editing ?? false) && labelRect.current
    }

    property bool current: {current = labelSequenceViewModel.handle.currentItem === model}
    property bool editing: popup.opened

    implicitWidth: labelText.width + 8
    opacity: editing ? 0 : 1

    property bool editingRequired: {editingRequired = (labelRect.labelSequenceBehaviorViewModel?.editing ?? false) && labelRect.current}

    border.width: 1
    radius: 2
    border.color: Theme.borderColor
    Behavior on border.color {
        ColorAnimation {
            duration: (labelRect.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }
    clip: true
    color: labelRect.model.selected ? SFPalette.noteSelectedColorChange.apply(labelSequenceBehaviorViewModel?.color ?? Theme.accentColor) : (labelSequenceBehaviorViewModel?.color ?? Theme.accentColor)
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
        color: SFPalette.suitableForegroundColor(labelSequenceBehaviorViewModel?.color ?? Theme.accentColor)
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
        radius: labelRect.radius
        removeIfEmpty: true
    }

}