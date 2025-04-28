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
    property QtObject interactionControllerNotifier: null
    property QtObject transactionControllerNotifier: null

    padding: 0
    focusPolicy: Qt.StrongFocus
    implicitWidth: mixer.implicitWidth

    ScopicFlowInternal.Mixer {
        id: mixer
        anchors.fill: parent
        trackListViewModel: control.trackListViewModel
        trackListLayoutViewModel: control.trackListLayoutViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel

        interactionControllerNotifier: control.interactionControllerNotifier
        transactionControllerNotifier: control.transactionControllerNotifier

    }
}