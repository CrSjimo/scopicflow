import QtQml
import QtQml.Models
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow

Item {
    id: trackListDelegate

    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property bool isCurrent: false
    property bool isLast: false
    property Component mouseArea: null
    property Component trackExtraDelegate: null
    property QtObject trackListViewModel: null
    required property int trackNumber
    required property QtObject trackViewModel
    property QtObject transactionControllerNotifier: null

    function emitInteractionNotificationSignal(interactionType, flags = 0) {
        trackListDelegate.interactionControllerNotifier?.itemInteracted(interactionType, trackListDelegate.trackViewModel, trackListDelegate.index, trackListDelegate.trackListViewModel, flags);
    }
    function fitHeight() {
        fitHeightAnimation.start();
    }
    function handleBeforeInteractionNotification(interactionType, flags = 0) {
        if (trackListDelegate.interactionControllerNotifier?.handleItemInteraction(interactionType, trackListDelegate.trackViewModel, trackListDelegate.index, trackListDelegate.trackListViewModel, flags))
            return false;
        return true;
    }
    function sendInteractionNotification(interactionType, flags = 0) {
        if (!handleBeforeInteractionNotification(interactionType, flags))
            return false;
        emitInteractionNotificationSignal(interactionType, flags);
        return true;
    }

    NumberAnimation on height {
        id: fitHeightAnimation

        duration: Theme.visualEffectAnimationDuration
        easing.type: Easing.OutCubic
        to: 80
    }

    onHeightChanged: () => {
        if (height < 80) {
            controlsSecondRow.opacity = 0;
        } else {
            controlsSecondRow.opacity = 1;
        }
    }
    onTrackViewModelChanged: () => {
        dummyModel.clear();
        if (trackListDelegate.trackViewModel)
            dummyModel.append({
                trackViewModel: trackListDelegate.trackViewModel
            });
    }

    Rectangle {
        anchors.bottomMargin: -1
        anchors.fill: parent
        anchors.leftMargin: -1
        anchors.rightMargin: -1
        border.color: Theme.borderColor
        border.width: 1
        clip: true
        color: trackListDelegate.trackViewModel.selected ? SFPalette.trackListSelectedColorChange.apply(SFPalette.trackListBackgroundColor) : SFPalette.trackListBackgroundColor

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }

        Item {
            LayoutMirroring.childrenInherit: true
            LayoutMirroring.enabled: false
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.top: parent.top
            width: 48

            Rectangle {
                id: selectionIndicator

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.margins: 8
                anchors.top: parent.top
                color: Theme.accentColor
                opacity: trackListDelegate.isCurrent ? 1 : 0
                radius: 1
                visible: opacity !== 0
                width: 2

                Behavior on opacity {
                    NumberAnimation {
                        duration: Theme.colorAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }
            }
            Text {
                id: trackNumberLabel

                anchors.left: selectionIndicator.right
                anchors.leftMargin: 4
                anchors.verticalCenter: parent.top
                anchors.verticalCenterOffset: 20
                color: trackListDelegate.isCurrent ? Theme.accentColor : SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                text: Qt.locale().toString(trackListDelegate.trackNumber)

                Behavior on color {
                    ColorAnimation {
                        duration: Theme.colorAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }
        Loader {
            anchors.fill: parent
            sourceComponent: trackListDelegate.mouseArea
        }
        Button {
            id: colorIndicator

            LayoutMirroring.enabled: false
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.top: parent.top
            width: 8

            background: Rectangle {
                color: trackListDelegate.trackViewModel.color
            }

            onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnColorIndicator)
            onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnColorIndicator)
            onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnColorIndicator)
            onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnColorIndicator)
            onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnColorIndicator)
            onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnColorIndicator)
        }
        Item {
            anchors.left: parent.left
            anchors.leftMargin: LayoutMirroring.enabled ? 24 : 48
            anchors.top: parent.top
            anchors.topMargin: 8

            Row {
                anchors.left: parent.left
                anchors.top: parent.top
                spacing: 8

                TrackMSR {
                    id: controlsFirstRow

                    trackViewModel: trackListDelegate.trackViewModel

                    muteButton.onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnMute)
                    muteButton.onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnMute)
                    muteButton.onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnMute)
                    muteButton.onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnMute)
                    muteButton.onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnMute)
                    muteButton.onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnMute)
                    recordButton.onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnRecord)
                    recordButton.onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnRecord)
                    recordButton.onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnRecord)
                    recordButton.onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnRecord)
                    recordButton.onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnRecord)
                    recordButton.onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnRecord)
                    soloButton.onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnSolo)
                    soloButton.onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnSolo)
                    soloButton.onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnSolo)
                    soloButton.onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnSolo)
                    soloButton.onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnSolo)
                    soloButton.onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnSolo)
                }
                TrackListEditLabel {
                    anchors.bottom: controlsFirstRow.bottom
                    anchors.top: controlsFirstRow.top
                    text: trackListDelegate.trackViewModel.name

                    onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnTrackName)
                    onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnTrackName)
                    onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnTrackName)
                    onEditingFinished: text => trackListDelegate.trackViewModel.name = text
                    onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnTrackName)
                    onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnTrackName)
                    onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnTrackName)
                }
            }
            Row {
                id: controlsSecondRow

                readonly property bool intermediate: gainSlider.pressed || panDial.pressed

                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: 40
                spacing: 0
                visible: opacity !== 0.0

                Behavior on opacity {
                    NumberAnimation {
                        duration: Theme.visualEffectAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }

                onIntermediateChanged: () => {
                    if (intermediate) {
                        trackListDelegate.transactionControllerNotifier?.transactionAboutToBegin();
                        trackListDelegate.trackViewModel.intermediate = true;
                    } else {
                        trackListDelegate.trackViewModel.intermediate = false;
                        trackListDelegate.transactionControllerNotifier?.transactionCommitted();
                    }
                }

                Row {
                    spacing: 4

                    FluentSystemIcon {
                        anchors.verticalCenter: gainSlider.verticalCenter
                        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                        font.pixelSize: 24
                        icon: 'cellular_data_1_20_regular'
                    }
                    Slider {
                        id: gainSlider

                        enabled: width > 40
                        from: SVS.decibelToLinearValue(-96) - SVS.decibelToLinearValue(0)
                        height: 24
                        opacity: enabled ? 1 : 0
                        to: SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(0)
                        value: SVS.decibelToLinearValue(trackListDelegate.trackViewModel.gain)
                        width: trackListDelegate.width - 256

                        Behavior on opacity {
                            NumberAnimation {
                                duration: Theme.visualEffectAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }

                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSlider)
                        onMoved: trackListDelegate.trackViewModel.gain = SVS.linearValueToDecibel(value + SVS.decibelToLinearValue(0))
                        onPressedChanged: trackListDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSlider)
                    }
                    TrackListEditLabel {
                        anchors.bottom: parent.bottom
                        anchors.top: parent.top
                        editText: Qt.locale().toString(trackListDelegate.trackViewModel.gain, "f", 1)
                        text: (trackListDelegate.trackViewModel.gain + 96 < 0.05 ? "-∞" : Qt.locale().toString(trackListDelegate.trackViewModel.gain, "f", 1)) + " dB"
                        width: 64

                        validator: DoubleValidator {
                            // TODO use svscraft expression validator instead
                            bottom: -96
                            decimals: 1
                            top: 6
                        }

                        onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnGainSpinBox)
                        onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnGainSpinBox)
                        onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnGainSpinBox)
                        onEditingFinished: text => trackListDelegate.trackViewModel.gain = Number.fromLocaleString(Qt.locale(), text)
                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSpinBox)
                        onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnGainSpinBox)
                        onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSpinBox)
                    }
                }
                Row {
                    spacing: 8

                    FluentSystemIcon {
                        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                        font.pixelSize: 24
                        height: 24
                        icon: 'live_20_regular'
                    }
                    Dial {
                        id: panDial

                        LayoutMirroring.enabled: false
                        from: -1.0
                        height: 24
                        to: 1.0
                        value: trackListDelegate.trackViewModel.pan
                        width: 24

                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanDial)
                        onMoved: trackListDelegate.trackViewModel.pan = value
                        onPressedChanged: trackListDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnPanDial)
                    }
                    TrackListEditLabel {
                        anchors.bottom: parent.bottom
                        anchors.top: parent.top
                        text: Qt.locale().toString(Math.round(trackListDelegate.trackViewModel.pan * 100))
                        width: 64

                        validator: IntValidator {
                            // TODO use svscraft expression validator instead
                            bottom: -100
                            top: 100
                        }

                        onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnPanSpinBox)
                        onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnPanSpinBox)
                        onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnPanSpinBox)
                        onEditingFinished: text => trackListDelegate.trackViewModel.pan = Number.fromLocaleString(Qt.locale(), text) * 0.01
                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanSpinBox)
                        onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnPanSpinBox)
                        onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnPanSpinBox)
                    }
                }
            }
        }
        Item {
            height: Math.max(parent.height - 88, 0)
            width: parent.width - 72
            x: 48
            y: 80

            ListModel {
                id: dummyModel

            }
            Repeater {
                anchors.fill: parent
                delegate: trackListDelegate.trackExtraDelegate
                model: dummyModel
            }
        }
        Rectangle {
            LayoutMirroring.childrenInherit: true
            LayoutMirroring.enabled: false
            anchors.bottom: parent.bottom
            anchors.margins: 1
            anchors.right: parent.right
            anchors.top: parent.top
            color: SFPalette.levelMeterColor
            width: 12

            LevelMeter {
                id: leftChannelLevelMeter

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.horizontalCenter
                anchors.top: parent.top
                value: trackListDelegate.trackViewModel.leftLevel
            }
            LevelMeter {
                id: rightChannelLevelMeter

                anchors.bottom: parent.bottom
                anchors.left: parent.horizontalCenter
                anchors.right: parent.right
                anchors.top: parent.top
                value: trackListDelegate.trackViewModel.rightLevel
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true

                onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnLevelMeter)
                onClicked: () => {
                    leftChannelLevelMeter.clipping = false;
                    rightChannelLevelMeter.clipping = false;
                    trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnLevelMeter);
                }
                onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnLevelMeter)
                onEntered: trackListDelegate.sendInteractionNotification(ScopicFlow.II_HoverEntered, ScopicFlow.InteractionOnLevelMeter)
                onExited: trackListDelegate.sendInteractionNotification(ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnLevelMeter)
                onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnLevelMeter)
                onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnLevelMeter)
            }
        }
    }
}
