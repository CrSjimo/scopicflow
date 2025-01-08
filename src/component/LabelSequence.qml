import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject timeAlignmentViewModel: null
    property QtObject playbackViewModel: null
    property QtObject labelSequenceViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null

    padding: 0
    focusPolicy: Qt.StrongFocus

    signal contextMenuRequested(tick: int)
    signal contextMenuRequestedForLabel(label: QtObject)

    ScopicFlowInternal.LabelSequence {
        id: labelSequence
        anchors.fill: parent

        timeAlignmentViewModel: control.timeAlignmentViewModel
        playbackViewModel: control.playbackViewModel
        labelSequenceViewModel: control.labelSequenceViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel

        onContextMenuRequested: tick => control.contextMenuRequested(tick)
        onContextMenuRequestedForLabel: label => control.contextMenuRequestedForLabel(label)
    }

}