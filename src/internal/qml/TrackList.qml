import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Palette as ScopicFlowPalette

TrackListInternal {
    id: trackList

    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null

    property Component trackExtraDelegate: null

    signal trackDoubleClicked(index: int);
    signal contextMenuRequestedForTrack(index: int);
    signal contextMenuRequestedForTrackDragging(index: int, target: int);

    property QtObject defaultPalette: ScopicFlowPalette.TrackList {}
    property QtObject palette: paletteViewModel?.palette?.trackList ?? defaultPalette

    clip: true

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

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: trackList.trackListViewModel
    }

    Rectangle {
        id: backgroundRectangle
        anchors.left: parent.left
        anchors.right: parent.right
        color: trackList.palette.backgroundColor
        y: -trackList.trackListViewModel?.viewportOffset ?? 0
        height: Math.max(trackList.height, trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1) ? trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1).y + 2 * trackLayoutRepeater.itemAt(trackLayoutRepeater.count - 1).height : 0, trackList.height - y)

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            cursorShape: undefined
            focusPolicy: Qt.StrongFocus
            property bool rejectClick: false
            onPressed: {
                rejectClick = false
            }
            onClicked: function (mouse) {
                if (rejectClick)
                    return
                selectionManipulator.select(null, mouse.button, mouse.modifiers)
                if (mouse.button & Qt.RightButton) {
                    trackList.contextMenuRequestedForTrack(-1)
                }
            }
            function handlePositionChanged(x, y, modifiers) {
                if (!rubberBandLayer.started) {
                    let multipleSelect = Boolean(modifiers & Qt.ControlModifier)
                    if (!multipleSelect) {
                        for (let i = 0; i < trackList.trackListViewModel.count; i++) {
                            let track = trackList.trackAt(i)
                            if (track.selected) {
                                track.selected = false
                            }
                        }
                    }
                    rubberBandLayer.startSelection(Qt.point(x, y))
                } else {
                    rubberBandLayer.updateSelection(Qt.point(x, y))
                }

            }
            DragScroller {
                id: dragScroller
                property point viewportPoint: Qt.point(0, 0)
                property int modifiers: 0
                onMoved: function (_, deltaY) {
                    if (!trackList.trackListViewModel)
                        return
                    let newViewportOffset = Math.max(0, trackList.trackListViewModel.viewportOffset + deltaY)
                    if (newViewportOffset + trackList.height > backgroundRectangle.height) {
                        newViewportOffset = backgroundRectangle.height - trackList.height
                    }
                    trackList.trackListViewModel.viewportOffset = newViewportOffset
                    let point = parent.mapFromItem(trackList, viewportPoint)
                    parent.handlePositionChanged(point.x, point.y, modifiers)
                }
            }
            onPositionChanged: function (mouse) {
                rejectClick = true
                let viewportPoint = mapToItem(trackList, mouse.x, mouse.y)
                dragScroller.viewportPoint = viewportPoint
                dragScroller.modifiers = mouse.modifiers
                if (viewportPoint.y < 0) {
                    dragScroller.distanceY = viewportPoint.y
                    dragScroller.running = true
                    return
                } else if (viewportPoint.y > trackList.height) {
                    dragScroller.distanceY = viewportPoint.y - trackList.height
                    dragScroller.running = true
                    return
                } else {
                    dragScroller.running = false
                }
                handlePositionChanged(mouse.x, mouse.y, mouse.modifiers)
            }
            onReleased: {
                rubberBandLayer.endSelection()
                dragScroller.running = false
            }
            onDoubleClicked: {
                trackList.trackDoubleClicked(-1)
            }
        }

        Column {
            id: trackLayout
            anchors.fill: parent
            function indexAt (point) {
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
                if (backgroundRectangle.contains(point))
                    return trackLayoutRepeater.count
                if (backgroundRectangle.contains(Qt.point(point.x, 0))) {
                    if (point.y < 0)
                        return 0
                    else
                        return trackLayoutRepeater.count
                }
                return -1
            }
            Repeater {
                id: trackLayoutRepeater
                model: trackList.trackListViewModel?.count ?? 0
                TrackListDelegate {
                    id: trackListDelegate
                    readonly property bool isTrackListDelegate: true
                    required property int index
                    trackViewModel: trackList.trackAt(index)
                    trackExtraDelegate: trackList.trackExtraDelegate
                    Connections {
                        target: trackList
                        function onLayoutRequired() { trackListDelegate.trackViewModel = trackList.trackAt(trackListDelegate.index) }
                    }
                    anchors.left: parent.left
                    anchors.right: parent.right
                    palette: trackList.palette
                    trackNumber: index + 1

                    isLast: index === trackList.trackListViewModel.count
                    isCurrent: trackList.trackListViewModel?.currentIndex === index

                    animationViewModel: trackList.animationViewModel

                    height: trackViewModel.rowHeight
                    onHeightChanged: {
                        trackViewModel.rowHeight = height
                        height = Qt.binding(function () { return this.trackViewModel.rowHeight })
                        rubberBandLayer.insertItem(index, Qt.rect(0, y, 1 << 20, height))
                    }
                    onYChanged: {
                        rubberBandLayer.insertItem(index, Qt.rect(0, y, 1 << 20, height))
                    }
                    Component.onDestruction: {
                        rubberBandLayer.removeItem(index)
                    }
                    mouseArea: MouseArea {

                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        cursorShape: undefined
                        focusPolicy: Qt.StrongFocus
                        property bool rejectClick: false
                        
                        function handlePositionChanged(x, y, modifiers) {
                            let point = mapToItem(trackLayout, x, y)
                            let index = trackLayout.indexAt(point)
                            if ((modifiers & Qt.AltModifier) || rubberBandLayer.started) {
                                if (lastIndicatorIndex !== -1) {
                                    let handle = (lastIndicatorIndex ? trackHandlesRepeater.itemAt(lastIndicatorIndex - 1) : topTrackHandle)
                                    handle.indicatesTarget = false
                                }
                                if (!rubberBandLayer.started) {
                                    let multipleSelect = Boolean(modifiers & Qt.ControlModifier)
                                    if (!multipleSelect) {
                                        for (let i = 0; i < trackList.trackListViewModel.count; i++) {
                                            let track = trackList.trackAt(i)
                                            if (track.selected) {
                                                track.selected = false
                                            }
                                        }
                                    }
                                    rubberBandLayer.startSelection(point)
                                } else {
                                    rubberBandLayer.updateSelection(point)
                                }
                                return
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

                        DragScroller {
                            id: dragScroller
                            property point viewportPoint: Qt.point(0, 0)
                            property int mouseModifiers: 0
                            onMoved: function (_, deltaY) {
                                if (!trackList.trackListViewModel)
                                    return
                                let newViewportOffset = Math.max(0, trackList.trackListViewModel.viewportOffset + deltaY)
                                if (newViewportOffset + trackList.height > backgroundRectangle.height) {
                                    newViewportOffset = backgroundRectangle.height - trackList.height
                                }
                                trackList.trackListViewModel.viewportOffset = newViewportOffset
                                let point = parent.mapFromItem(trackList, viewportPoint)
                                parent.handlePositionChanged(point.x, point.y, mouseModifiers)
                            }
                        }

                        onPressed: function (mouse) {
                            rejectClick = false
                        }
                        property int lastIndicatorIndex: -1
                        onPositionChanged: function (mouse) {
                            rejectClick = true
                            if (!(mouse.modifiers & Qt.AltModifier) && !rubberBandLayer.started) {
                                selectionManipulator.select(trackListDelegate.index, Qt.RightButton, mouse.modifiers)
                                cursorShape = Qt.ClosedHandCursor
                            }
                            let viewportPoint = mapToItem(trackList, mouse.x, mouse.y)
                            dragScroller.viewportPoint = viewportPoint
                            dragScroller.mouseModifiers = mouse.modifiers
                            if (viewportPoint.y < 0) {
                                dragScroller.distanceY = viewportPoint.y
                                dragScroller.running = true
                                return
                            } else if (viewportPoint.y > trackList.height) {
                                dragScroller.distanceY = viewportPoint.y - trackList.height
                                dragScroller.running = true
                                return
                            } else {
                                dragScroller.running = false
                            }
                            handlePositionChanged(mouse.x, mouse.y, mouse.modifiers)
                        }
                        onReleased: function (mouse) {
                            cursorShape = undefined
                            dragScroller.running = false
                            rubberBandLayer.endSelection()
                            if (lastIndicatorIndex !== -1) {
                                if (mouse.button & Qt.LeftButton) {
                                    trackList.handleTrackMoved(trackListDelegate.index, lastIndicatorIndex)
                                } else {
                                    trackList.contextMenuRequestedForTrackDragging(trackListDelegate.index, lastIndicatorIndex)
                                }
                                let handle = (lastIndicatorIndex ? trackHandlesRepeater.itemAt(lastIndicatorIndex - 1) : topTrackHandle)
                                handle.indicatesTarget = false
                            } else {
                                if (rejectClick && (mouse.button & Qt.RightButton)) {
                                    trackList.contextMenuRequestedForTrack(trackListDelegate.index)
                                }
                            }
                            lastIndicatorIndex = -1
                        }
                        onClicked: function (mouse) {
                            if (rejectClick)
                                return
                            selectionManipulator.select(trackListDelegate.index, mouse.button, mouse.modifiers)
                            if (mouse.button & Qt.RightButton) {
                                trackList.contextMenuRequestedForTrack(trackListDelegate.index ?? -1)
                            }
                        }

                        onDoubleClicked: function (mouse) {
                            trackList.trackDoubleClicked(trackListDelegate.index ?? -1)
                        }
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

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            rubberBand: Rectangle {
                color: trackList.palette.rubberBandColor
                border.width: 1
                border.color: trackList.palette.rubberBandBorderColor
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
                viewportOffsetAnimation.stop()
                viewportOffsetAnimation.to = newViewportOffset
                viewportOffsetAnimation.start()
            } else {
                trackList.trackListViewModel.viewportOffset = newViewportOffset
            }
        }
    }


}