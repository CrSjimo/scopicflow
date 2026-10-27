import QtQml
import QtQuick
import QtQuick.Layouts

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

RowLayout {
    id: control

    readonly property ToolButton muteButton: muteButton_
    readonly property ToolButton soloButton: soloButton_
    required property QtObject trackViewModel

    spacing: 4

    ToolButton {
        id: muteButton_

        Theme.accentColor: SFPalette.muteColor
        ThemedItem.controlType: SVS.CT_Accent
        checkable: true
        checked: control.trackViewModel.mute ?? false
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
        checked: control.trackViewModel.solo ?? false
        flat: false
        text: 'Solo'
        Component.onCompleted: contentItem.text = 'S'
        DescriptiveText.toolTip: text
    }
}
