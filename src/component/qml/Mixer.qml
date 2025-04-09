import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject trackListViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null

    padding: 0
    focusPolicy: Qt.StrongFocus
    implicitWidth: mixer.implicitWidth

    signal trackDoubleClicked(index: int)
    signal contextMenuRequestedForTrack(index: int)

    ScopicFlowInternal.Mixer {
        id: mixer
        anchors.fill: parent
        trackListViewModel: control.trackListViewModel
        trackListLayoutViewModel: control.trackListLayoutViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel

        onTrackDoubleClicked: index => control.trackDoubleClicked(index)
        onContextMenuRequestedForTrack: index => control.contextMenuRequestedForTrack(index)

    }
}