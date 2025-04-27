import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null

    property double topMargin: 0
    property double bottomMargin: 0

    padding: 0
    focusPolicy: Qt.StrongFocus

    function mapToKey(y) {
        return clavier.mapToKey(y)
    }

    ScopicFlowInternal.Clavier {
        id: clavier
        anchors.fill: parent
        clavierViewModel: control.clavierViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        interactionControllerNotifier: control.interactionControllerNotifier

        topMargin: control.topMargin
        bottomMargin: control.bottomMargin

    }

}