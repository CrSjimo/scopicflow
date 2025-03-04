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
    property QtObject labelSequenceBehaviorViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    
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
        labelSequenceBehaviorViewModel: control.labelSequenceBehaviorViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        
        onContextMenuRequested: tick => control.contextMenuRequested(tick)
        onContextMenuRequestedForLabel: label => control.contextMenuRequestedForLabel(label)
    }

}