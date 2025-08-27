import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow

Item {
    id: mixer

    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject trackListViewModel: null
    property QtObject transactionControllerNotifier: null

    clip: true
    implicitWidth: trackListManipulator.viewportHeight

    SelectableViewModelManipulator {
        id: selectionManipulator

        viewModel: mixer.trackListViewModel
    }
    TrackListLocator {
        id: trackListLocator

        overriddenRowHeight: 130
        trackListViewModel: mixer.trackListViewModel
    }
    TrackListManipulator {
        id: trackListManipulator

        animationViewModel: mixer.animationViewModel
        height: parent.width
        trackListLayoutViewModel: mixer.trackListLayoutViewModel
        viewportHeight: Math.max(0, trackListLocator.totalHeight - 2)
    }
    Rectangle {
        id: background

        anchors.fill: parent
        color: SFPalette.trackListBackgroundColor
    }
    Item {
        id: viewport

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        width: trackListManipulator.viewportHeight
        x: -mixer.trackListLayoutViewModel?.viewportOffset ?? 0

        Repeater {
            id: repeater

            model: mixer.trackListViewModel?.handle.items ?? null

            delegate: Rectangle {
                id: mixerDelegate

                required property int index
                required property QtObject modelData

                anchors.bottom: parent.bottom
                anchors.top: parent.top
                color: Theme.splitterColor
                width: 130
                x: index * 130

                MixerDelegate {
                    anchors.bottom: parent.bottom
                    anchors.top: parent.top
                    animationViewModel: mixer.animationViewModel
                    index: parent.index
                    interactionControllerNotifier: mixer.interactionControllerNotifier
                    isCurrent: parent.index === mixer.trackListViewModel?.handle.currentIndex
                    trackListViewModel: mixer.trackListViewModel
                    trackNumber: parent.index + 1
                    trackViewModel: parent.modelData
                    transactionControllerNotifier: mixer.transactionControllerNotifier

                    mouseArea: MouseArea {
                        function emitInteractionNotificationSignal(interactionType) {
                            mixer.interactionControllerNotifier?.itemInteracted(interactionType, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem);
                        }
                        function handleBeforeInteractionNotification(interactionType) {
                            if (mixer.interactionControllerNotifier?.handleItemInteraction(interactionType, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem))
                                return false;
                            return true;
                        }
                        function sendInteractionNotification(interactionType) {
                            if (!handleBeforeInteractionNotification(interactionType))
                                return false;
                            emitInteractionNotificationSignal(interactionType);
                            return true;
                        }

                        anchors.fill: parent
                        hoverEnabled: true

                        onCanceled: sendInteractionNotification(ScopicFlow.II_Canceled)
                        onClicked: mouse => {
                            if (!handleBeforeInteractionNotification(ScopicFlow.II_Click))
                                return;
                            selectionManipulator.select(mixerDelegate.index, mouse.button, mouse.modifiers);
                            emitInteractionNotificationSignal(ScopicFlow.II_Click);
                        }
                        onDoubleClicked: sendInteractionNotification(ScopicFlow.II_DoubleClicked)
                        onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
                        onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
                        onPressed: mouse => {
                            if (!sendInteractionNotification(ScopicFlow.II_Pressed))
                                mouse.accepted = false;
                        }
                        onReleased: sendInteractionNotification(ScopicFlow.II_Released)
                    }

                    MouseArea {
                        acceptedButtons: Qt.RightButton
                        anchors.fill: parent

                        onClicked: mouse => {
                            if (mixer.interactionControllerNotifier?.handleItemInteraction(ScopicFlow.II_ContextMenu, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem))
                                return;
                            selectionManipulator.select(mixerDelegate.index, mouse.button, mouse.modifiers);
                            mixer.interactionControllerNotifier?.itemInteracted(ScopicFlow.II_ContextMenu, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem);
                        }
                    }
                }
            }
        }
    }
    StyledScrollBar {
        allowDragAdjustment: false
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        orientation: Qt.Horizontal
        position: (mixer.trackListLayoutViewModel?.viewportOffset ?? 0) / trackListManipulator.viewportHeight
        size: mixer.width / trackListManipulator.viewportHeight

        onPositionChanged: {
            if (mixer.trackListLayoutViewModel && Math.abs(mixer.trackListLayoutViewModel.viewportOffset - position * trackListManipulator.viewportHeight) > Number.EPSILON * 1000)
                mixer.trackListLayoutViewModel.viewportOffset = position * trackListManipulator.viewportHeight;
        }
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        direction: Qt.Horizontal
        viewModel: mixer.scrollBehaviorViewModel

        onMoved: function (deltaX) {
            trackListManipulator.moveViewBy(deltaX);
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Horizontal
        viewModel: mixer.scrollBehaviorViewModel
        zoomableOrientation: 0

        onMoved: function (deltaX, isPhysicalWheel) {
            trackListManipulator.moveViewBy(deltaX, isPhysicalWheel);
        }
    }
}
