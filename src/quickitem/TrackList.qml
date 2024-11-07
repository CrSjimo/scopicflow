import QtQuick
import ScopicFlowPrivate

import '.'

TrackList {
    id: trackList

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        y: trackList.trackListViewModel?.viewportOffset ?? 0

        Column {
            anchors.fill: parent
            Repeater {
                model: trackList.trackListViewModel?.count ?? 0
                TrackListDelegate {
                    required property int index
                    property var trackViewModel: trackList.trackAt(index)
                    anchors.left: parent.left
                    anchors.right: parent.right
                    trackNumber: index + 1
                    trackName: trackViewModel.name
                    mute: trackViewModel.mute
                    solo: trackViewModel.solo
                    record: trackViewModel.record
                    gain: trackViewModel.gain
                    pan: trackViewModel.pan
                    leftLevel: trackViewModel.leftLevel
                    rightLevel: trackViewModel.rightLevel
                    selected: trackViewModel.selected
                    height: trackViewModel.rowHeight
                }
            }
        }

    }

}