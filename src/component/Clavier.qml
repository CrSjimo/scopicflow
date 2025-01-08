import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Pane {
    id: control
    enum LabelStrategy {
        None,
        C,
        All
    }
    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null
    property int labelStrategy: Clavier.LabelStrategy.C
    property double topMargin: 0
    property double bottomMargin: 0

    padding: 0
    focusPolicy: Qt.StrongFocus

    function mapToKey(y) {
        return clavier.mapToKey(y)
    }

    signal notePressed(key: int)
    signal noteReleased(key: int)
    signal noteDoubleClicked(key: int)
    signal contextMenuRequestedForNote(key: int)

    ScopicFlowInternal.Clavier {
        id: clavier
        anchors.fill: parent
        clavierViewModel: control.clavierViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel
        labelStrategy: control.labelStrategy
        topMargin: control.topMargin
        bottomMargin: control.bottomMargin

        onNotePressed: key => control.notePressed(key)
        onNoteReleased: key => control.noteReleased(key)
        onNoteDoubleClicked: key => control.noteDoubleClicked(key)
        onContextMenuRequestedForNote: key => control.contextMenuRequestedForNote(key)

    }

}