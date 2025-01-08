import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject timeAlignmentViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null

    padding: 0
    focusPolicy: Qt.StrongFocus

    function mapToTick(x) {
        return timeline.mapToTick(x)
    }
    function mapToX(tick) {
        return timeline.mapToX(tick)
    }
    signal positionIndicatorDoubleClicked()
    signal contextMenuRequestedForTimeline(tick: int)
    signal contextMenuRequestedForPositionIndicator()

    ScopicFlowInternal.Timeline {
        id: timeline
        anchors.fill: parent
        timeAlignmentViewModel: control.timeAlignmentViewModel
        playbackViewModel: control.playbackViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel
        onPositionIndicatorDoubleClicked: control.positionIndicatorDoubleClicked()
        onContextMenuRequestedForTimeline: tick => control.contextMenuRequestedForTimeline(tick)
        onContextMenuRequestedForPositionIndicator: control.contextMenuRequestedForPositionIndicator()
    }
}