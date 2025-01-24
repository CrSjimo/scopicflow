import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject labelSequenceViewModel: null
    property QtObject labelSequenceLayoutViewModel: null
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

        timeViewModel: control.timeViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        playbackViewModel: control.playbackViewModel
        labelSequenceViewModel: control.labelSequenceViewModel
        labelSequenceLayoutViewModel: control.labelSequenceLayoutViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel

        onContextMenuRequested: tick => control.contextMenuRequested(tick)
        onContextMenuRequestedForLabel: label => control.contextMenuRequestedForLabel(label)
    }

}