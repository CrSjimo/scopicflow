import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: label

    property bool current: {
        current = labelSequenceViewModel?.currentItem === labelViewModel;
    }
    property bool editing: false
    required property QtObject labelSequenceViewModel

    readonly property QtObject labelViewModel: SequenceSlicerLoader.viewModel

    clip: true
    implicitWidth: labelText.width + 8
    opacity: editing ? 0 : 1
    visible: SequenceSlicerLoader.inRange && !editing

    z: {
        z = current ? 2 : labelViewModel?.selected ? 1 : 0
    }

    Rectangle {
        anchors.fill: parent
        border.color: label.current && labelSequence.activeFocus ? Theme.controlCheckedColorChange.apply(Theme.accentColor) : Theme.borderColor
        border.width: 1
        color: label.labelViewModel?.selected ? Theme.controlCheckedColorChange.apply(Theme.buttonColor) : Theme.buttonColor
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
    }

    Binding {
        label.current: label.labelSequenceViewModel?.currentItem === label.labelViewModel
        label.z: label.current ? 2 : label.labelViewModel?.selected ? 1 : 0
        when: label.SequenceSlicerLoader.inRange
    }
    Text {
        id: labelText

        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        color: Theme.foregroundPrimaryColor
        text: label.labelViewModel?.content ?? ""

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
}
