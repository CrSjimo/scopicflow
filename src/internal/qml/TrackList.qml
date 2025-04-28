import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: trackList
    clip: true

    property QtObject trackListViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject transactionControllerNotifier: null
    property Component trackExtraDelegate: null
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
            trackList.transactionControllerNotifier?.transactionAboutToBegin()
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
            trackList.transactionControllerNotifier?.transactionCommitted()
        }
        for (let i = 0; i < handle.count; i++) {
            if (handle.itemAt(i) === currentItem) {
                handle.currentIndex = i
                break
            }
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
        color: SFPalette.trackListBackgroundColor
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

        MouseArea {
            id: backMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            cursorShape: undefined
            hoverEnabled: true
            property bool dragged: false
            property point pressedPoint: Qt.point(0, 0)

            Connections {
                target: dragScroller
                function onPositionChanged(x, y, modifiers) {
                    let point = backMouseArea.mapFromItem(trackList, x, y)
                    backMouseArea.handlePositionChanged(point.x, point.y, modifiers)
                }
            }

            function handlePositionChanged(x, y, modifiers) {
                if (!rubberBandLayer.started) {
                    selectionManipulator.select(null, Qt.RightButton, modifiers)
                    trackList.transactionControllerNotifier?.transactionAboutToBegin()
                    rubberBandLayer.startSelection(Qt.point(x, y))
                } else {
                    rubberBandLayer.updateSelection(Qt.point(x, y))
                }

            }

            function sendInteractionNotification(interactionType) {
                if (!handleBeforeInteractionNotification(interactionType))
                    return false
                emitInteractionNotificationSignal(interactionType)
                return true
            }
            function handleBeforeInteractionNotification(interactionType) {
                if (trackList.interactionControllerNotifier?.handleSceneInteraction(interactionType, trackList.trackListViewModel, trackList.trackListLayoutViewModel, -1, 0))
                    return false
                return true
            }
            function emitInteractionNotificationSignal(interactionType) {
                trackList.interactionControllerNotifier?.sceneInteracted(interactionType, trackList.trackListViewModel, trackList.trackListLayoutViewModel, -1, 0)
            }

            onPressed: (mouse) => {
                dragged = false
                pressedPoint = Qt.point(mouse.x, mouse.y)
                if (!sendInteractionNotification(ScopicFlow.II_Pressed))
                    mouse.accepted = false
            }
            onReleased: () => {
                if (rubberBandLayer.started) {
                    rubberBandLayer.endSelection()
                    trackList.transactionControllerNotifier?.transactionCommitted()
                }
                dragScroller.running = false
                sendInteractionNotification(ScopicFlow.II_Released)
            }
            onCanceled: () => {
                if (rubberBandLayer.started) {
                    rubberBandLayer.endSelection()
                    trackList.transactionControllerNotifier?.transactionAborted()
                }
                dragScroller.running = false
                sendInteractionNotification(ScopicFlow.II_Canceled)
            }
            onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
            onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
            onClicked: (mouse) => {
                if (mouse.button === Qt.LeftButton && !dragged) {
                    if (!handleBeforeInteractionNotification(ScopicFlow.II_Clicked))
                        return
                    selectionManipulator.select(null, mouse.button, mouse.modifiers)
                    emitInteractionNotificationSignal(ScopicFlow.II_Clicked)
                } else if (mouse.button === Qt.RightButton) {
                    let interactionType = dragged || (mouse.modifiers & Qt.ControlModifier) ? ScopicFlow.II_ItemContextMenu : ScopicFlow.II_ContextMenu
                    if (!handleBeforeInteractionNotification(interactionType))
                        return
                    selectionManipulator.select(null, mouse.button, mouse.modifiers | (dragged ? Qt.ControlModifier : 0))
                    emitInteractionNotificationSignal(interactionType)
                }
            }
            onDoubleClicked: sendInteractionNotification(ScopicFlow.II_DoubleClicked)
            onPositionChanged: (mouse) => {
                if (!pressed)
                    return
                dragged = true
                let viewportPoint = mapToItem(trackList, mouse.x, mouse.y)
                dragScroller.viewportPoint = viewportPoint
                dragScroller.modifiers = mouse.modifiers
                dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                    if (!triggered)
                        handlePositionChanged(mouse.x, mouse.y, mouse.modifiers)
                })
            }
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
                    trackListViewModel: trackList.trackListViewModel
                    trackExtraDelegate: trackList.trackExtraDelegate
                    anchors.left: parent.left
                    anchors.right: parent.right
                    trackNumber: index + 1

                    isLast: index === trackList.trackListViewModel?.handle.count
                    isCurrent: trackList.trackListViewModel?.handle.currentIndex === index

                    animationViewModel: trackList.animationViewModel
                    interactionControllerNotifier: trackList.interactionControllerNotifier
                    transactionControllerNotifier: trackList.transactionControllerNotifier

                    height: trackViewModel.rowHeight
                    y: trackListLocator.map[index] ?? 0
                    onHeightChanged: () => { // FIXME 改成类似 Slider 那样的绑定关系
                        trackViewModel.rowHeight = height
                        height = Qt.binding(function () { return this.trackViewModel.rowHeight })
                        rubberBandLayer.insertItem(index, Qt.rect(0, y, 1 << 20, height))
                    }
                    onYChanged: () => {
                        rubberBandLayer.insertItem(index, Qt.rect(0, y, 1 << 20, height))
                    }
                    Component.onDestruction: () => {
                        rubberBandLayer.removeItem(index)
                    }
                    mouseArea: MouseArea {
                        id: trackMouseArea
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        cursorShape: undefined
                        focusPolicy: Qt.StrongFocus
                        hoverEnabled: true
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
                        function sendInteractionNotification(interactionType) {
                            if (!handleBeforeInteractionNotification(interactionType))
                                return false
                            emitInteractionNotificationSignal(interactionType)
                            return true
                        }
                        function handleBeforeInteractionNotification(interactionType) {
                            if (trackList.interactionControllerNotifier?.handleItemInteraction(interactionType, trackListDelegate.trackViewModel, trackListDelegate.index, trackList.trackListViewModel, ScopicFlow.InteractionOnTrackItem))
                                return false
                            return true
                        }
                        function emitInteractionNotificationSignal(interactionType) {
                            trackList.interactionControllerNotifier?.itemInteracted(interactionType, trackListDelegate.trackViewModel, trackListDelegate.index, trackList.trackListViewModel, ScopicFlow.InteractionOnTrackItem)
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

                        onPressed: (mouse) => {
                            dragged = false
                            if (!sendInteractionNotification(ScopicFlow.II_Pressed))
                                mouse.accepted = false
                        }
                        onReleased: (mouse) => {
                            cursorShape = undefined
                            dragScroller.running = false
                            if (!handleBeforeInteractionNotification(ScopicFlow.II_Released))
                                return
                            if (lastIndicatorIndex !== -1) {
                                if (mouse.button === Qt.LeftButton) {
                                    trackList.moveTrack(trackListDelegate.index, lastIndicatorIndex)
                                }
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex)
                                handle.indicatesTarget = false
                            }
                            lastIndicatorIndex = -1
                            emitInteractionNotificationSignal(ScopicFlow.II_Released)
                        }
                        onCanceled: () => {
                            cursorShape = undefined
                            dragScroller.running = false
                            rubberBandLayer.endSelection()
                            if (lastIndicatorIndex !== -1) {
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex)
                                handle.indicatesTarget = false
                            }
                            lastIndicatorIndex = -1
                            sendInteractionNotification(ScopicFlow.II_Canceled)
                        }
                        onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
                        onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
                        onClicked: (mouse) => {
                            if (mouse.button === Qt.LeftButton && !dragged) {
                                if (!handleBeforeInteractionNotification(ScopicFlow.II_Clicked))
                                    return
                                selectionManipulator.select(trackListDelegate.index, mouse.button, mouse.modifiers)
                                emitInteractionNotificationSignal(ScopicFlow.II_Clicked)
                            } else if (mouse.button === Qt.RightButton) {
                                if (!handleBeforeInteractionNotification(ScopicFlow.II_ContextMenu))
                                    return
                                selectionManipulator.select(trackListDelegate.index, mouse.button, mouse.modifiers)
                                emitInteractionNotificationSignal(ScopicFlow.II_ContextMenu)
                            }
                        }
                        onDoubleClicked: sendInteractionNotification(ScopicFlow.II_DoubleClicked)
                        onPositionChanged: (mouse) => {
                            if (!pressed)
                                return
                            dragged = true
                            selectionManipulator.select(trackListDelegate.index, Qt.RightButton, mouse.modifiers)
                            let viewportPoint = mapToItem(trackList, mouse.x, mouse.y)
                            dragScroller.viewportPoint = viewportPoint
                            dragScroller.modifiers = mouse.modifiers
                            dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                                if (!triggered)
                                    handlePositionChanged(mouse.x, mouse.y, mouse.modifiers)
                            })
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
                            duration: Theme.visualEffectAnimationDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                    height: 4
                    color: Theme.accentColor

                    MouseArea {
                        id: trackHandleMouseArea
                        anchors.fill: parent
                        cursorShape: Qt.SplitVCursor
                        property double originalY: -1
                        enabled: trackHandle.index !== 0
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

                        function sendInteractionNotification(interactionType) {
                            if (!handleBeforeInteractionNotification(interactionType))
                                return false
                            emitInteractionNotificationSignal(interactionType)
                            return true
                        }
                        function handleBeforeInteractionNotification(interactionType) {
                            if (trackHandle.index === 0)
                                return false
                            if (trackList.interactionControllerNotifier?.handleItemInteraction(interactionType, trackHandle.trackViewModel, trackHandle.index - 1, trackList.trackListViewModel, ScopicFlow.InteractionOnTrackHandle))
                                return false
                            return true
                        }
                        function emitInteractionNotificationSignal(interactionType) {
                            trackList.interactionControllerNotifier?.itemInteracted(interactionType, trackHandle.trackViewModel, trackHandle.index - 1, trackList.trackListViewModel, ScopicFlow.InteractionOnTrackHandle)
                        }

                        onPressed: (mouse) => {
                            originalY = mouse.y
                            if (!sendInteractionNotification(ScopicFlow.II_Pressed)) {
                                mouse.accepted = false
                            }
                            trackList.transactionControllerNotifier?.transactionAboutToBegin()
                        }
                        onReleased: () => {
                            dragScroller.running = false
                            originalY = -1
                            trackList.transactionControllerNotifier?.transactionCommitted()
                            sendInteractionNotification(ScopicFlow.II_Released)
                        }
                        onCanceled: () => {
                            dragScroller.running = false
                            originalY = -1
                            trackList.transactionControllerNotifier?.transactionAborted()
                            sendInteractionNotification(ScopicFlow.II_Canceled)
                        }
                        onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
                        onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
                        onClicked: sendInteractionNotification(ScopicFlow.II_Clicked)
                        onDoubleClicked: () => {
                            if (!handleBeforeInteractionNotification(ScopicFlow.II_DoubleClieked))
                                return
                            let delegate = trackLayoutRepeater.itemAt(trackHandle.index - 1)
                            delegate.fitHeight()
                            emitInteractionNotificationSignal(ScopicFlow.II_DoubleClicked)
                        }
                        onPositionChanged: (mouse) => {
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
                    }
                }
            }
        }

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            rubberBand: RubberBandRectangle {
            }
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: trackList.scrollBehaviorViewModel
        direction: Qt.Vertical
        onMoved: (_, deltaY) => {
            trackListManipulator.moveViewBy(deltaY)
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: trackList.scrollBehaviorViewModel
        movableOrientation: Qt.Vertical
        zoomableOrientation: 0
        onMoved: (_, deltaY, isPhysicalWheel) => {
            trackListManipulator.moveViewBy(deltaY, isPhysicalWheel)
        }
    }

}