import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject scrollBehaviorViewModel: null
    property Component trackExtraDelegate: null
    property QtObject trackListLayoutViewModel: null
    property QtObject trackListViewModel: null
    property QtObject transactionControllerNotifier: null

    focusPolicy: Qt.StrongFocus
    padding: 0

    ScopicFlowInternal.TrackList {
        anchors.fill: parent
        animationViewModel: control.animationViewModel
        interactionControllerNotifier: control.interactionControllerNotifier
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        trackExtraDelegate: control.trackExtraDelegate
        trackListLayoutViewModel: control.trackListLayoutViewModel
        trackListViewModel: control.trackListViewModel
        transactionControllerNotifier: control.transactionControllerNotifier
    }
}
