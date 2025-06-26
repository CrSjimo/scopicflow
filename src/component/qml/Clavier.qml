import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control

    property QtObject animationViewModel: null
    property double bottomMargin: 0
    property QtObject clavierViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject scrollBehaviorViewModel: null
    property double topMargin: 0

    function mapToKey(y) {
        return clavier.mapToKey(y);
    }

    focusPolicy: Qt.StrongFocus
    padding: 0

    ScopicFlowInternal.Clavier {
        id: clavier

        anchors.fill: parent
        animationViewModel: control.animationViewModel
        bottomMargin: control.bottomMargin
        clavierViewModel: control.clavierViewModel
        interactionControllerNotifier: control.interactionControllerNotifier
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        topMargin: control.topMargin
    }
}
