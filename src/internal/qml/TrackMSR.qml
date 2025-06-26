import QtQml
import QtQuick
import QtQuick.Layouts

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

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
        text: 'M'
        visible: typeof (msr.trackViewModel.mute) === "boolean"

        onCheckedChanged: msr.trackViewModel.mute = checked
    }
    ToolButton {
        id: soloButton_

        Theme.accentColor: SFPalette.soloColor
        ThemedItem.controlType: SVS.CT_Accent
        checkable: true
        checked: msr.trackViewModel.solo ?? false
        flat: false
        text: 'S'
        visible: typeof (msr.trackViewModel.solo) === "boolean"

        onCheckedChanged: msr.trackViewModel.solo = checked
    }
    ToolButton {
        id: recordButton_

        Theme.accentColor: SFPalette.recordColor
        ThemedItem.controlType: SVS.CT_Accent
        checkable: true
        checked: msr.trackViewModel.record ?? false
        flat: false
        text: 'R'
        visible: typeof (msr.trackViewModel.record) === "boolean"

        onCheckedChanged: msr.trackViewModel.record = checked
    }
}
