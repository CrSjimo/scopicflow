import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject labelSequenceBehaviorViewModel: null
    property QtObject labelSequenceViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    property QtObject transactionControllerNotifier: null

    focusPolicy: Qt.StrongFocus
    padding: 0

    ScopicFlowInternal.LabelSequence {
        id: labelSequence

        anchors.fill: parent
        animationViewModel: control.animationViewModel
        interactionControllerNotifier: control.interactionControllerNotifier
        labelSequenceBehaviorViewModel: control.labelSequenceBehaviorViewModel
        labelSequenceViewModel: control.labelSequenceViewModel
        playbackViewModel: control.playbackViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        timeLayoutViewModel: control.timeLayoutViewModel
        timeViewModel: control.timeViewModel
        transactionControllerNotifier: control.transactionControllerNotifier
    }
}
