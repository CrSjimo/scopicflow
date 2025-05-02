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
    property QtObject interactionControllerNotifier: null
    property QtObject transactionControllerNotifier: null
    
    padding: 0
    focusPolicy: Qt.StrongFocus

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
        interactionControllerNotifier: control.interactionControllerNotifier
        transactionControllerNotifier: control.transactionControllerNotifier

    }

}