import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Rectangle {
    id: label

    property bool current: {
        current = labelSequenceViewModel.handle.currentItem === model;
    }
    property bool editing: popup.opened
    required property QtObject labelSequenceViewModel

    readonly property QtObject labelViewModel: SequenceSlicerLoader.viewModel

    border.color: Theme.borderColor
    border.width: 1
    clip: true
    color: label.labelViewModel.selected ? Theme.controlCheckedColorChange.apply(Theme.buttonColor) : Theme.buttonColor
    implicitWidth: labelText.width + 8
    opacity: editing ? 0 : 1
    radius: 2
    visible: SequenceSlicerLoader.inRange

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

    Binding {
        label.current: label.labelSequenceViewModel && label.labelSequenceViewModel.handle.currentItem === label.labelViewModel
        label.editingRequired: (label.labelSequenceBehaviorViewModel?.editing ?? false) && label.current
        when: label.SequenceSlicerLoader.inRange
    }
    Text {
        id: labelText

        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        color: Theme.foregroundPrimaryColor
        text: label.labelViewModel.content

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
    ItemPopupEdit {
        id: popup

        containerModel: label.labelSequenceViewModel
        model: label.labelViewModel
        radius: label.radius
        removeIfEmpty: true
        targetProperty: "content"
    }
}
