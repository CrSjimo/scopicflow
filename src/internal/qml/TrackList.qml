import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Palette as ScopicFlowPalette

Item {
    id: trackList
    clip: true

    property QtObject trackListViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null
    property QtObject defaultPalette: ScopicFlowPalette.TrackList {}
    property Component trackExtraDelegate: null

    readonly property QtObject palette: paletteViewModel?.palette?.trackList ?? defaultPalette

    signal trackDoubleClicked(index: int)
    signal contextMenuRequestedForTrack(index: int)
    signal contextMenuRequestedForTrackDragging(index: int, target: int)
    
    function moveTrack(index, target) {
        if (!trackListViewModel)
            return
        let handle = trackListViewModel.handle
        let currentItem = handle.itemAt(handle.currentIndex)
        if (!handle.itemAt(index).selected) {
            if (target > index) {
                handle.rotateItems(index, target - index, index + 1)
            } else {
                handle.rotateItems(target, index + 1 - target, index)
            }
        } else {
            handle.intermediate = true
            let continuousSelectionStart = -1
            let nextTarget = target
            let i
            for (i = target; i < handle.count; i++) {
                if (handle.itemAt(i).selected && continuousSelectionStart === -1) {
                    continuousSelectionStart = i
                } else if (!handle.itemAt(i).selected) {
                    if (continuousSelectionStart !== -1) {
                        handle.rotateItems(nextTarget, i - nextTarget, continuousSelectionStart)
                        nextTarget = nextTarget + i - continuousSelectionStart
                        continuousSelectionStart = -1
                    }
                }
            }
            if (continuousSelectionStart !== -1) {
                    handle.rotateItems(nextTarget, i - nextTarget, continuousSelectionStart)
            }
            continuousSelectionStart = -1
            nextTarget = target
            for (i = target - 1; i >= 0; i--) {
                if (handle.itemAt(i).selected && continuousSelectionStart === -1) {
                    continuousSelectionStart = i
                } else if (!handle.itemAt(i).selected) {
                    if (continuousSelectionStart !== -1) {
                            handle.rotateItems(i + 1, nextTarget - i - 1, continuousSelectionStart + 1)
                        nextTarget = nextTarget + i - continuousSelectionStart
                        continuousSelectionStart = -1
                    }
                }
            }
            if (continuousSelectionStart !== -1) {
                    handle.rotateItems(i + 1, nextTarget - i - 1, continuousSelectionStart + 1)
            }
            handle.intermediate = false
        }
        for (let i = 0; i < handle.count; i++) {
            if (handle.itemAt(i) === currentItem) {
                handle.currentIndex = i
                break
            }
        }
    }
    
    onHeightChanged: {
        if (!trackList.trackListViewModel)
            return
        if (trackList.trackListLayoutViewModel.viewportOffset + trackList.height > viewport.height) {
            trackList.trackListLayoutViewModel.viewportOffset = viewport.height - trackList.height
        }
    }

    NumberAnimation {
        id: viewportOffsetAnimation
        target: trackList.trackListLayoutViewModel
        property: "viewportOffset"
        easing.type: Easing.OutCubic
        duration: (trackList.animationViewModel?.scrollAnimationRatio ?? 1) * 250
    }

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: trackList.trackListViewModel
    }

    TrackListLocator {
        id: trackListLocator
        trackListViewModel: trackList.trackListViewModel
    }

    TrackListManipulator {
        id: trackListManipulator
        anchors.fill: parent
        trackListLayoutViewModel: trackList.trackListLayoutViewModel
        animationViewModel: trackList.animationViewModel
        viewportHeight: trackListLocator.viewportHeight
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: trackList.palette.backgroundColor
    }

    DragScroller {
        id: dragScroller
        property point viewportPoint: Qt.point(0, 0)
        property int modifiers: 0
        signal positionChanged(x: double, y: double, modifiers: int)
        onMoved: (_, deltaY) => {
            trackListManipulator.moveViewBy(deltaY)
            positionChanged(viewportPoint.x, deltaY > 0 ? trackList.height : 0, modifiers)
        }
    }

    Item {
        id: viewport
        anchors.left: parent.left
        anchors.right: parent.right
        y: -trackList.trackListLayoutViewModel?.viewportOffset ?? 0
        height: trackListLocator.viewportHeight

        Item {
            id: rubberBandQuasiMouseArea
            anchors.fill: parent
            property bool dragged: false
            property point pressedPoint: Qt.point(0, 0)
            property bool pressed: false

            Connections {
                target: dragScroller
                enabled: rubberBandQuasiMouseArea.pressed
                function onPositionChanged(x, y, modifiers) {
                    let point = rubberBandQuasiMouseArea.mapFromItem(trackList, x, y)
                    rubberBandQuasiMouseArea.handlePositionChanged(point.x, point.y, modifiers)
                }
            }

            function onPressed (mouse) {
                dragged = false
                pressedPoint = Qt.point(mouse.x, mouse.y)
            }
            function handlePositionChanged(x, y, modifiers) {
                if (!rubberBandLayer.started) {
                    selectionManipulator.select(null, Qt.RightButton, modifiers)
                    rubberBandLayer.startSelection(Qt.point(x, y))
                } else {
                    rubberBandLayer.updateSelection(Qt.point(x, y))
                }

            }
            function onPositionChanged (mouse) {
                dragged = true
                let viewportPoint = mapToItem(trackList, mouse.x, mouse.y)
                dragScroller.viewportPoint = viewportPoint
                dragScroller.modifiers = mouse.modifiers
                dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                    if (!triggered)
                        handlePositionChanged(mouse.x, mouse.y, mouse.modifiers)
                })
            }
            function onCanceled () {
                rubberBandLayer.endSelection()
                dragScroller.running = false
            }
        }

        MouseArea {
            id: backMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            cursorShape: undefined

            onPressedChanged: rubberBandQuasiMouseArea.pressed = pressed
            onPressed: (mouse) => rubberBandQuasiMouseArea.onPressed(mouse)
            onPositionChanged: (mouse) => rubberBandQuasiMouseArea.onPositionChanged(mouse)
            onReleased: canceled()
            onCanceled: rubberBandQuasiMouseArea.onCanceled()

            onClicked: function (mouse) {
                if (rubberBandQuasiMouseArea.dragged)
                    return
                selectionManipulator.select(null, mouse.button, mouse.modifiers)
                if (mouse.button & Qt.RightButton) {
                    trackList.contextMenuRequestedForTrack(-1)
                }
            }
            onDoubleClicked: {
                trackList.trackDoubleClicked(-1)
            }
            // TODO right button rubber band
        }

        Item {
            id: trackLayout
            anchors.fill: parent
            property bool dragging: false

            // 获取 point 位置处音轨的 index
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
                if (viewport.contains(point))
                    return trackLayoutRepeater.count
                if (viewport.contains(Qt.point(point.x, 0))) {
                    if (point.y < 0)
                        return 0
                    else
                        return trackLayoutRepeater.count
                }
                return -1
            }
            Repeater {
                id: trackLayoutRepeater
                // model 只能用 count 并通过索引获取 trackViewModel，不能直接用 items，不然拖动音轨的时候旧的组件会被销毁导致出 bug
                model: trackList.trackListViewModel?.handle.count ?? 0
                TrackListDelegate {
                    id: trackListDelegate
                    readonly property bool isTrackListDelegate: true
                    required property int index
                    trackViewModel: trackList.trackListViewModel.handle.items[index]
                    trackExtraDelegate: trackList.trackExtraDelegate
                    anchors.left: parent.left
                    anchors.right: parent.right
                    palette: trackList.palette
                    trackNumber: index + 1

                    isLast: index === trackList.trackListViewModel?.handle.count
                    isCurrent: trackList.trackListViewModel?.handle.currentIndex === index

                    animationViewModel: trackList.animationViewModel

                    height: trackViewModel.rowHeight
                    y: trackListLocator.map[index] ?? 0
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
                        id: trackMouseArea
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        cursorShape: undefined
                        focusPolicy: Qt.StrongFocus
                        property bool dragged: false
                        property int lastIndicatorIndex: -1

                        Connections {
                            target: dragScroller
                            enabled: trackMouseArea.pressed
                            function onPositionChanged(x, y, modifiers) {
                                let point = trackMouseArea.mapFromItem(trackList, x, y)
                                trackMouseArea.handlePositionChanged(point.x, point.y, modifiers)
                            }
                        }

                        onPressedChanged: trackLayout.dragging = pressed
                        
                        function handlePositionChanged(x, y, modifiers) {
                            let point = mapToItem(trackLayout, x, y)
                            let index = trackLayout.indexAt(point)
                            if (lastIndicatorIndex !== -1) {
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex)
                                handle.indicatesTarget = false
                            }
                            if (index !== -1) {
                                let handle = trackHandlesRepeater.itemAt(index)
                                handle.indicatesTarget = true
                            }
                            lastIndicatorIndex = index
                        }

                        onPressed: function (mouse) {
                            dragged = false
                        }
                        onPositionChanged: function (mouse) {
                            dragged = true
                            let viewportPoint = mapToItem(trackList, mouse.x, mouse.y)
                            dragScroller.viewportPoint = viewportPoint
                            dragScroller.modifiers = mouse.modifiers
                            dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                                if (!triggered)
                                    handlePositionChanged(mouse.x, mouse.y, mouse.modifiers)
                            })
                        }
                        onReleased: function (mouse) {
                            cursorShape = undefined
                            dragScroller.running = false
                            if (lastIndicatorIndex !== -1) {
                                if (mouse.button === Qt.LeftButton) {
                                    trackList.moveTrack(trackListDelegate.index, lastIndicatorIndex)
                                } else {
                                    trackList.contextMenuRequestedForTrackDragging(trackListDelegate.index, lastIndicatorIndex)
                                }
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex)
                                handle.indicatesTarget = false
                            } else {
                                if (dragged && (mouse.button & Qt.RightButton)) {
                                    trackList.contextMenuRequestedForTrack(trackListDelegate.index)
                                }
                            }
                            lastIndicatorIndex = -1
                        }
                        onCanceled: {
                            cursorShape = undefined
                            dragScroller.running = false
                            rubberBandLayer.endSelection()
                            if (lastIndicatorIndex !== -1) {
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex)
                                handle.indicatesTarget = false
                            }
                            lastIndicatorIndex = -1
                        }
                        onClicked: function (mouse) {
                            if (dragged)
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
            Repeater {
                id: trackHandlesRepeater
                model: trackLayoutRepeater.count + 1
                Rectangle {
                    id: trackHandle
                    required property int index
                    property QtObject trackViewModel: index !== 0 ? trackList.trackListViewModel.handle.items[index - 1] : null
                    property bool indicatesTarget: false
                    anchors.left: parent.left
                    anchors.right: parent.right
                    y: (trackListLocator.map[index] ?? 0) - height / 2
                    opacity: indicatesTarget || trackHandleMouseArea.pressed || trackHandleMouseArea.originalY !== -1
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
                        enabled: (trackList.trackListLayoutViewModel?.trackHandleEnabled ?? false) && trackHandle.index !== 0
                        hoverEnabled: true

                        Connections {
                            target: dragScroller
                            enabled: trackHandleMouseArea.pressed
                            function onPositionChanged(x, y, modifiers) {
                                let point = trackHandleMouseArea.mapFromItem(trackList, x, y)
                                trackHandleMouseArea.handlePositionChanged(point.x, point.y, modifiers)
                            }
                        }

                        function handlePositionChanged(x, y, modifiers) {
                            let deltaSize = y - originalY
                            let newHeight = Math.max(40, trackHandle.trackViewModel.rowHeight + deltaSize)
                            trackHandle.trackViewModel.rowHeight = newHeight
                        }

                        onPressed: function (mouse) {
                            originalY = mouse.y
                        }
                        onPositionChanged: function (mouse) {
                            if (originalY === -1)
                                return
                            let viewportPoint = mapToItem(trackList, mouse.x, mouse.y)
                            dragScroller.viewportPoint = viewportPoint
                            dragScroller.modifiers = mouse.modifiers
                            dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                                if (!triggered)
                                    handlePositionChanged(mouse.x, mouse.y, mouse.modifiers)
                            })
                        }
                        onReleased: canceled()
                        onDoubleClicked: {
                            let delegate = trackLayoutRepeater.itemAt(trackHandle.index - 1)
                            delegate.fitHeight()
                        }
                        onCanceled: {
                            dragScroller.running = false
                            originalY = -1
                        }
                    }
                }
            }
        }

        MouseArea {
            id: frontMouseArea
            anchors.fill: parent
            cursorShape: trackLayout.dragging ? Qt.ArrowCursor : undefined
            acceptedButtons: Qt.LeftButton | Qt.RightButton

            onPressedChanged: rubberBandQuasiMouseArea.pressed = pressed
            onPressed: (mouse) => {
                if (!(mouse.modifiers & Qt.AltModifier)) {
                    mouse.accepted = false
                    return
                }
                rubberBandQuasiMouseArea.onPressed(mouse)
            }
            onPositionChanged: (mouse) => rubberBandQuasiMouseArea.onPositionChanged(mouse)
            onReleased: canceled()
            onCanceled: rubberBandQuasiMouseArea.onCanceled()
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
        direction: Qt.Vertical
        onMoved: function (_, deltaY) {
            trackListManipulator.moveViewBy(deltaY)
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: trackList.scrollBehaviorViewModel
        onMoved: function (_, deltaY, isPhysicalWheel) {
            trackListManipulator.moveViewBy(deltaY, isPhysicalWheel)
        }
    }

}