import QtQml
import QtQuick
import ScopicFlowPrivate

import '.'
import './HelperComponents'
import "qrc:/ScopicFlow/modules/dev/sjimo/ScopicFlow/Palettes" as ScopicFlowPalette

TrackList {
    id: trackList

    property QtObject defaultPalette: ScopicFlowPalette.TrackList {}
    property QtObject palette: paletteViewModel?.palette?.trackList ?? defaultPalette

    property bool trackHandleEnabled: true
    onHeightChanged: {
        if (!trackList.trackListViewModel)
            return
        if (trackList.trackListViewModel.viewportOffset + trackList.height > backgroundRectangle.height) {
            trackList.trackListViewModel.viewportOffset = backgroundRectangle.height - trackList.height
        }
    }

    NumberAnimation {
        id: viewportOffsetAnimation
        target: trackListViewModel
        property: "viewportOffset"
        easing.type: Easing.OutCubic
        duration: (trackList.animationViewModel?.scrollAnimationRatio ?? 1) * 250
    }

    Rectangle {
        id: backgroundRectangle
        anchors.left: parent.left
        anchors.right: parent.right
        color: trackList.palette.backgroundColor
        y: -trackList.trackListViewModel?.viewportOffset ?? 0
        height: Math.max(trackList.height, trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1) ? trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1).y + 2 * trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1).height : 0, trackList.height - y)

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
                        function onLayoutRequired() { trackListDelegate.trackViewModel = trackList.trackAt(trackListDelegate.index) }
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

        Item {
            id: trackHandles
            anchors.fill: parent
            Rectangle {
                id: topTrackHandle
                property bool indicatesTarget: false
                anchors.left: parent.left
                anchors.right: parent.right
                y:  -2
                opacity: indicatesTarget
                height: 4
                color: trackList.palette.primaryColor
            }
            Repeater {
                id: trackHandlesRepeater
                model: trackLayoutRepeater.count
                Rectangle {
                    id: trackHandle
                    required property int index
                    property var trackViewModel: trackList.trackAt(index)
                    property bool indicatesTarget: false
                    Connections {
                        target: trackList
                        function onLayoutRequired() { trackHandle.trackViewModel = trackList.trackAt(trackHandle.index) }
                    }
                    anchors.left: parent.left
                    anchors.right: parent.right
                    y: trackLayoutRepeater.itemAt(index).y + trackLayoutRepeater.itemAt(index).height - 2
                    opacity: indicatesTarget || trackHandleMouseArea.containsPressed || trackHandleMouseArea.originalY !== -1
                    Behavior on opacity {
                        NumberAnimation {
                            duration: (trackList.animationViewModel?.visualEffectAnimationRatio ?? 1.0) * 250
                            easing.type: Easing.OutCubic
                        }
                    }
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

        MouseArea {
            function interactionTarget (mouse, determineInteractive = true) {
                let point = Qt.point(mouse.x, mouse.y);
                let flag = false
                let item = null
                if (trackHandles.childAt(point.x, point.y))
                    return null
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
            function indexAt (x, y) {
                let point = Qt.point(x, y);
                let item = null
                for (let child = trackLayout.childAt(point.x, point.y); child;) {
                    if (child.isTrackListDelegate) {
                        item = child
                        break
                    }
                    point = child.parent.mapToItem(child, point)
                    child = child.childAt(point.x, point.y)
                }
                if (item)
                    return item.index
                if (backgroundRectangle.contains(Qt.point(x, y)))
                    return trackLayoutRepeater.count
                return -1
            }
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            cursorShape: undefined
            property bool rejectClick: false
            property var pressedItem: null

            property double deltaTickingY: 4
            function calculateDraggingScrollingSpeed(y) {
                return Math.min(1, y / 256)
            }
            Timer {
                id: tickingTimer
                interval: 10
                repeat: true

                onTriggered: {
                    if (!trackList.trackListViewModel)
                        return
                    let newViewportOffset = Math.max(0, trackList.trackListViewModel.viewportOffset + parent.deltaTickingY)
                    if (newViewportOffset + trackList.height > backgroundRectangle.height) {
                        newViewportOffset = backgroundRectangle.height - trackList.height
                    }
                    trackList.trackListViewModel.viewportOffset = newViewportOffset
                }
            }

            onPressed: function (mouse) {
                rejectClick = false
                let item = interactionTarget(mouse)
                if (!item) {
                    mouse.accepted = false
                } else {
                    pressedItem = item
                }
            }
            property int lastIndicatorIndex: -1
            onPositionChanged: function (mouse) {
                if (!pressedItem || typeof(pressedItem.index) !== "number")
                    return
                rejectClick = true
                let index = indexAt(mouse.x, mouse.y)
                if (mouse.modifiers & Qt.ControlModifier) {
                    cursorShape = Qt.CrossCursor
                    if (lastIndicatorIndex !== -1) {
                        let handle = (lastIndicatorIndex ? trackHandlesRepeater.itemAt(lastIndicatorIndex - 1) : topTrackHandle)
                        handle.indicatesTarget = false
                    }
                    if (index >= 0 && index < trackLayoutRepeater.count) {
                        trackList.trackAt(index).selected = true
                        trackList.trackListViewModel.currentIndex = index
                        pressedItem = trackLayoutRepeater.itemAt(index)
                    }
                    return
                }
                cursorShape = Qt.ClosedHandCursor
                let viewportPoint = trackList.mapFromItem(backgroundRectangle, mouse.x, mouse.y)
                if (viewportPoint.y < 0) {
                    deltaTickingY = -calculateDraggingScrollingSpeed(-viewportPoint.y) * tickingTimer.interval
                    tickingTimer.start()
                    return
                } else if (viewportPoint.y > trackList.height) {
                    deltaTickingY = calculateDraggingScrollingSpeed(viewportPoint.y - trackList.height) * tickingTimer.interval
                    tickingTimer.start()
                    return
                } else {
                    tickingTimer.stop()
                }
                if (lastIndicatorIndex !== -1) {
                    let handle = (lastIndicatorIndex ? trackHandlesRepeater.itemAt(lastIndicatorIndex - 1) : topTrackHandle)
                    handle.indicatesTarget = false
                }
                if (index !== -1) {
                    let handle = (index ? trackHandlesRepeater.itemAt(index - 1) : topTrackHandle)
                    handle.indicatesTarget = true
                }
                lastIndicatorIndex = index
            }
            onReleased: function (mouse) {
                cursorShape = undefined
                tickingTimer.stop()
                if (lastIndicatorIndex !== -1) {
                    if (pressedItem) {
                        if (mouse.button & Qt.LeftButton) {
                            trackList.handleTrackMoved(pressedItem.index, lastIndicatorIndex)
                        } else {
                            trackList.contextMenuRequestedForTrackDragging(pressedItem.index, lastIndicatorIndex)
                        }
                    }
                    let handle = (lastIndicatorIndex ? trackHandlesRepeater.itemAt(lastIndicatorIndex - 1) : topTrackHandle)
                    handle.indicatesTarget = false
                } else {
                    if (rejectClick && pressedItem) {
                        trackList.contextMenuRequestedForTrack(pressedItem.index)
                    }
                }
                lastIndicatorIndex = -1
            }
            onClicked: function (mouse) {
                if (rejectClick)
                    return
                if (!pressedItem)
                    return
                if (mouse.button & Qt.RightButton) {
                    if (typeof(pressedItem.index) === "number")
                        trackList.trackListViewModel.currentIndex = pressedItem.index
                    trackList.contextMenuRequestedForTrack(pressedItem.index ?? -1)
                    return
                }
                let multipleSelect = Boolean(mouse.modifiers & Qt.ControlModifier)
                let extendingSelect = Boolean(mouse.modifiers & Qt.ShiftModifier)
                let previousSelected = typeof(pressedItem.index) === "number" && pressedItem.trackViewModel.selected
                let previousSelectionCount = 0
                if (!multipleSelect || typeof(pressedItem.index) !== "number") {
                    for (let i = 0; i < trackList.trackListViewModel.count; i++) {
                        let track = trackList.trackAt(i)
                        if (track.selected) {
                            track.selected = false
                            previousSelectionCount++
                        }
                    }
                }
                if (typeof(pressedItem.index) === "number") {
                    if (extendingSelect) {
                        for (let i = trackList.trackListViewModel.currentIndex; i <= pressedItem.index; i++) {
                            trackList.trackAt(i).selected = true
                        }
                    } else {
                        trackList.trackListViewModel.currentIndex = pressedItem.index
                        pressedItem.trackViewModel.selected = previousSelectionCount > 1 || !previousSelected
                    }
                }
            }

            onDoubleClicked: function (mouse) {
                if (!pressedItem)
                    return
                trackList.trackDoubleClicked(pressedItem.index ?? -1)
            }
        }

    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: trackList.scrollBehaviorViewModel
        onMoved: function (_, deltaY) {
            if (!trackList.trackListViewModel)
                return
            let newViewportOffset = Math.max(0, trackList.trackListViewModel.viewportOffset + deltaY)
            if (newViewportOffset + trackList.height > backgroundRectangle.height) {
                newViewportOffset = backgroundRectangle.height - trackList.height
            }
            trackList.trackListViewModel.viewportOffset = newViewportOffset
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: trackList.scrollBehaviorViewModel
        onMoved: function (_, deltaY, isPhysicalWheel) {
            if (!trackList.trackListViewModel)
                return
            let newViewportOffset = Math.max(0, trackList.trackListViewModel.viewportOffset + deltaY)
            if (newViewportOffset + trackList.height > backgroundRectangle.height) {
                newViewportOffset = backgroundRectangle.height - trackList.height
            }
            if (isPhysicalWheel) {
                viewportOffsetAnimation.to = newViewportOffset
                viewportOffsetAnimation.start()
            } else {
                trackList.trackListViewModel.viewportOffset = newViewportOffset
            }
        }
    }


}