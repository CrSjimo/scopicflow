import QtQml
import QtQuick
import QtQuick.Layouts

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

RowLayout {
    id: msr
    required property QtObject trackViewModel
    spacing: 4
    ToolButton {
        Theme.accentColor: SFPalette.muteColor
        ThemedItem.controlType: SVS.CT_Accent
        visible: typeof(msr.trackViewModel.mute) === "boolean"
        checkable: true
        flat: false
        text: 'M'
        checked: msr.trackViewModel.mute ?? false
        onCheckedChanged: msr.trackViewModel.mute = checked
    }
    ToolButton {
        Theme.accentColor: SFPalette.soloColor
        ThemedItem.controlType: SVS.CT_Accent
        visible: typeof(msr.trackViewModel.solo) === "boolean"
        checkable: true
        flat: false
        text: 'S'
        checked: msr.trackViewModel.solo ?? false
        onCheckedChanged: msr.trackViewModel.solo = checked
    }
    ToolButton {
        id: recordButton
        Theme.accentColor: SFPalette.recordColor
        ThemedItem.controlType: SVS.CT_Accent
        visible: typeof(msr.trackViewModel.record) === "boolean"
        checkable: true
        flat: false
        text: 'R'
        checked: msr.trackViewModel.record ?? false
        onCheckedChanged: msr.trackViewModel.record = checked
    }
}