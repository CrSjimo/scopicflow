pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: noteEditLayerSequence
    property RangeSequenceViewModel clipSequenceViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property ClavierViewModel clavierViewModel: null
    property NoteEditLayerInteractionController noteEditLayerInteractionController: null
    property SelectionController selectionController: null
    property ListViewModel trackListViewModel: null

    property bool active: true
    property ClipViewModel editingItem: null
    property int bottomExpansion: 0

    SequenceSlicerLoader {
        range: [0, 1073741824] // TODO infinity range
        viewModel: noteEditLayerSequence.clipSequenceViewModel
        delegate: NoteEditLayer {
            anchors.fill: parent
            clipViewModel: SequenceSlicerLoader.viewModel
            noteSequenceViewModel: clipViewModel?.associatedNoteSequence ?? null
            scrollBehaviorViewModel: noteEditLayerSequence.scrollBehaviorViewModel
            timeLayoutViewModel: noteEditLayerSequence.timeLayoutViewModel
            timeViewModel: noteEditLayerSequence.timeViewModel
            clavierViewModel: noteEditLayerSequence.clavierViewModel
            noteEditLayerInteractionController: noteEditLayerSequence.noteEditLayerInteractionController
            selectionController: noteEditLayerSequence.selectionController
            trackListViewModel: noteEditLayerSequence.trackListViewModel
            active: noteEditLayerSequence.active
            readonly property bool current: noteEditLayerSequence.editingItem === clipViewModel
            thumbnailDisplay: !current
            clipBoundaryVisible: current
            bottomExpansion: noteEditLayerSequence.bottomExpansion
            z: thumbnailDisplay ? 0 : 1
        }
    }
}