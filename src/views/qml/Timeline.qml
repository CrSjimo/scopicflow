import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null

    function mapToTick(x) {
        return timeline.mapToTick(x);
    }
    function mapToX(tick) {
        return timeline.mapToX(tick);
    }

    focusPolicy: Qt.StrongFocus
    padding: 0

    ScopicFlowInternal.Timeline {
        id: timeline

        anchors.fill: parent
        animationViewModel: control.animationViewModel
        interactionControllerNotifier: control.interactionControllerNotifier
        playbackViewModel: control.playbackViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        timeViewModel: control.timeViewModel
    }
}
