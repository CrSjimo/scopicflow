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

    required property ListViewModel trackListViewModel
    required property TrackListLayoutViewModel trackListLayoutViewModel
    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property SelectionController selectionController
    required property TimeViewModel timeViewModel
    required property TimeLayoutViewModel timeLayoutViewModel
    required property RangeSequenceViewModel clipSequenceViewModel
    required property ClipPaneInteractionController clipPaneInteractionController

    ClipPane {
        width: parent.width
        height: parent.height
        trackListViewModel: main.trackListViewModel
        trackListLayoutViewModel: main.trackListLayoutViewModel
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        selectionController: main.selectionController
        timeViewModel: main.timeViewModel
        timeLayoutViewModel: main.timeLayoutViewModel
        clipSequenceViewModel: main.clipSequenceViewModel
        clipPaneInteractionController: main.clipPaneInteractionController
    }

}