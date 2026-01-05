import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: mixer

    property ListViewModel trackListViewModel: null
    property TrackListLayoutViewModel trackListLayoutViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TrackListInteractionController trackListInteractionController: null
    property SelectionController selectionController: null
    property int trackWidth: 128
    readonly property double contentWidth: trackListManipulator.viewportHeight

    function moveViewBy(deltaX, animated = false) {
        trackListManipulator.moveViewBy(deltaX, animated)
    }

    clip: true
    implicitWidth: trackWidth

    TrackListManipulator {
        id: trackListManipulator

        trackListLayoutViewModel: mixer.trackListLayoutViewModel
        trackListViewModel: mixer.trackListViewModel
        viewSize: mixer.width
        overriddenRowHeight: mixer.trackWidth
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundPrimaryColor
    }

    Item {
        id: viewport

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        width: Math.max(contentWidth, mixer.width)
        x: -(mixer.trackListLayoutViewModel?.viewportOffset ?? 0)

        Repeater {
            id: repeater

            model: mixer.trackListViewModel?.count ?? 0

            delegate: MixerDelegate {
                required property int index

                anchors.bottom: parent.bottom
                anchors.top: parent.top
                width: mixer.trackWidth
                x: index * mixer.trackWidth

                selectionController: mixer.selectionController
                trackNumber: Qt.locale().toString(index + 1)
                trackViewModel: mixer.trackListViewModel.items[index]

                mouseArea: Item {

                    MouseArea {
                        id: trackMouseArea

                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton
                        hoverEnabled: true

                        onClicked: mouse => {
                            if (mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.Select) {
                                mixer.selectionController.selectByMouse(trackViewModel, mouse.button, mouse.modifiers);
                            }
                        }
                        onDoubleClicked: () => mixer.trackListInteractionController.itemDoubleClicked(mixer, index)
                        onEntered: () => mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.ItemBackground)
                        onExited: () => mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.ItemBackground)
                    }

                    GenericRightButtonMouseArea {
                        controller: mixer.trackListInteractionController
                        selectInteractionFlag: TrackListInteractionController.Select
                        paneItem: mixer
                        viewModel: trackViewModel
                        selectionController: mixer.selectionController
                    }
                }

                Rectangle {
                    id: leftBorder
                    anchors.horizontalCenter: parent.left
                    width: 1
                    height: parent.height
                    color: Theme.borderColor
                }

                Rectangle {
                    id: rightBorder
                    anchors.horizontalCenter: parent.right
                    width: 1
                    height: parent.height
                    color: Theme.borderColor
                }

                trackMSR.muteButton.visible: Boolean(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditMute)
                trackMSR.soloButton.visible: Boolean(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditSolo)
                trackMSR.recordButton.visible: Boolean(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditRecord)

                trackMSR.muteButton.onClicked: () => {
                    mixer.trackListInteractionController.muteEditingStarted(mixer, index)
                    trackViewModel.mute = trackMSR.muteButton.checked
                    mixer.trackListInteractionController.muteEditingFinished(mixer, index)
                }
                trackMSR.soloButton.onClicked: () => {
                    mixer.trackListInteractionController.soloEditingStarted(mixer, index)
                    trackViewModel.solo = trackMSR.soloButton.checked
                    mixer.trackListInteractionController.soloEditingFinished(mixer, index)
                }
                trackMSR.recordButton.onClicked: () => {
                    mixer.trackListInteractionController.recordEditingStarted(mixer, index)
                    trackViewModel.record = trackMSR.recordButton.checked
                    mixer.trackListInteractionController.recordEditingFinished(mixer, index)
                }

                trackMSR.muteButton.onHoveredChanged: () => {
                    if (trackMSR.muteButton.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.MuteButton)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.MuteButton)
                    }
                }
                trackMSR.soloButton.onHoveredChanged: () => {
                    if (trackMSR.soloButton.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.SoloButton)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.SoloButton)
                    }
                }
                trackMSR.recordButton.onHoveredChanged: () => {
                    if (trackMSR.recordButton.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.RecordButton)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.RecordButton)
                    }
                }

                trackNameEditLabel.readOnly: !(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditName)
                trackNameEditLabel.onEditingStarted: mixer.trackListInteractionController.nameEditingStarted(mixer, index)
                trackNameEditLabel.onEditingCommitted: text => {
                    trackViewModel.name = text
                    mixer.trackListInteractionController.nameEditingCommitted(mixer, index)
                }
                trackNameEditLabel.onEditingAborted: mixer.trackListInteractionController.nameEditingAborted(mixer, index)
                trackNameEditLabel.onHoveredChanged: () => {
                    if (trackNameEditLabel.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.NameLabel)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.NameLabel)
                    }
                }

                gainSlider.enabled: Boolean(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditGain)
                gainSlider.onPressedChanged: () => {
                    if (gainSlider.pressed) {
                        mixer.trackListInteractionController.gainEditingStarted(mixer, index)
                    } else {
                        mixer.trackListInteractionController.gainEditingCommitted(mixer, index)
                    }
                }
                gainSlider.onMoved: trackViewModel.gain = SVS.linearValueToDecibel(gainSlider.value + SVS.decibelToLinearValue(0))
                gainSlider.ThemedItem.onDoubleClickReset: () => {
                    mixer.trackListInteractionController.gainEditingStarted(mixer, index)
                    trackViewModel.gain = SVS.linearValueToDecibel(SVS.decibelToLinearValue(0))
                    mixer.trackListInteractionController.gainEditingCommitted(mixer, index)
                }
                gainSlider.onHoveredChanged: () => {
                    if (gainSlider.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.GainSlider)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.GainSlider)
                    }
                }

                gainEditLabel.readOnly: !(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditGain)
                gainEditLabel.onEditingStarted: mixer.trackListInteractionController.gainEditingStarted(mixer, index)
                gainEditLabel.onEditingCommitted: text => {
                    trackViewModel.gain = Number.fromLocaleString(Qt.locale(), text)
                    mixer.trackListInteractionController.gainEditingCommitted(mixer, index)
                }
                gainEditLabel.onEditingAborted: mixer.trackListInteractionController.gainEditingAborted(mixer, index)
                gainEditLabel.onHoveredChanged: () => {
                    if (gainEditLabel.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.GainLabel)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.GainLabel)
                    }
                }

                panDial.enabled: Boolean(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditPan)
                panDial.onPressedChanged: () => {
                    if (panDial.pressed) {
                        mixer.trackListInteractionController.panEditingStarted(mixer, index)
                    } else {
                        mixer.trackListInteractionController.panEditingCommitted(mixer, index)
                    }
                }
                panDial.onMoved: trackViewModel.pan = panDial.value
                panDial.ThemedItem.onDoubleClickReset: () => {
                    mixer.trackListInteractionController.panEditingStarted(mixer, index)
                    trackViewModel.pan = 0
                    mixer.trackListInteractionController.panEditingCommitted(mixer, index)
                }
                panDial.onHoveredChanged: () => {
                    if (panDial.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.PanDial)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.PanDial)
                    }
                }

                panEditLabel.readOnly: !(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditPan)
                panEditLabel.onEditingStarted: mixer.trackListInteractionController.panEditingStarted(mixer, index)
                panEditLabel.onEditingCommitted: text => {
                    trackViewModel.pan = Number.fromLocaleString(Qt.locale(), text) * 0.01
                    mixer.trackListInteractionController.panEditingCommitted(mixer, index)
                }
                panEditLabel.onEditingAborted: mixer.trackListInteractionController.panEditingAborted(mixer, index)
                panEditLabel.onHoveredChanged: () => {
                    if (panEditLabel.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.PanLabel)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.PanLabel)
                    }
                }

                levelMeterMouseArea.onClicked: () => {
                    mixer.trackListInteractionController.itemLevelMeterClicked(mixer, index)
                }
                levelMeterMouseArea.onEntered: () => mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.LevelMeter)
                levelMeterMouseArea.onExited: () => mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.LevelMeter)

                peakMouseArea.onClicked: () => {
                    mixer.trackListInteractionController.itemLevelMeterClicked(mixer, index)
                }
                peakMouseArea.onEntered: () => mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.LevelMeter)
                peakMouseArea.onExited: () => mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.LevelMeter)

                multiChannelOutputButton.visible: Boolean(mixer.trackListInteractionController.itemInteraction & TrackListInteractionController.EditMultiChannelOutput)
                multiChannelOutputButton.onClicked: () => {
                    mixer.trackListInteractionController.multiChannelOutputEditingStarted(mixer, index)
                    trackViewModel.multiChannelOutput = multiChannelOutputButton.checked
                    mixer.trackListInteractionController.multiChannelOutputEditingFinished(mixer, index)
                }
                multiChannelOutputButton.onHoveredChanged: () => {
                    if (multiChannelOutputButton.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.MultiChannelOutputButton)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.MultiChannelOutputButton)
                    }
                }

                colorIndicator.onClicked: () => mixer.trackListInteractionController.itemColorIndicatorClicked(mixer, index)
                colorIndicator.onHoveredChanged: () => {
                    if (colorIndicator.hovered) {
                        mixer.trackListInteractionController.itemHoverEntered(mixer, trackViewModel, TrackListInteractionController.ColorIndicator)
                    } else {
                        mixer.trackListInteractionController.itemHoverExited(mixer, trackViewModel, TrackListInteractionController.ColorIndicator)
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
        position: {
            const maxOffset = Math.max(0, contentWidth - trackListManipulator.viewSize)
            return maxOffset > 0 ? (mixer.trackListLayoutViewModel?.viewportOffset ?? 0) / maxOffset : 0
        }
        size: contentWidth > 0 ? Math.min(1, trackListManipulator.viewSize / contentWidth) : 1

        onPositionChanged: {
            if (!mixer.trackListLayoutViewModel)
                return;

            const maxOffset = Math.max(0, contentWidth - trackListManipulator.viewSize)
            const targetOffset = position * maxOffset
            if (Math.abs((mixer.trackListLayoutViewModel.viewportOffset ?? 0) - targetOffset) > Number.EPSILON * 1000) {
                mixer.trackListLayoutViewModel.viewportOffset = targetOffset
            }
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Horizontal
        viewModel: mixer.scrollBehaviorViewModel
        zoomableOrientation: 0

        onMoved: (deltaX, isPhysicalWheel) => mixer.moveViewBy(deltaX, isPhysicalWheel)
    }
}
