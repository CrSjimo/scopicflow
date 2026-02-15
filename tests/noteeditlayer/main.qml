import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Views
import dev.sjimo.ScopicFlow.Internal

ApplicationWindow {
    id: main
    visible: true
    width: 640
    height: 480

    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property SelectionController selectionController
    required property TimeViewModel timeViewModel
    required property TimeLayoutViewModel timeLayoutViewModel
    required property ClavierViewModel clavierViewModel
    required property RangeSequenceViewModel noteSequenceViewModel
    required property RangeSequenceViewModel noteSequenceViewModel2
    required property NoteEditLayerInteractionController noteEditLayerInteractionController
    required property ClipViewModel clipViewModel
    required property ClipViewModel clipViewModel2
    required property ListViewModel trackListViewModel

    PianoRollBackground {
        width: parent.width
        height: parent.height
        timeViewModel: main.timeViewModel
        timeLayoutViewModel: main.timeLayoutViewModel
        clavierViewModel: main.clavierViewModel
    }
    NoteEditLayer {
        width: parent.width
        height: parent.height
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        selectionController: main.selectionController
        timeViewModel: main.timeViewModel
        timeLayoutViewModel: main.timeLayoutViewModel
        clavierViewModel: main.clavierViewModel
        noteSequenceViewModel: main.noteSequenceViewModel
        noteEditLayerInteractionController: main.noteEditLayerInteractionController
        clipViewModel: main.clipViewModel
        trackListViewModel: main.trackListViewModel
        thumbnailDisplay: false
    }
    NoteEditLayer {
        width: parent.width
        height: parent.height
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        selectionController: main.selectionController
        timeViewModel: main.timeViewModel
        timeLayoutViewModel: main.timeLayoutViewModel
        clavierViewModel: main.clavierViewModel
        noteSequenceViewModel: main.noteSequenceViewModel2
        noteEditLayerInteractionController: main.noteEditLayerInteractionController
        clipViewModel: main.clipViewModel2
        trackListViewModel: main.trackListViewModel
        thumbnailDisplay: true
    }
    PianoRollScrollLayer {
        width: parent.width
        height: parent.height
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        clavierViewModel: main.clavierViewModel
        timeLayoutViewModel: main.timeLayoutViewModel
        timeViewModel: main.timeViewModel
    }

    Connections {
        target: main.noteEditLayerInteractionController
        function onItemDoubleClicked(noteEditLayer, item) {
            noteEditLayer.editLyricInPlace(item)
        }
    }

}