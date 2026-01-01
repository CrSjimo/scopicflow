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
    width: 360
    height: 640

    required property ListViewModel trackListViewModel
    required property TrackListLayoutViewModel trackListLayoutViewModel
    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property TrackListInteractionController trackListInteractionController
    required property SelectionController selectionController

    TrackList {
        width: parent.width
        height: parent.height
        trackListViewModel: main.trackListViewModel
        trackListLayoutViewModel: main.trackListLayoutViewModel
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        trackListInteractionController: main.trackListInteractionController
        selectionController: main.selectionController
    }

}