import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null

    padding: 0
    focusPolicy: Qt.StrongFocus

    function mapToTick(x) {
        return timeline.mapToTick(x)
    }
    function mapToX(tick) {
        return timeline.mapToX(tick)
    }

    ScopicFlowInternal.Timeline {
        id: timeline
        anchors.fill: parent
        timeViewModel: control.timeViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        playbackViewModel: control.playbackViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        interactionControllerNotifier: control.interactionControllerNotifier
    }
}