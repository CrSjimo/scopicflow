import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Views

ApplicationWindow {
    id: main
    visible: true
    width: 1024
    height: 64

    required property TimeLayoutViewModel timeLayoutViewModel
    required property TimeViewModel timeViewModel
    required property PlaybackViewModel playbackViewModel
    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property TimelineInteractionController timelineInteractionController

    Timeline {
        id: timeline
        width: parent.width
        timeLayoutViewModel: main.timeLayoutViewModel
        timeViewModel: main.timeViewModel
        playbackViewModel: main.playbackViewModel
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        timelineInteractionController: main.timelineInteractionController
    }

}