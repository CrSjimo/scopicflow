import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: mixer
    clip: true
    implicitWidth: trackListManipulator.viewportHeight

    property QtObject trackListViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject transactionControllerNotifier: null

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: mixer.trackListViewModel
    }

    TrackListLocator {
        id: trackListLocator
        trackListViewModel: mixer.trackListViewModel
        overriddenRowHeight: 130
    }

    TrackListManipulator {
        id: trackListManipulator
        height: parent.width
        trackListLayoutViewModel: mixer.trackListLayoutViewModel
        animationViewModel: mixer.animationViewModel
        viewportHeight: Math.max(0, trackListLocator.totalHeight - 2)
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: SFPalette.trackListBackgroundColor
    }

    Item {
        id: viewport
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        x: -mixer.trackListLayoutViewModel?.viewportOffset ?? 0
        width: trackListManipulator.viewportHeight
        Repeater {
            id: repeater
            model: mixer.trackListViewModel?.handle.items ?? null
            delegate: Rectangle {
                id: mixerDelegate
                required property int index
                required property QtObject modelData
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                x: index * 130
                width: 130
                color: Theme.splitterColor
                MixerDelegate {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    trackViewModel: parent.modelData
                    trackListViewModel: mixer.trackListViewModel
                    index: parent.index
                    trackNumber: parent.index + 1
                    isCurrent: parent.index === mixer.trackListViewModel?.handle.currentIndex
                    animationViewModel: mixer.animationViewModel
                    interactionControllerNotifier: mixer.interactionControllerNotifier
                    transactionControllerNotifier: mixer.transactionControllerNotifier
                    mouseArea: MouseArea {
                        anchors.fill: parent
                        hoverEnabled: true
                        function sendInteractionNotification(interactionType) {
                            if (!handleBeforeInteractionNotification(interactionType))
                                return false
                            emitInteractionNotificationSignal(interactionType)
                            return true
                        }
                        function handleBeforeInteractionNotification(interactionType) {
                            if (mixer.interactionControllerNotifier?.handleItemInteraction(interactionType, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem))
                                return false
                            return true
                        }
                        function emitInteractionNotificationSignal(interactionType) {
                            mixer.interactionControllerNotifier?.itemInteracted(interactionType, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem)
                        }
                        onPressed: (mouse) => {
                            if (!sendInteractionNotification(ScopicFlow.II_Pressed))
                                mouse.accepted = false
                        }
                        onReleased: sendInteractionNotification(ScopicFlow.II_Released)
                        onCanceled: sendInteractionNotification(ScopicFlow.II_Canceled)
                        onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
                        onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
                        onClicked: (mouse) => {
                            if (!handleBeforeInteractionNotification(ScopicFlow.II_Click))
                                return
                            selectionManipulator.select(mixerDelegate.index, mouse.button, mouse.modifiers)
                            emitInteractionNotificationSignal(ScopicFlow.II_Click)
                        }
                        onDoubleClicked: sendInteractionNotification(ScopicFlow.II_DoubleClicked)
                    }
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: (mouse) => {
                            if (mixer.interactionControllerNotifier?.handleItemInteraction(ScopicFlow.II_ContextMenu, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem))
                                return
                            selectionManipulator.select(mixerDelegate.index, mouse.button, mouse.modifiers)
                            mixer.interactionControllerNotifier?.itemInteracted(ScopicFlow.II_ContextMenu, mixerDelegate.modelData, mixerDelegate.index, mixer.trackListViewModel, ScopicFlow.InteractionOnTrackItem)

                        }
                    }
                }
            }
        }
    }

    StyledScrollBar {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        orientation: Qt.Horizontal
        allowDragAdjustment: false
        size: mixer.width / trackListManipulator.viewportHeight
        position: (mixer.trackListLayoutViewModel?.viewportOffset ?? 0) / trackListManipulator.viewportHeight
        onPositionChanged: {
            if (mixer.trackListLayoutViewModel && Math.abs(mixer.trackListLayoutViewModel.viewportOffset - position * trackListManipulator.viewportHeight) > Number.EPSILON * 1000)
                mixer.trackListLayoutViewModel.viewportOffset = position * trackListManipulator.viewportHeight
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: mixer.scrollBehaviorViewModel
        direction: Qt.Horizontal
        onMoved: function (deltaX) {
            trackListManipulator.moveViewBy(deltaX)
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: mixer.scrollBehaviorViewModel
        movableOrientation: Qt.Horizontal
        zoomableOrientation: 0
        onMoved: function (deltaX, isPhysicalWheel) {
            trackListManipulator.moveViewBy(deltaX, isPhysicalWheel)
        }
    }
}