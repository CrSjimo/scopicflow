import QtQml
import QtQuick
import QtQuick.Controls.Basic

import "./Private" as ScopicFlowPrivate

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

    padding: 0
    focusPolicy: Qt.StrongFocus

    function mapToKey(y) {
        return clavier.mapToKey(y)
    }

    signal notePressed(key: int)
    signal noteReleased(key: int)
    signal noteDoubleClicked(key: int)
    signal contextMenuRequestedForNote(key: int)

    ScopicFlowPrivate.Clavier {
        id: clavier
        anchors.fill: parent
        clavierViewModel: control.clavierViewModel
        scrollBehaviorViewModel: control.scrollBehaviorViewModel
        animationViewModel: control.animationViewModel
        paletteViewModel: control.paletteViewModel
        labelStrategy: control.labelStrategy

        onNotePressed: key => control.notePressed(key)
        onNoteReleased: key => control.noteReleased(key)
        onNoteDoubleClicked: key => control.noteDoubleClicked(key)
        onContextMenuRequestedForNote: key => control.contextMenuRequestedForNote(key)

    }

}