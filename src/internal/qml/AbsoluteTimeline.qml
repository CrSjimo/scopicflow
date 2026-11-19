import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: timeline

    required property TimeLayoutViewModel timeLayoutViewModel
    required property TimeViewModel timeViewModel

    Accessible.name: qsTr("Absolute Timeline")
    Accessible.role: Accessible.Pane
    clip: true
    implicitHeight: 20
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true

    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundColor(timeline.ThemedItem.backgroundLevel)
    }

    AbsoluteTimelineScale {
        anchors.fill: parent
        color: Theme.foregroundColor(timeline.ThemedItem.foregroundLevel)
        font: Theme.font
        timeLayoutViewModel: timeline.timeLayoutViewModel
        timeViewModel: timeline.timeViewModel
    }
}
