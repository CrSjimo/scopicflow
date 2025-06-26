import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject trackListViewModel: null
    property QtObject transactionControllerNotifier: null

    focusPolicy: Qt.StrongFocus
    implicitWidth: mixer.implicitWidth
    padding: 0

    ScopicFlowInternal.Mixer {
        id: mixer

        anchors.fill: parent
        animationViewModel: control.animationViewModel
        interactionControllerNotifier: control.interactionControllerNotifier
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        trackListLayoutViewModel: control.trackListLayoutViewModel
        trackListViewModel: control.trackListViewModel
        transactionControllerNotifier: control.transactionControllerNotifier
    }
}
