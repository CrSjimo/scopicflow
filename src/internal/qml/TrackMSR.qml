import QtQml
import QtQuick
import QtQuick.Layouts

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

RowLayout {
    id: msr

    readonly property ToolButton muteButton: muteButton_
    readonly property ToolButton recordButton: recordButton_
    readonly property ToolButton soloButton: soloButton_
    required property QtObject trackViewModel

    spacing: 4

    ToolButton {
        id: muteButton_

        Theme.accentColor: SFPalette.muteColor
        ThemedItem.controlType: SVS.CT_Accent
        checkable: true
        checked: msr.trackViewModel.mute ?? false
        flat: false
        text: 'Mute'
        Component.onCompleted: contentItem.text = 'M'
        DescriptiveText.toolTip: text
    }
    ToolButton {
        id: soloButton_

        Theme.accentColor: SFPalette.soloColor
        ThemedItem.controlType: SVS.CT_Accent
        checkable: true
        checked: msr.trackViewModel.solo ?? false
        flat: false
        text: 'Solo'
        Component.onCompleted: contentItem.text = 'S'
        DescriptiveText.toolTip: text
    }
    ToolButton {
        id: recordButton_

        Theme.accentColor: SFPalette.recordColor
        ThemedItem.controlType: SVS.CT_Accent
        checkable: true
        checked: msr.trackViewModel.record ?? false
        flat: false
        text: 'Record'
        Component.onCompleted: contentItem.text = 'R'
        DescriptiveText.toolTip: text
    }
}
