import QtQml
import QtQml.Models
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {

    id: trackListDelegate

    required property int trackNumber
    required property QtObject trackViewModel

    onTrackViewModelChanged: () => {
        dummyModel.clear()
        if (trackListDelegate.trackViewModel)
            dummyModel.append({trackViewModel: trackListDelegate.trackViewModel})
    }

    property bool isCurrent: false
    property bool isLast: false

    property QtObject trackListViewModel: null
    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject transactionControllerNotifier: null
    property Component mouseArea: null
    property Component trackExtraDelegate: null

    NumberAnimation on height {
        id: fitHeightAnimation
        to: 80
        easing.type: Easing.OutCubic
        duration: Theme.visualEffectAnimationDuration
    }

    function fitHeight() {
        fitHeightAnimation.start()
    }

    function sendInteractionNotification(interactionType, flags = 0) {
        if (!handleBeforeInteractionNotification(interactionType, flags))
            return false
        emitInteractionNotificationSignal(interactionType, flags)
        return true
    }
    function handleBeforeInteractionNotification(interactionType, flags = 0) {
        if (trackListDelegate.interactionControllerNotifier?.handleItemInteraction(interactionType, trackListDelegate.trackViewModel, trackListDelegate.index, trackListDelegate.trackListViewModel, flags))
            return false
        return true
    }
    function emitInteractionNotificationSignal(interactionType, flags = 0) {
        trackListDelegate.interactionControllerNotifier?.itemInteracted(interactionType, trackListDelegate.trackViewModel, trackListDelegate.index, trackListDelegate.trackListViewModel, flags)
    }

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: -1
        anchors.rightMargin: -1
        anchors.bottomMargin: -1
        color: trackListDelegate.trackViewModel.selected ? SFPalette.trackListSelectedColorChange.apply(SFPalette.trackListBackgroundColor) : SFPalette.trackListBackgroundColor
        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
        border.width: 1
        border.color: Theme.borderColor

        clip: true

        Item {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            LayoutMirroring.enabled: false
            LayoutMirroring.childrenInherit: true
            width: 48

            Rectangle {
                id: selectionIndicator
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 8
                anchors.leftMargin: 16
                width: 2
                radius: 1
                color: Theme.accentColor
                visible: opacity !== 0
                opacity: trackListDelegate.isCurrent ? 1 : 0
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
                text: Qt.locale().toString(trackListDelegate.trackNumber)
                color: trackListDelegate.isCurrent ? Theme.accentColor : SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                Behavior on color {
                    ColorAnimation {
                        duration: Theme.colorAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }
            }
        }

        Loader {
            sourceComponent: trackListDelegate.mouseArea
            anchors.fill: parent
        }

        Button {
            id: colorIndicator
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            LayoutMirroring.enabled: false
            width: 8

            background: Rectangle {
                color: trackListDelegate.trackViewModel.color
            }

            onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnColorIndicator)
            onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnColorIndicator)
            onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnColorIndicator)
            onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnColorIndicator)
            onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnColorIndicator)
            onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnColorIndicator)
        }

        Item {
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.left: parent.left
            anchors.leftMargin: LayoutMirroring.enabled ? 24 : 48
            Row {
                anchors.top: parent.top
                anchors.left: parent.left
                spacing: 8
                TrackMSR {
                    id: controlsFirstRow
                    trackViewModel: trackListDelegate.trackViewModel

                    muteButton.onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnMute)
                    muteButton.onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnMute)
                    muteButton.onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnMute)
                    muteButton.onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnMute)
                    muteButton.onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnMute)
                    muteButton.onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnMute)

                    soloButton.onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnSolo)
                    soloButton.onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnSolo)
                    soloButton.onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnSolo)
                    soloButton.onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnSolo)
                    soloButton.onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnSolo)
                    soloButton.onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnSolo)

                    recordButton.onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnRecord)
                    recordButton.onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnRecord)
                    recordButton.onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnRecord)
                    recordButton.onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnRecord)
                    recordButton.onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnRecord)
                    recordButton.onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnRecord)
                }
                TrackListEditLabel {
                    anchors.top: controlsFirstRow.top
                    anchors.bottom: controlsFirstRow.bottom
                    text: trackListDelegate.trackViewModel.name
                    onEditingFinished: (text) => trackListDelegate.trackViewModel.name = text
                    onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnTrackName)
                    onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnTrackName)
                    onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnTrackName)
                    onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnTrackName)
                    onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnTrackName)
                    onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnTrackName)
                }

            }

            Row {
                anchors.top: parent.top
                anchors.topMargin: 40
                anchors.left: parent.left
                id: controlsSecondRow
                spacing: 0
                visible: opacity !== 0.0
                Behavior on opacity {
                    NumberAnimation {
                        duration: Theme.visualEffectAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }
                readonly property bool intermediate: gainSlider.pressed || panDial.pressed
                onIntermediateChanged: () => {
                    if (intermediate) {
                        trackListDelegate.transactionControllerNotifier?.transactionAboutToBegin()
                        trackListDelegate.trackViewModel.intermediate = true
                    } else {
                        trackListDelegate.trackViewModel.intermediate = false
                        trackListDelegate.transactionControllerNotifier?.transactionCommitted()
                    }

                }
                Row {
                    spacing: 4
                    FluentSystemIcon {
                        anchors.verticalCenter: gainSlider.verticalCenter
                        icon: 'cellular_data_1_20_regular'
                        font.pixelSize: 24
                        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                    }
                    Slider {
                        id: gainSlider
                        height: 24
                        width: trackListDelegate.width - 256
                        enabled: width > 40
                        opacity: enabled ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation {
                                duration: Theme.visualEffectAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                        from: SVS.decibelToLinearValue(-96) - SVS.decibelToLinearValue(0)
                        to: SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(0)
                        value: SVS.decibelToLinearValue(trackListDelegate.trackViewModel.gain)
                        onMoved: trackListDelegate.trackViewModel.gain = SVS.linearValueToDecibel(value + SVS.decibelToLinearValue(0))
                        onPressedChanged: trackListDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSlider)
                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSlider)
                    }
                    TrackListEditLabel {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 64
                        text: (trackListDelegate.trackViewModel.gain + 96 < 0.05 ? "-∞" : Qt.locale().toString(trackListDelegate.trackViewModel.gain, "f", 1)) + " dB"
                        editText: Qt.locale().toString(trackListDelegate.trackViewModel.gain, "f", 1)
                        validator: DoubleValidator { // TODO use svscraft expression validator instead
                            bottom: -96
                            top: 6
                            decimals: 1
                        }
                        onEditingFinished: (text) => trackListDelegate.trackViewModel.gain = Number.fromLocaleString(Qt.locale(), text)
                        onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnGainSpinBox)
                        onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSpinBox)
                        onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnGainSpinBox)
                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSpinBox)
                        onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnGainSpinBox)
                        onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnGainSpinBox)
                    }
                }
                Row {
                    spacing: 8
                    FluentSystemIcon {
                        height: 24
                        icon: 'live_20_regular'
                        font.pixelSize: 24
                        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                    }
                    Dial {
                        id: panDial
                        LayoutMirroring.enabled: false
                        height: 24
                        width: 24
                        from: -1.0
                        to: 1.0
                        value: trackListDelegate.trackViewModel.pan
                        onMoved: trackListDelegate.trackViewModel.pan = value
                        onPressedChanged: trackListDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnPanDial)
                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanDial)
                    }
                    TrackListEditLabel {
                        anchors.top: parent.top
                        anchors.bottom: parent.bottom
                        width: 64
                        text: Qt.locale().toString(Math.round(trackListDelegate.trackViewModel.pan * 100))
                        validator: IntValidator { // TODO use svscraft expression validator instead
                            bottom: -100
                            top: 100
                        }
                        onEditingFinished: (text) => trackListDelegate.trackViewModel.pan = Number.fromLocaleString(Qt.locale(), text) * 0.01
                        onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnPanSpinBox)
                        onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnPanSpinBox)
                        onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnPanSpinBox)
                        onHoveredChanged: trackListDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanSpinBox)
                        onClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnPanSpinBox)
                        onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnPanSpinBox)
                    }
                }
            }
        }

        Item {
            x: 48
            y: 80
            width: parent.width - 72
            height: Math.max(parent.height - 88, 0)
            ListModel {
                id: dummyModel
            }
            Repeater {
                anchors.fill: parent
                model: dummyModel
                delegate: trackListDelegate.trackExtraDelegate
            }
        }

        Rectangle {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 1
            LayoutMirroring.enabled: false
            LayoutMirroring.childrenInherit: true
            color: SFPalette.levelMeterColor
            width: 12
            LevelMeter {
                id: leftChannelLevelMeter
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.horizontalCenter
                value: trackListDelegate.trackViewModel.leftLevel
            }
            LevelMeter {
                id: rightChannelLevelMeter
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.horizontalCenter
                anchors.right: parent.right
                value: trackListDelegate.trackViewModel.rightLevel
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onPressed: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnLevelMeter)
                onReleased: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnLevelMeter)
                onCanceled: trackListDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnLevelMeter)
                onEntered: trackListDelegate.sendInteractionNotification(ScopicFlow.II_HoverEntered, ScopicFlow.InteractionOnLevelMeter)
                onExited: trackListDelegate.sendInteractionNotification(ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnLevelMeter)
                onClicked: () => {
                    leftChannelLevelMeter.clipping = false
                    rightChannelLevelMeter.clipping = false
                    trackListDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnLevelMeter)
                }
                onDoubleClicked: trackListDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnLevelMeter)
            }
        }

    }

    onHeightChanged: () => {
        if (height < 80) {
            controlsSecondRow.opacity = 0
        } else {
            controlsSecondRow.opacity = 1
        }
    }

}