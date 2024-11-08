import QtQml
import QtQuick
import ScopicFlowPrivate

import '.'

TrackList {
    id: trackList
    property bool trackHandleEnabled: true

    Rectangle {
        id: backgroundRectangle
        anchors.left: parent.left
        anchors.right: parent.right
        color: trackList.palette.backgroundColor
        y: trackList.trackListViewModel?.viewportOffset ?? 0
        height: (trackLayoutRepeater.count ? trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1).y + trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1).height : 0) + trackList.height

        Column {
            id: trackLayout
            anchors.fill: parent
            Repeater {
                id: trackLayoutRepeater
                model: trackList.trackListViewModel?.count ?? 0
                TrackListDelegate {
                    id: trackListDelegate
                    readonly property bool isTrackListDelegate: true
                    required property int index
                    property var trackViewModel: trackList.trackAt(index)
                    Connections {
                        target: trackList
                        function onLayoutRequired() { trackListDelegate.trackList = trackList.trackAt(trackListDelegate.index) }
                    }
                    anchors.left: parent.left
                    anchors.right: parent.right
                    palette: trackList.palette
                    trackNumber: index + 1
                    trackName: trackViewModel.name
                    onTrackNameChanged: trackViewModel.name = trackName

                    mute: trackViewModel.mute
                    onMuteChanged: trackViewModel.mute = mute
                    solo: trackViewModel.solo
                    onSoloChanged: trackViewModel.solo = solo
                    record: trackViewModel.record
                    onRecordChanged: trackViewModel.record = record

                    gain: trackViewModel.gain
                    onGainChanged: trackViewModel.gain = gain
                    pan: trackViewModel.pan
                    onPanChanged: trackViewModel.pan = pan

                    intermediate: trackViewModel.intermediate
                    onIntermediateChanged: trackViewModel.intermediate = intermediate

                    leftLevel: trackViewModel.leftLevel
                    rightLevel: trackViewModel.rightLevel

                    selected: trackViewModel.selected
                    isLast: index === trackList.trackListViewModel.count
                    isCurrent: trackList.trackListViewModel?.currentIndex === index
                    onSelectedChanged: trackViewModel.selected = selected

                    height: trackViewModel.rowHeight
                    onHeightChanged: {
                        trackViewModel.rowHeight = height
                        height = Qt.binding(function () { return this.trackViewModel.rowHeight })
                    }
                }
            }
        }

        MouseArea {
            function interactionTarget (mouse, determineInteractive = true) {
                let point = Qt.point(mouse.x, mouse.y);
                let flag = false
                let item = null
                if (!trackLayout.childAt(point.x, point.y))
                    return backgroundRectangle
                for (let child = trackLayout.childAt(point.x, point.y); child;) {
                    if (child.isTrackListDelegate) {
                        item = child
                        if (!determineInteractive)
                            break
                    }
                    flag = child.isMouseInteractionTarget
                    point = child.parent.mapToItem(child, point)
                    child = child.childAt(point.x, point.y)
                }
                if (flag)
                    return item
                return null
            }
            anchors.fill: parent
            onPressed: function (mouse) {
                let item = interactionTarget(mouse)
                if (!item) {
                    mouse.accepted = false
                }
            }
            onClicked: function (mouse) {
                let item = interactionTarget(mouse)
                let multipleSelect = Boolean(mouse.modifiers & Qt.ControlModifier)
                if (!multipleSelect || typeof(item.index) !== "number") {
                    for (let i = 0; i < trackList.trackListViewModel.count; i++) {
                        let track = trackList.trackAt(i)
                        track.selected = false
                    }
                }
                if (typeof(item.index) === "number") {
                    trackList.trackListViewModel.currentIndex = item.index
                    item.trackViewModel.selected = true
                }
            }
        }

        Repeater {
            model: trackLayoutRepeater.count
            Rectangle {
                id: trackHandle
                required property int index
                property var trackViewModel: trackList.trackAt(index)
                Connections {
                    target: trackList
                    function onLayoutRequired() { trackListDelegate.trackList = trackList.trackAt(trackListDelegate.index) }
                }
                anchors.left: parent.left
                anchors.right: parent.right
                y: trackLayoutRepeater.itemAt(index).y + trackLayoutRepeater.itemAt(index).height - 2
                opacity: trackHandleMouseArea.containsPressed || trackHandleMouseArea.containsMouse || trackHandleMouseArea.originalY !== -1
                height: 4
                color: trackList.palette.primaryColor

                MouseArea {
                    id: trackHandleMouseArea
                    anchors.fill: parent
                    cursorShape: Qt.SplitVCursor
                    property double originalY: -1
                    enabled: trackList.trackHandleEnabled
                    hoverEnabled: true

                    onPressed: function (mouse) {
                        originalY = mouse.y
                    }
                    onPositionChanged: function (mouse) {
                        if (originalY !== -1) {
                            let deltaSize = mouse.y - originalY;
                            let newHeight = Math.max(40, trackHandle.trackViewModel.rowHeight + deltaSize);
                            trackHandle.trackViewModel.rowHeight = newHeight
                        }
                    }
                    onReleased: function (mouse) {
                        originalY = -1
                    }
                }
            }
        }

    }

}