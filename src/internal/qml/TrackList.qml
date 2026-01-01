import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: trackList

    property ListViewModel trackListViewModel: null
    property TrackListLayoutViewModel trackListLayoutViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TrackListInteractionController trackListInteractionController: null
    property SelectionController selectionController: null

    function moveTrack(index, target) {
        if (!trackListViewModel)
            return;
        let currentItem = selectionController.currentItem
        if (!trackListViewModel.items[index].selected) {
            if (target > index) {
                trackListViewModel.rotate(index, index + 1, target);
            } else {
                trackListViewModel.rotate(target, index, index + 1);
            }
        } else {
            let continuousSelectionStart = -1;
            let nextTarget = target;
            let i;
            for (i = target; i < trackListViewModel.count; i++) {
                if (trackListViewModel.items[i].selected && continuousSelectionStart === -1) {
                    continuousSelectionStart = i;
                } else if (!trackListViewModel.items[i].selected) {
                    if (continuousSelectionStart !== -1) {
                        trackListViewModel.rotate(nextTarget, continuousSelectionStart, i);
                        nextTarget = nextTarget + i - continuousSelectionStart;
                        continuousSelectionStart = -1;
                    }
                }
            }
            if (continuousSelectionStart !== -1) {
                trackListViewModel.rotate(nextTarget, continuousSelectionStart, i);
            }
            continuousSelectionStart = -1;
            nextTarget = target;
            for (i = target - 1; i >= 0; i--) {
                if (trackListViewModel.items[i].selected && continuousSelectionStart === -1) {
                    continuousSelectionStart = i;
                } else if (!trackListViewModel.items[i].selected) {
                    if (continuousSelectionStart !== -1) {
                        trackListViewModel.rotate(i + 1, continuousSelectionStart + 1, nextTarget);
                        nextTarget = nextTarget + i - continuousSelectionStart;
                        continuousSelectionStart = -1;
                    }
                }
            }
            if (continuousSelectionStart !== -1) {
                trackListViewModel.rotate(i + 1, continuousSelectionStart + 1, nextTarget);
            }
        }
        for (let i = 0; i < trackListViewModel.count; i++) {
            if (trackListViewModel.items[i] === currentItem) {
                selectionController.currentItem.currentItem = currentItem;
                break;
            }
        }
    }

    clip: true

    TrackListManipulator {
        id: trackListManipulator

        trackListLayoutViewModel: trackList.trackListLayoutViewModel
        trackListViewModel: trackList.trackListViewModel
        target: trackList
    }
    Rectangle {
        id: background

        anchors.fill: parent
        color: Theme.backgroundPrimaryColor
    }
    DragScroller {
        id: dragScroller

        property int modifiers: 0
        property point viewportPoint: Qt.point(0, 0)

        signal positionChanged(x: double, y: double, modifiers: int)

        onMoved: (_, deltaY) => {
            trackListManipulator.moveViewBy(deltaY);
            positionChanged(viewportPoint.x, deltaY > 0 ? trackList.height : 0, modifiers);
        }
    }
    Item {
        id: viewport

        anchors.left: parent.left
        anchors.right: parent.right
        height: Math.max(trackListManipulator.viewportHeight, trackList.height)
        y: -trackList.trackListLayoutViewModel?.viewportOffset ?? 0

        MouseArea {
            id: backMouseArea

            property bool dragged: false
            property point pressedPoint: Qt.point(0, 0)

            function handlePositionChanged(x, y, modifiers) {
                if (!rubberBandLayer.started) {
                    trackList.selectionController.selectByMouse(null, Qt.RightButton, modifiers);
                    rubberBandLayer.startSelection(Qt.point(x, y));
                } else {
                    rubberBandLayer.updateSelection(Qt.point(x, y));
                }
            }

            acceptedButtons: Qt.LeftButton
            anchors.fill: parent
            cursorShape: undefined
            hoverEnabled: true

            onCanceled: () => {
                if (rubberBandLayer.started) {
                    rubberBandLayer.endSelection(true);
                }
                dragScroller.running = false;
            }
            onClicked: mouse => {
                if (mouse.button === Qt.LeftButton && !dragged) {
                    trackList.selectionController.selectByMouse(null, mouse.button, mouse.modifiers);
                } else if (mouse.button === Qt.RightButton) {
                    trackList.selectionController.selectByMouse(null, mouse.button, mouse.modifiers | (dragged ? Qt.ControlModifier : 0));
                }
            }
            onPositionChanged: mouse => {
                if (!pressed)
                    return;
                dragged = true;
                let viewportPoint = mapToItem(trackList, mouse.x, mouse.y);
                dragScroller.viewportPoint = viewportPoint;
                dragScroller.modifiers = mouse.modifiers;
                dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                    if (!triggered)
                        handlePositionChanged(mouse.x, mouse.y, mouse.modifiers);
                });
            }
            onPressed: mouse => {
                dragged = false;
                pressedPoint = Qt.point(mouse.x, mouse.y);
            }
            onReleased: () => {
                if (rubberBandLayer.started) {
                    rubberBandLayer.endSelection(false);
                }
                dragScroller.running = false;
            }

            Connections {
                function onPositionChanged(x, y, modifiers) {
                    let point = backMouseArea.mapFromItem(trackList, x, y);
                    backMouseArea.handlePositionChanged(point.x, point.y, modifiers);
                }

                target: dragScroller
            }
        }
        Column {
            id: trackLayout

            spacing: 0

            property bool dragging: false

            // 获取 point 位置处音轨的 index
            function indexAt(point) {
                if (point.x < 0 || point.x > width) {
                    return -1;
                }
                return trackListManipulator.mapToPosition(point.y)
            }

            anchors.fill: parent

            Repeater {
                id: trackLayoutRepeater

                // model 只能用 count 并通过索引获取 trackViewModel，不能直接用 items，不然拖动音轨的时候旧的组件会被销毁导致出 bug
                model: trackList.trackListViewModel?.count ?? 0

                TrackListDelegate {
                    id: trackListDelegate

                    required property int index

                    width: parent.width
                    height: trackViewModel.rowHeight
                    trackNumber: (index + 1).toLocaleString()
                    trackViewModel: trackList.trackListViewModel.items[index]
                    selectionController: trackList.selectionController

                    mouseArea: MouseArea {
                        id: trackMouseArea

                        property bool dragged: false
                        property int lastIndicatorIndex: -1

                        function handlePositionChanged(x, y, modifiers) {
                            let point = mapToItem(trackLayout, x, y);
                            let index = trackLayout.indexAt(point);
                            if (lastIndicatorIndex !== -1) {
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex);
                                handle.indicatesTarget = false;
                            }
                            if (index !== -1) {
                                let handle = trackHandlesRepeater.itemAt(index);
                                handle.indicatesTarget = true;
                            }
                            lastIndicatorIndex = index;
                        }

                        acceptedButtons: Qt.LeftButton
                        cursorShape: undefined
                        focusPolicy: Qt.StrongFocus
                        hoverEnabled: true

                        onCanceled: () => {
                            cursorShape = undefined;
                            dragScroller.running = false;
                            rubberBandLayer.endSelection();
                            if (lastIndicatorIndex !== -1) {
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex);
                                handle.indicatesTarget = false;
                            }
                            lastIndicatorIndex = -1;
                        }
                        onClicked: mouse => {
                            if (!dragged) {
                                trackList.selectionController.selectByMouse(trackListDelegate.trackViewModel, mouse.button, mouse.modifiers);
                            }
                        }
                        onPositionChanged: mouse => {
                            if (!pressed)
                                return;
                            dragged = true;
                            trackList.selectionController.selectByMouse(trackListDelegate.trackViewModel, Qt.RightButton, mouse.modifiers);
                            let viewportPoint = mapToItem(trackList, mouse.x, mouse.y);
                            dragScroller.viewportPoint = viewportPoint;
                            dragScroller.modifiers = mouse.modifiers;
                            dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                                if (!triggered)
                                    handlePositionChanged(mouse.x, mouse.y, mouse.modifiers);
                            });
                        }
                        onPressed: mouse => {
                            dragged = false;
                        }
                        onPressedChanged: trackLayout.dragging = pressed
                        onReleased: mouse => {
                            cursorShape = undefined;
                            dragScroller.running = false;
                            if (lastIndicatorIndex !== -1) {
                                trackList.moveTrack(trackListDelegate.index, lastIndicatorIndex);
                                let handle = trackHandlesRepeater.itemAt(lastIndicatorIndex);
                                handle.indicatesTarget = false;
                            }
                            lastIndicatorIndex = -1;
                        }

                        Connections {
                            function onPositionChanged(x, y, modifiers) {
                                let point = trackMouseArea.mapFromItem(trackList, x, y);
                                trackMouseArea.handlePositionChanged(point.x, point.y, modifiers);
                            }

                            enabled: trackMouseArea.pressed
                            target: dragScroller
                        }
                    }

                    Component.onDestruction: () => {
                        rubberBandLayer.removeItem(trackViewModel);
                    }
                    onHeightChanged: () => {
                        // FIXME 改成类似 Slider 那样的绑定关系
                        trackViewModel.rowHeight = height;
                        height = Qt.binding(function () {
                            return this.trackViewModel.rowHeight;
                        });
                        rubberBandLayer.insertItem(trackViewModel, Qt.rect(0, y, 1 << 20, height));
                    }
                    onYChanged: () => {
                        rubberBandLayer.insertItem(trackViewModel, Qt.rect(0, y, 1 << 20, height));
                    }
                }
            }
        }
        Column {
            id: trackHandles

            anchors.fill: parent

            Repeater {
                id: trackHandlesRepeater

                model: (trackList.trackListViewModel?.count ?? 0) + 1

                Item {
                    id: trackHandle

                    required property int index
                    property bool indicatesTarget: false
                    property QtObject trackViewModel: index !== 0 ? trackList.trackListViewModel.items[index - 1] : null

                    width: parent.width
                    height: trackViewModel?.rowHeight ?? 0

                    Rectangle {
                        color: Theme.borderColor
                        width: parent.width
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: -0.5
                        height: 1
                    }

                    Rectangle {
                        color: Theme.accentColor
                        width: parent.width
                        height: 4
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: -2
                        opacity: trackHandle.indicatesTarget || trackHandleMouseArea.pressed || trackHandleMouseArea.originalY !== -1

                        Behavior on opacity {
                            NumberAnimation {
                                duration: Theme.visualEffectAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                    }

                    MouseArea {
                        id: trackHandleMouseArea

                        property double originalY: -1

                        function handlePositionChanged(x, y, modifiers) {
                            let deltaSize = y - originalY;
                            let newHeight = Math.max(40, trackHandle.trackViewModel.rowHeight + deltaSize);
                            trackHandle.trackViewModel.rowHeight = newHeight;
                        }

                        width: parent.width
                        height: 4
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: -2
                        cursorShape: Qt.SplitVCursor
                        enabled: trackHandle.index !== 0
                        hoverEnabled: true

                        onCanceled: () => {
                            dragScroller.running = false;
                            originalY = -1;
                        }
                        onDoubleClicked: () => {
                            let delegate = trackLayoutRepeater.itemAt(trackHandle.index - 1);
                            delegate.fitHeight();
                        }
                        onPositionChanged: mouse => {
                            if (originalY === -1)
                                return;
                            let viewportPoint = mapToItem(trackList, mouse.x, mouse.y);
                            dragScroller.viewportPoint = viewportPoint;
                            dragScroller.modifiers = mouse.modifiers;
                            dragScroller.determine(0, 0, viewportPoint.y, trackList.height, (_, triggered) => {
                                if (!triggered)
                                    handlePositionChanged(mouse.x, mouse.y, mouse.modifiers);
                            });
                        }
                        onPressed: mouse => {
                            originalY = mouse.y;
                        }
                        onReleased: () => {
                            dragScroller.running = false;
                            originalY = -1;
                        }

                        Connections {
                            function onPositionChanged(x, y, modifiers) {
                                let point = trackHandleMouseArea.mapFromItem(trackList, x, y);
                                trackHandleMouseArea.handlePositionChanged(point.x, point.y, modifiers);
                            }

                            enabled: trackHandleMouseArea.pressed
                            target: dragScroller
                        }
                    }
                }
            }
        }
        RubberBandLayer {
            id: rubberBandLayer

            anchors.fill: parent
            selectionController: trackList.selectionController

            rubberBand: RubberBandRectangle {
            }
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Vertical
        zoomableOrientation: 0
        pinchOrientationHint: Qt.Vertical
        viewModel: trackList.scrollBehaviorViewModel

        onMoved: (_, deltaY, isPhysicalWheel) => {
            trackListManipulator.moveViewBy(deltaY, isPhysicalWheel);
        }
    }
}
