import QtQml
import QtQuick

import SVSCraft
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
    property bool rightAligned: false

    function moveTrack(index, target) {
        if (!trackListViewModel) {
            return false;
        }
        if (selectionController.getSelectedItems().length <= 1 && (target === index || target === index + 1)) {
            return false;
        }
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
        return true
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
    Item {
        id: viewport

        anchors.left: parent.left
        anchors.right: parent.right
        height: Math.max(trackListManipulator.viewportHeight, trackList.height)
        y: -trackList.trackListLayoutViewModel?.viewportOffset ?? 0

        RubberBandDragHandler {
            id: rubberBandDragHandler
            controller: trackList.trackListInteractionController
            target: trackList
            verticalManipulator: trackListManipulator
            rubberBandLayer: rubberBandLayer
            selectionController: trackList.selectionController
        }
        DispatcherMouseArea {
            determineDragHandler: (mouse) => {
                let m = {
                    [TrackListInteractionController.RubberBandSelect]: rubberBandDragHandler,
                }
                if (mouse.modifiers & Qt.ControlModifier) {
                    return ((mouse.modifiers & Qt.AltModifier) ?
                        m[trackList.trackListInteractionController.secondarySelectInteraction] :
                        m[trackList.trackListInteractionController.primarySelectInteraction]) ?? null
                } else {
                    return ((mouse.modifiers & Qt.AltModifier) ?
                        m[trackList.trackListInteractionController.secondarySceneInteraction] :
                        m[trackList.trackListInteractionController.primarySceneInteraction]) ?? null
                }

            }
            onClicked: (mouse) => {
                if (trackList.trackListInteractionController.clickSelectable) {
                    trackList.selectionController.selectByMouse(null, Qt.LeftButton, mouse.modifiers);
                }
            }
            onDoubleClicked: () => {
                trackList.trackListInteractionController.doubleClicked(trackList)
            }
        }
        GenericBackRightButtonMouseArea {
            id: backRightButtonMouseArea

            selectionController: trackList.selectionController
            controller: trackList.trackListInteractionController
            target: trackList
        }
        GenericBackHoverMouseArea {
            id: backHoverMouseArea

            controller: trackList.trackListInteractionController
            target: trackList
        }
        Item {
            id: trackLayout

            property bool dragging: false

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

                    property double animatedHeight: 0
                    readonly property NumberAnimation fitHeightAnimation: fitHeightAnimation
                    NumberAnimation on animatedHeight {
                        id: fitHeightAnimation
                        running: false
                        duration: Theme.visualEffectAnimationDuration
                        easing.type: Easing.OutCubic
                        to: 80
                    }

                    function fitHeight() {
                        fitHeightAnimation.from = trackListDelegate.trackViewModel.rowHeight
                        fitHeightAnimation.start()
                    }

                    width: parent.width
                    height: fitHeightAnimation.running ? animatedHeight : trackViewModel.rowHeight
                    y: trackListManipulator.map[index]
                    trackNumber: (index + 1).toLocaleString()
                    trackViewModel: trackList.trackListViewModel.items[index]
                    selectionController: trackList.selectionController
                    rightAligned: trackList.rightAligned

                    mouseArea: Item {
                        component TrackDragHandler: DispatchedDragHandler {
                            id: trackDragHandler
                            property bool shouldCopyTrack: false
                            onDragStarted: () => {
                                trackList.trackListInteractionController.dragMovingStarted(trackList, trackListDelegate.index)
                                if (trackList.trackListInteractionController.clickSelectable) {
                                    trackList.selectionController.selectByMouse(trackListDelegate.trackViewModel, Qt.RightButton, modifiers);
                                }
                            }
                            onDragMoved: (_, y) => {
                                const parentPoint = mapToItem(trackList, 0, y)
                                trackDragScroller.determine(0, 0, parentPoint.y, trackList.height, (_, triggered) => {
                                    if (!triggered)
                                        trackDragScroller.handlePositionChanged(y);
                                });
                            }
                            onDragFinished: () => {
                                if (trackDragScroller.lastIndicatorIndex !== -1) {
                                    if (shouldCopyTrack) {
                                        trackList.trackListInteractionController.dragMovingAborted(trackList, trackListDelegate.index)
                                        trackList.trackListInteractionController.copyItemsRequested(trackList, trackDragScroller.lastIndicatorIndex)
                                    } else if (trackList.moveTrack(trackListDelegate.index, trackDragScroller.lastIndicatorIndex)) {
                                        trackList.trackListInteractionController.dragMovingCommitted(trackList, trackListDelegate.index)
                                    } else {
                                        trackList.trackListInteractionController.dragMovingAborted(trackList, trackListDelegate.index)
                                    }
                                    let handle = trackHandlesRepeater.itemAt(trackDragScroller.lastIndicatorIndex);
                                    handle.indicatesTarget = false;
                                } else {
                                    trackList.trackListInteractionController.dragMovingAborted(trackList, trackListDelegate.index)
                                }
                                trackDragScroller.lastIndicatorIndex = -1;
                                trackDragScroller.running = trackDragScroller.draggingStarted = false;
                            }
                            onDragCanceled: () => {
                                trackList.trackListInteractionController.dragMovingAborted(trackList, trackListDelegate.index)
                                if (trackDragScroller.lastIndicatorIndex !== -1) {
                                    let handle = trackHandlesRepeater.itemAt(trackDragScroller.lastIndicatorIndex);
                                    handle.indicatesTarget = false;
                                }
                                trackDragScroller.lastIndicatorIndex = -1;
                                trackDragScroller.running = trackDragScroller.draggingStarted = false;
                            }
                            DragScroller {
                                id: trackDragScroller

                                property int lastIndicatorIndex: -1
                                property bool draggingStarted: false

                                function handlePositionChanged(y) {
                                    if (!draggingStarted) {
                                        if (Math.abs(y - trackDragHandler.startPoint.y) < 4) {
                                            return
                                        }
                                    }
                                    draggingStarted = true
                                    let point = mapToItem(trackLayout, 0, y);
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

                                onMoved: (_, deltaY) => {
                                    trackListManipulator.moveViewBy(deltaY);
                                    handlePositionChanged(deltaY > 0 ? trackList.height : 0);
                                }
                            }
                        }
                        TrackDragHandler {
                            id: dragMoveHandler
                        }
                        TrackDragHandler {
                            id: dragCopyHandler
                            shouldCopyTrack: true
                        }
                        DispatcherMouseArea {
                            determineDragHandler: (mouse) => {
                                let m = {
                                    [TrackListInteractionController.DragMove]: dragMoveHandler,
                                    [TrackListInteractionController.DragCopy]: dragCopyHandler,
                                    [TrackListInteractionController.RubberBandSelect]: rubberBandDragHandler,
                                }
                                if (mouse.modifiers & Qt.ControlModifier) {
                                    return ((mouse.modifiers & Qt.AltModifier) ?
                                        m[trackList.trackListInteractionController.secondarySelectInteraction] :
                                        m[trackList.trackListInteractionController.primarySelectInteraction]) ?? null
                                } else {
                                    return ((mouse.modifiers & Qt.AltModifier) ?
                                        m[trackList.trackListInteractionController.secondaryItemInteraction] :
                                        m[trackList.trackListInteractionController.primaryItemInteraction]) ?? null
                                }

                            }
                            onClicked: (mouse) => {
                                if (trackList.trackListInteractionController.clickSelectable) {
                                    trackList.selectionController.selectByMouse(trackListDelegate.trackViewModel, Qt.LeftButton, mouse.modifiers);
                                }
                            }
                            onDoubleClicked: () => {
                                trackList.trackListInteractionController.itemDoubleClicked(trackList, trackListDelegate.index)
                            }
                        }
                        GenericRightButtonMouseArea {
                            id: trackRightButtonMouseArea
                            controller: trackList.trackListInteractionController
                            paneItem: trackList
                            viewModel: trackListDelegate.trackViewModel
                            selectionController: trackList.selectionController
                        }
                        MouseArea {
                            id: trackHoverMouseArea
                            acceptedButtons: Qt.NoButton

                            anchors.fill: parent

                            hoverEnabled: true
                            onEntered: () => {
                                trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.index, TrackListInteractionController.ItemBackground)
                            }

                            onExited: () => {
                                trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.index, TrackListInteractionController.ItemBackground)
                            }
                        }
                    }

                    Component.onDestruction: () => {
                        rubberBandLayer.removeItem(trackViewModel);
                    }
                    onHeightChanged: () => {
                        rubberBandLayer.insertItem(trackViewModel, Qt.rect(0, y, 1 << 20, height));
                    }
                    onYChanged: () => {
                        rubberBandLayer.insertItem(trackViewModel, Qt.rect(0, y, 1 << 20, height));
                    }

                    colorIndicator.onClicked: () => {
                        trackList.trackListInteractionController.itemColorIndicatorClicked(trackList, index)
                    }
                    colorIndicator.onHoveredChanged: () => {
                        if (colorIndicator.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.ColorIndicator)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.ColorIndicator)
                        }
                    }

                    trackMSR.muteButton.visible: Boolean(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditMute)
                    trackMSR.soloButton.visible: Boolean(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditSolo)
                    trackMSR.recordButton.visible: Boolean(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditRecord)

                    trackMSR.muteButton.onClicked: () => {
                        trackList.trackListInteractionController.muteEditingStarted(trackList, index)
                        trackViewModel.mute = trackMSR.muteButton.checked
                        trackList.trackListInteractionController.muteEditingFinished(trackList, index)
                    }

                    trackMSR.soloButton.onClicked: () => {
                        trackList.trackListInteractionController.soloEditingStarted(trackList, index)
                        trackViewModel.solo = trackMSR.soloButton.checked
                        trackList.trackListInteractionController.soloEditingFinished(trackList, index)
                    }

                    trackMSR.recordButton.onClicked: () => {
                        trackList.trackListInteractionController.recordEditingStarted(trackList, index)
                        trackViewModel.record = trackMSR.recordButton.checked
                        trackList.trackListInteractionController.recordEditingFinished(trackList, index)
                    }

                    trackMSR.muteButton.onHoveredChanged: () => {
                        if (trackMSR.muteButton.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.MuteButton)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.MuteButton)
                        }
                    }

                    trackMSR.soloButton.onHoveredChanged: () => {
                        if (trackMSR.soloButton.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.SoloButton)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.SoloButton)
                        }
                    }

                    trackMSR.recordButton.onHoveredChanged: () => {
                        if (trackMSR.recordButton.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.RecordButton)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.RecordButton)
                        }
                    }

                    trackNameEditLabel.readOnly: !(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditName)
                    trackNameEditLabel.onEditingStarted: trackList.trackListInteractionController.nameEditingStarted(trackList, index)
                    trackNameEditLabel.onEditingCommitted: (text) => {
                        trackViewModel.name = text
                        trackList.trackListInteractionController.nameEditingCommitted(trackList, index)
                    }
                    trackNameEditLabel.onEditingAborted: trackList.trackListInteractionController.nameEditingAborted(trackList, index)
                    trackNameEditLabel.onHoveredChanged: () => {
                        if (trackNameEditLabel.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.NameLabel)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.NameLabel)
                        }
                    }

                    gainSlider.enabled: Boolean(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditGain)
                    gainSlider.onPressedChanged: () => {
                        if (gainSlider.pressed) {
                            trackList.trackListInteractionController.gainEditingStarted(trackList, index)
                        } else {
                            trackList.trackListInteractionController.gainEditingCommitted(trackList, index)
                        }
                    }
                    gainSlider.onMoved: trackViewModel.gain = SVS.linearValueToDecibel(gainSlider.value + SVS.decibelToLinearValue(0))
                    gainSlider.ThemedItem.onDoubleClickReset: () => {
                        trackList.trackListInteractionController.gainEditingStarted(trackList, index)
                        trackViewModel.gain = SVS.linearValueToDecibel(SVS.decibelToLinearValue(0))
                        trackList.trackListInteractionController.gainEditingCommitted(trackList, index)
                    }
                    gainSlider.onHoveredChanged: () => {
                        if (gainSlider.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.GainSlider)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.GainSlider)
                        }
                    }

                    gainEditLabel.readOnly: !(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditGain)
                    gainEditLabel.onEditingStarted: trackList.trackListInteractionController.gainEditingStarted(trackList, index)
                    gainEditLabel.onEditingCommitted: (text) => {
                        trackViewModel.gain = Number.fromLocaleString(Qt.locale(), text)
                        trackList.trackListInteractionController.gainEditingCommitted(trackList, index)
                    }
                    gainEditLabel.onEditingAborted: trackList.trackListInteractionController.gainEditingAborted(trackList, index)
                    gainEditLabel.onHoveredChanged: () => {
                        if (gainEditLabel.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.GainLabel)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.GainLabel)
                        }
                    }

                    panDial.enabled: Boolean(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditPan)
                    panDial.onPressedChanged: () => {
                        if (panDial.pressed) {
                            trackList.trackListInteractionController.panEditingStarted(trackList, index)
                        } else {
                            trackList.trackListInteractionController.panEditingCommitted(trackList, index)
                        }
                    }
                    panDial.onMoved: trackViewModel.pan = panDial.value
                    panDial.ThemedItem.onDoubleClickReset: () => {
                        trackList.trackListInteractionController.panEditingStarted(trackList, index)
                        trackViewModel.pan = 0
                        trackList.trackListInteractionController.panEditingCommitted(trackList, index)
                    }
                    panDial.onHoveredChanged: () => {
                        if (panDial.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.PanDial)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.PanDial)
                        }
                    }

                    panEditLabel.readOnly: !(trackList.trackListInteractionController.itemAction & TrackListInteractionController.EditPan)
                    panEditLabel.onEditingStarted: trackList.trackListInteractionController.panEditingStarted(trackList, index)
                    panEditLabel.onEditingCommitted: (text) => {
                        trackViewModel.pan = Number.fromLocaleString(Qt.locale(), text) / 100
                        trackList.trackListInteractionController.panEditingCommitted(trackList, index)
                    }
                    panEditLabel.onEditingAborted: trackList.trackListInteractionController.panEditingAborted(trackList, index)
                    panEditLabel.onHoveredChanged: () => {
                        if (panEditLabel.hovered) {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.PanLabel)
                        } else {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.PanLabel)
                        }
                    }

                    levelMeterMouseArea.onClicked: () => {
                        trackList.trackListInteractionController.itemLevelMeterClicked(trackList, trackListDelegate.trackViewModel)
                    }
                    levelMeterMouseArea.onEntered: () => {
                        trackList.trackListInteractionController.itemHoverEntered(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.LevelMeter)
                    }
                    levelMeterMouseArea.onExited: () => {
                        trackList.trackListInteractionController.itemHoverExited(trackList, trackListDelegate.trackViewModel, TrackListInteractionController.LevelMeter)
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
                    property NumberAnimation currentFitHeightAnimation: null
                    property double animatedHeight: 0

                    width: parent.width
                    height: index === 0 ? 2 : currentFitHeightAnimation?.running ? animatedHeight : trackViewModel.rowHeight

                    Rectangle {
                        color: Theme.borderColor
                        width: parent.width
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 1.5
                        height: 1
                    }

                    Rectangle {
                        color: Theme.accentColor
                        width: parent.width
                        height: 4
                        anchors.bottom: parent.bottom
                        opacity: (trackHandle.indicatesTarget || trackHandleMouseArea.pressed || trackHandleMouseArea.originalY !== -1) ? 1 : 0

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
                        property bool dragged: false

                        function handlePositionChanged(x, y) {
                            let deltaSize = y - originalY;
                            let newHeight = Math.max(40, trackHandle.trackViewModel.rowHeight + deltaSize);
                            trackHandle.trackViewModel.rowHeight = newHeight;
                        }

                        width: parent.width
                        height: 4
                        anchors.bottom: parent.bottom
                        cursorShape: Qt.SplitVCursor
                        enabled: trackHandle.index !== 0
                        hoverEnabled: trackHandle.index !== 0
                        CursorBinding.cursorShape: cursorShape

                        onCanceled: () => {
                            CursorBinding.enabled = false
                            dragScroller.running = false;
                            originalY = -1;
                            if (dragged) {
                                trackList.trackListInteractionController.heightAdjustingFinished(trackList, trackHandle.index - 1)
                            }
                        }
                        onDoubleClicked: () => {
                            if (trackHandle.trackViewModel.rowHeight === 80) {
                                return
                            }
                            let delegate = trackLayoutRepeater.itemAt(trackHandle.index - 1);
                            trackHandle.currentFitHeightAnimation = delegate.fitHeightAnimation
                            trackHandle.animatedHeight = Qt.binding(() => delegate.animatedHeight)
                            delegate.fitHeight();
                            trackList.trackListInteractionController.heightAdjustingStarted(trackList, trackHandle.index - 1)
                            trackHandle.trackViewModel.rowHeight = 80
                            trackList.trackListInteractionController.heightAdjustingFinished(trackList, trackHandle.index - 1)
                        }
                        onPositionChanged: mouse => {
                            if (!(trackList.trackListInteractionController.itemAction & TrackListInteractionController.AdjustHeight)) {
                                return;
                            }
                            if (originalY === -1) {
                                return;
                            }
                            if (!dragged) {
                                dragged = true;
                                if (trackHandle.currentFitHeightAnimation) {
                                    trackHandle.currentFitHeightAnimation.stop();
                                    trackHandle.currentFitHeightAnimation = null;
                                }
                                trackList.trackListInteractionController.heightAdjustingStarted(trackList, trackHandle.index - 1)
                            }
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
                            dragged = false
                            CursorBinding.enabled = true
                        }
                        onReleased: canceled()

                        onEntered: () => {
                            trackList.trackListInteractionController.itemHoverEntered(trackList, trackHandle.index - 1, TrackListInteractionController.HeightAdjustmentHandle)
                        }

                        onExited: () => {
                            trackList.trackListInteractionController.itemHoverExited(trackList, trackHandle.index - 1, TrackListInteractionController.HeightAdjustmentHandle)
                        }

                        DragScroller {
                            id: dragScroller

                            property int modifiers: 0
                            property point viewportPoint: Qt.point(0, 0)

                            function handlePositionChanged(x, y) {
                                let point = trackHandleMouseArea.mapFromItem(trackList, x, y);
                                trackHandleMouseArea.handlePositionChanged(point.x, point.y);
                            }

                            onMoved: (_, deltaY) => {
                                trackListManipulator.moveViewBy(deltaY);
                                positionChanged(viewportPoint.x, deltaY > 0 ? trackList.height : 0, modifiers);
                            }
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
