import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import SVSCraft
import SVSCraft.UIComponents
import SVSCraft.UIComponents.impl

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow

Item {
    id: mixerDelegate

    property QtObject animationViewModel: null
    required property int index
    property QtObject interactionControllerNotifier: null
    property bool isCurrent: false
    property Component mouseArea: null
    property QtObject trackListViewModel: null
    required property int trackNumber
    required property QtObject trackViewModel
    property QtObject transactionControllerNotifier: null

    function emitInteractionNotificationSignal(interactionType, flags = 0) {
        mixerDelegate.interactionControllerNotifier?.itemInteracted(interactionType, mixerDelegate.trackViewModel, mixerDelegate.index, mixerDelegate.trackListViewModel, flags);
    }
    function handleBeforeInteractionNotification(interactionType, flags = 0) {
        if (mixerDelegate.interactionControllerNotifier?.handleItemInteraction(interactionType, mixerDelegate.trackViewModel, mixerDelegate.index, mixerDelegate.trackListViewModel, flags))
            return false;
        return true;
    }
    function sendInteractionNotification(interactionType, flags = 0) {
        if (!handleBeforeInteractionNotification(interactionType, flags))
            return false;
        emitInteractionNotificationSignal(interactionType, flags);
        return true;
    }

    width: 128

    Rectangle {
        anchors.fill: parent
        color: mixerDelegate.trackViewModel.selected ? SFPalette.trackListSelectedColorChange.apply(SFPalette.trackListBackgroundColor) : SFPalette.trackListBackgroundColor

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
    }
    Loader {
        anchors.fill: parent
        enabled: !mixerDelegate.trackViewModel.bus
        sourceComponent: mixerDelegate.mouseArea
    }
    ColumnLayout {
        readonly property bool intermediate: gainSlider.pressed || panDial.pressed

        anchors.fill: parent
        spacing: 0

        onIntermediateChanged: () => {
            if (intermediate) {
                mixerDelegate.transactionControllerNotifier?.transactionAboutToBegin();
                mixerDelegate.trackViewModel.intermediate = true;
            } else {
                mixerDelegate.trackViewModel.intermediate = false;
                mixerDelegate.transactionControllerNotifier?.transactionCommitted();
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: routeButton.visible ? 28 : 42
            Layout.rightMargin: 8
            Layout.topMargin: 8

            Dial {
                id: panDial

                LayoutMirroring.enabled: false
                from: -1.0
                to: 1.0
                value: mixerDelegate.trackViewModel.pan

                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanDial)
                onMoved: {
                    mixerDelegate.trackViewModel.pan = value;
                }
                onPressedChanged: mixerDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnPanDial)
            }
            TrackListEditLabel {
                height: 24
                text: Qt.locale().toString(Math.round(mixerDelegate.trackViewModel.pan * 100))
                width: 24

                validator: IntValidator {
                    // TODO use svscraft expression validator instead
                    bottom: -100
                    top: 100
                }

                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnPanSpinBox)
                onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnPanSpinBox)
                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnPanSpinBox)
                onEditingFinished: text => mixerDelegate.trackViewModel.pan = Number.fromLocaleString(Qt.locale(), text) * 0.01
                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanSpinBox)
                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnPanSpinBox)
                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnPanSpinBox)
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                id: routeButton

                Theme.accentColor: SFPalette.routeColor
                ThemedItem.controlType: SVS.CT_Accent
                checkable: true
                checked: mixerDelegate.trackViewModel.route ?? false
                flat: false
                text: 'R'
                visible: typeof (mixerDelegate.trackViewModel.route) === "boolean"

                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnRoute)
                onCheckedChanged: mixerDelegate.trackViewModel.route = checked
                onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnRoute)
                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnRoute)
                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnRoute)
                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnRoute)
                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnRoute)
            }
        }
        Rectangle {
            Layout.bottomMargin: 8
            Layout.fillWidth: true
            Layout.topMargin: 8
            color: Theme.splitterColor
            height: 1
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            TrackMSR {
                id: msr

                anchors.horizontalCenter: parent.horizontalCenter
                trackViewModel: mixerDelegate.trackViewModel

                muteButton.onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnMute)
                muteButton.onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnMute)
                muteButton.onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnMute)
                muteButton.onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnMute)
                muteButton.onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnMute)
                muteButton.onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnMute)
                recordButton.onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnRecord)
                recordButton.onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnRecord)
                recordButton.onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnRecord)
                recordButton.onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnRecord)
                recordButton.onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnRecord)
                recordButton.onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnRecord)
                soloButton.onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnSolo)
                soloButton.onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnSolo)
                soloButton.onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnSolo)
                soloButton.onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnSolo)
                soloButton.onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnSolo)
                soloButton.onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnSolo)
            }
            RowLayout {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: msr.bottom

                ColumnLayout {
                    Layout.fillHeight: true

                    MixerSlider {
                        id: gainSlider

                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillHeight: true
                        value: SVS.decibelToLinearValue(mixerDelegate.trackViewModel.gain)

                        onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSlider)
                        onMoved: mixerDelegate.trackViewModel.gain = SVS.linearValueToDecibel(value + SVS.decibelToLinearValue(0))
                        onPressedChanged: mixerDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSlider)
                    }
                    TrackListEditLabel {
                        center: true
                        editText: Qt.locale().toString(mixerDelegate.trackViewModel.gain, "f", 1)
                        height: 24
                        text: (mixerDelegate.trackViewModel.gain + 96 < 0.05 ? "-∞" : Qt.locale().toString(mixerDelegate.trackViewModel.gain, "f", 1)) + " dB"
                        width: 48

                        validator: DoubleValidator {
                            // TODO use svscraft expression validator instead
                            bottom: -96
                            decimals: 1
                            top: 6
                        }

                        onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnGainSpinBox)
                        onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnGainSpinBox)
                        onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnGainSpinBox)
                        onEditingFinished: text => mixerDelegate.trackViewModel.gain = Number.fromLocaleString(Qt.locale(), text)
                        onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSpinBox)
                        onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnGainSpinBox)
                        onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSpinBox)
                    }
                }
                ColumnLayout {
                    Layout.fillHeight: true
                    Layout.topMargin: 8
                    spacing: 14
                    width: 12

                    Rectangle {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: SFPalette.levelMeterColor

                        LevelMeter {
                            id: leftChannelLevelMeter

                            LayoutMirroring.enabled: false
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.horizontalCenter
                            anchors.top: parent.top
                            value: mixerDelegate.trackViewModel.leftLevel
                        }
                        LevelMeter {
                            id: rightChannelLevelMeter

                            LayoutMirroring.enabled: false
                            anchors.bottom: parent.bottom
                            anchors.left: parent.horizontalCenter
                            anchors.right: parent.right
                            anchors.top: parent.top
                            value: mixerDelegate.trackViewModel.rightLevel
                        }
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true

                            onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnLevelMeter)
                            onClicked: () => {
                                leftChannelLevelMeter.clipping = false;
                                rightChannelLevelMeter.clipping = false;
                                peakText.maxValue = -96;
                                mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnLevelMeter);
                            }
                            onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnLevelMeter)
                            onEntered: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverEntered, ScopicFlow.InteractionOnLevelMeter)
                            onExited: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnLevelMeter)
                            onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnLevelMeter)
                            onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnLevelMeter)
                        }
                    }
                    Item {
                        Layout.bottomMargin: 4
                        Layout.fillWidth: true
                        height: peakText.height

                        Text {
                            id: peakText

                            property double maxValue: -96
                            property double value: Math.max(mixerDelegate.trackViewModel.leftLevel, mixerDelegate.trackViewModel.rightLevel)

                            anchors.left: parent.left
                            color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                            horizontalAlignment: Text.AlignHCenter
                            opacity: 0.5
                            text: (Math.abs(maxValue + 96) < 0.05 ? '' : Qt.locale().toString(maxValue, "f", 1))

                            onValueChanged: maxValue = Math.max(maxValue, value)

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true

                                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnPeakLabel)
                                onClicked: () => {
                                    leftChannelLevelMeter.clipping = false;
                                    rightChannelLevelMeter.clipping = false;
                                    peakText.maxValue = -96;
                                    mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnPeakLabel);
                                }
                                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnPeakLabel)
                                onEntered: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverEntered, ScopicFlow.InteractionOnLevelMeter)
                                onExited: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnLevelMeter)
                                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnPeakLabel)
                                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnPeakLabel)
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.topMargin: 8
            color: Theme.splitterColor
            height: 1
        }
        ColumnLayout {
            Layout.bottomMargin: 4
            Layout.fillWidth: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            spacing: 0

            TrackListEditLabel {
                Layout.fillWidth: true
                center: true
                enabled: !mixerDelegate.trackViewModel.bus
                height: 24
                text: mixerDelegate.trackViewModel.name

                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnTrackName)
                onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnTrackName)
                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnTrackName)
                onEditingFinished: text => mixerDelegate.trackViewModel.name = text
                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnTrackName)
                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnTrackName)
                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnTrackName)
            }
            RowLayout {
                property double _opacity: mixerDelegate.isCurrent ? 1 : 0

                Layout.fillWidth: true
                Layout.leftMargin: 8
                Layout.rightMargin: 8
                spacing: 8

                Behavior on _opacity {
                    NumberAnimation {
                        duration: Theme.colorAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }

                Rectangle {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.horizontalStretchFactor: 1
                    color: Theme.accentColor
                    height: 2
                    opacity: parent._opacity
                    radius: 1
                }
                Text {
                    id: trackNumberLabel

                    Layout.alignment: Qt.AlignVCenter
                    color: mixerDelegate.isCurrent ? Theme.accentColor : SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                    opacity: mixerDelegate.isCurrent ? 1 : 0.5
                    text: Qt.locale().toString(mixerDelegate.trackNumber)

                    Behavior on color {
                        ColorAnimation {
                            duration: Theme.colorAnimationDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                }
                Rectangle {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                    Layout.horizontalStretchFactor: 1
                    color: Theme.accentColor
                    height: 2
                    opacity: parent._opacity
                    radius: 1
                }
            }
        }
        Button {
            Layout.fillWidth: true
            height: 8

            background: Rectangle {
                color: mixerDelegate.trackViewModel.color
            }

            onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnColorIndicator)
            onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnColorIndicator)
            onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnColorIndicator)
            onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnColorIndicator)
            onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnColorIndicator)
            onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnColorIndicator)
        }
    }

    component MixerSlider: T.Slider {
        id: control

        bottomPadding: 4
        from: SVS.decibelToLinearValue(-96) - SVS.decibelToLinearValue(0)
        implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitHandleHeight + topPadding + bottomPadding)
        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitHandleWidth + leftPadding + rightPadding)
        orientation: Qt.Vertical
        padding: 0
        to: SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(0)
        topPadding: 8

        background: Rectangle {
            color: Theme.splitterColor
            height: control.availableHeight
            radius: 1
            width: 2
            x: control.leftPadding + (control.availableWidth - width) / 2
            y: control.topPadding

            Item {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 16
                anchors.right: parent.left
                anchors.rightMargin: 14
                anchors.top: parent.top
                anchors.topMargin: 16

                Repeater {
                    model: [6, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -8, -10, -12, -15, -18, -21, -24, -30, -36, -48, -96]

                    delegate: Rectangle {
                        readonly property bool major: [6, 0, -6, -12, -24, -96].indexOf(modelData) !== -1
                        required property int modelData
                        readonly property double position: (SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(modelData)) / (SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(-96))

                        anchors.right: parent.right
                        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                        height: 1
                        opacity: major ? 1 : 0.5
                        width: 4
                        y: position * (control.availableHeight - 32)

                        Text {
                            anchors.right: parent.left
                            anchors.rightMargin: 2
                            anchors.verticalCenter: parent.verticalCenter
                            color: parent.color
                            font.pixelSize: 8
                            text: parent.modelData === -96 ? "-∞" : parent.modelData
                            visible: parent.major
                        }
                    }
                }
            }
        }
        handle: SliderHandle {
            border.color: Theme.borderColor
            border.width: 1
            color: !control.enabled ? Theme.controlDisabledColorChange.apply(Theme.buttonColor) : control.pressed ? Theme.controlPressedColorChange.apply(Theme.buttonColor) : control.hovered ? Theme.controlHoveredColorChange.apply(Theme.buttonColor) : Theme.buttonColor
            controlItem: control
            height: 32
            implicitHeight: 32
            implicitWidth: 24
            radius: 4
            visualPosition: control.visualPosition
            width: 24

            Rectangle {
                anchors.centerIn: parent
                color: Theme.foregroundPrimaryColor
                height: 2
                radius: 2
                width: 16
            }
        }

        TapHandler {
            onDoubleTapped: () => {
                if (!Theme.doubleClickResetEnabled)
                    return;
                if (from > 0 && to > 0 || from < 0 && to < 0)
                    return;
                GlobalHelper.ungrabMouse(control);
                control.handle._doubleClicked = control.handle._doubleClickTriggered = true;
                GlobalHelper.setProperty(control, "value", 0);
                control.handle._doubleClickTriggered = false;
            }
        }
    }
}
