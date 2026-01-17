import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: label

    property bool current: false
    property bool editing: false
    property bool editScopeFocused: false
    property bool selected: false
    property string content: ""
    required property PointSequenceViewModel labelSequenceViewModel
    required property SelectionController selectionController

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
        border.color: label.current ? label.editScopeFocused ? Theme.accentColor : Theme.foregroundSecondaryColor : Theme.borderColor
        border.width: 1
        color: label.selected ? SFPalette.itemSelectedColorChange.apply(Theme.backgroundTertiaryColor) : Theme.backgroundTertiaryColor
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
        label.z: label.current ? 2 : label.labelViewModel?.selected ? 1 : 0
        label.selected: label.labelViewModel?.selected ?? false
        label.content: label.labelViewModel?.content ?? ""
        when: label.SequenceSlicerLoader.inRange
    }
    Binding {
        label.editScopeFocused: label.selectionController?.editScopeFocused ?? false
        when: label.SequenceSlicerLoader.inRange && label.current
    }
    Text {
        id: labelText

        anchors.left: parent.left
        anchors.leftMargin: 4
        anchors.verticalCenter: parent.verticalCenter
        color: Theme.foregroundPrimaryColor
        text: label.content
    }
}
