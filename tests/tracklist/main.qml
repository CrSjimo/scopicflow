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
    height: 80

    required property TrackViewModel trackViewModel

    TrackListDelegate {
        width: parent.width
        height: parent.height
        selectionController: null
        trackViewModel: main.trackViewModel
        trackNumber: "1"
        current: true
    }

}