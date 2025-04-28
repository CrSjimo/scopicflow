import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import SVSCraft
import SVSCraft.UIComponents
import SVSCraft.UIComponents.impl

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: mixerDelegate
    width: 128
    required property QtObject trackViewModel
    required property int index
    required property int trackNumber
    property bool isCurrent: false
    property QtObject trackListViewModel: null
    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject transactionControllerNotifier: null
    property Component mouseArea: null

    component MixerSlider: T.Slider {
        id: control
        implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset, implicitHandleWidth + leftPadding + rightPadding)
        implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset, implicitHandleHeight + topPadding + bottomPadding)
        orientation: Qt.Vertical
        from: SVS.decibelToLinearValue(-96) - SVS.decibelToLinearValue(0)
        to: SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(0)

        padding: 0
        topPadding: 8
        bottomPadding: 4

        handle: SliderHandle {
            controlItem: control
            visualPosition: control.visualPosition
            implicitWidth: 24
            implicitHeight: 32
            width: 24
            height: 32
            radius: 4
            color: !control.enabled ? Theme.controlDisabledColorChange.apply(Theme.buttonColor) :
                   control.pressed ? Theme.controlPressedColorChange.apply(Theme.buttonColor) :
                   control.hovered ? Theme.controlHoveredColorChange.apply(Theme.buttonColor) :
                   Theme.buttonColor
            border.width: 1
            border.color: Theme.borderColor
            Rectangle {
                height: 2
                width: 16
                radius: 2
                anchors.centerIn: parent
                color: Theme.foregroundPrimaryColor
            }
        }

        background: Rectangle {
            x: control.leftPadding + (control.availableWidth - width) / 2
            y: control.topPadding
            width: 2
            height: control.availableHeight
            radius: 1
            color: Theme.splitterColor
            Item {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 16
                anchors.bottomMargin: 16
                anchors.right: parent.left
                anchors.rightMargin: 14
                Repeater {
                    model: [6, 5, 4, 3, 2, 1, 0, -1, -2, -3, -4, -5, -6, -8, -10, -12, -15, -18, -21, -24, -30, -36, -48, -96]
                    delegate: Rectangle {
                        required property int modelData
                        readonly property double position: (SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(modelData)) / (SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(-96))
                        readonly property bool major: [6, 0, -6, -12, -24, -96].indexOf(modelData) !== -1
                        width: 4
                        height: 1
                        anchors.right: parent.right
                        y: position * (control.availableHeight - 32)
                        color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                        opacity: major ? 1 : 0.5
                        Text {
                            visible: parent.major
                            anchors.right: parent.left
                            anchors.rightMargin: 2
                            anchors.verticalCenter: parent.verticalCenter
                            font.pixelSize: 8
                            color: parent.color
                            text: parent.modelData === -96 ? "-∞" : parent.modelData
                        }
                    }
                }
            }
        }

        TapHandler {
            onDoubleTapped: () => {
                if (!Theme.doubleClickResetEnabled)
                    return
                if (from > 0 && to > 0 || from < 0 && to < 0)
                    return
                GlobalHelper.ungrabMouse(control)
                control.handle._doubleClicked = control.handle._doubleClickTriggered = true
                GlobalHelper.setProperty(control, "value", 0)
                control.handle._doubleClickTriggered = false
            }
        }
    }

    function sendInteractionNotification(interactionType, flags = 0) {
        if (!handleBeforeInteractionNotification(interactionType, flags))
            return false
        emitInteractionNotificationSignal(interactionType, flags)
        return true
    }
    function handleBeforeInteractionNotification(interactionType, flags = 0) {
        if (mixerDelegate.interactionControllerNotifier?.handleItemInteraction(interactionType, mixerDelegate.trackViewModel, mixerDelegate.index, mixerDelegate.trackListViewModel, flags))
            return false
        return true
    }
    function emitInteractionNotificationSignal(interactionType, flags = 0) {
        mixerDelegate.interactionControllerNotifier?.itemInteracted(interactionType, mixerDelegate.trackViewModel, mixerDelegate.index, mixerDelegate.trackListViewModel, flags)
    }

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
        sourceComponent: mixerDelegate.mouseArea
        anchors.fill: parent
        enabled: !mixerDelegate.trackViewModel.bus
    }
    ColumnLayout {
        spacing: 0
        anchors.fill: parent
        readonly property bool intermediate: gainSlider.pressed || panDial.pressed
        onIntermediateChanged: () => {
            if (intermediate) {
                mixerDelegate.transactionControllerNotifier?.transactionAboutToBegin()
                mixerDelegate.trackViewModel.intermediate = true
            } else {
                mixerDelegate.trackViewModel.intermediate = false
                mixerDelegate.transactionControllerNotifier?.transactionCommitted()
            }
        }
        RowLayout {
            Layout.leftMargin: routeButton.visible ? 28 : 42
            Layout.rightMargin: 8
            Layout.topMargin: 8
            Layout.fillWidth: true
            Dial {
                id: panDial
                LayoutMirroring.enabled: false
                from: -1.0
                to: 1.0
                value: mixerDelegate.trackViewModel.pan
                onMoved: {
                    mixerDelegate.trackViewModel.pan = value
                }
                onPressedChanged: mixerDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnPanDial)
                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanDial)
            }
            TrackListEditLabel {
                width: 24
                height: 24
                text: Qt.locale().toString(Math.round(mixerDelegate.trackViewModel.pan * 100))
                validator: IntValidator { // TODO use svscraft expression validator instead
                    bottom: -100
                    top: 100
                }
                onEditingFinished: (text) => mixerDelegate.trackViewModel.pan = Number.fromLocaleString(Qt.locale(), text) * 0.01
                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnPanSpinBox)
                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnPanSpinBox)
                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnPanSpinBox)
                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnPanSpinBox)
                onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnPanSpinBox)
                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnPanSpinBox)
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                id: routeButton
                Theme.accentColor: SFPalette.routeColor
                ThemedItem.controlType: SVS.CT_Accent
                checkable: true
                flat: false
                text: 'R'
                visible: typeof(mixerDelegate.trackViewModel.route) === "boolean"
                checked: mixerDelegate.trackViewModel.route ?? false
                onCheckedChanged: mixerDelegate.trackViewModel.route = checked

                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnRoute)
                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnRoute)
                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnRoute)
                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnRoute)
                onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnRoute)
                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnRoute)

            }
        }
        Rectangle {
            Layout.topMargin: 8
            Layout.bottomMargin: 8
            Layout.fillWidth: true
            height: 1
            color: Theme.splitterColor
        }
        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            TrackMSR {
                id: msr
                trackViewModel: mixerDelegate.trackViewModel
                anchors.horizontalCenter: parent.horizontalCenter

                muteButton.onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnMute)
                muteButton.onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnMute)
                muteButton.onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnMute)
                muteButton.onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnMute)
                muteButton.onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnMute)
                muteButton.onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnMute)

                soloButton.onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnSolo)
                soloButton.onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnSolo)
                soloButton.onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnSolo)
                soloButton.onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnSolo)
                soloButton.onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnSolo)
                soloButton.onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnSolo)

                recordButton.onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnRecord)
                recordButton.onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnRecord)
                recordButton.onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnRecord)
                recordButton.onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnRecord)
                recordButton.onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnRecord)
                recordButton.onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnRecord)

            }
            RowLayout {
                anchors.top: msr.bottom
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                ColumnLayout {
                    Layout.fillHeight: true
                    MixerSlider {
                        id: gainSlider
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillHeight: true
                        value: SVS.decibelToLinearValue(mixerDelegate.trackViewModel.gain)
                        onMoved: mixerDelegate.trackViewModel.gain = SVS.linearValueToDecibel(value + SVS.decibelToLinearValue(0))
                        onPressedChanged: mixerDelegate.sendInteractionNotification(pressed ? ScopicFlow.II_Pressed : ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSlider)
                        onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSlider)
                    }
                    TrackListEditLabel {
                        width: 48
                        height: 24
                        text: (mixerDelegate.trackViewModel.gain + 96 < 0.05 ? "-∞" : Qt.locale().toString(mixerDelegate.trackViewModel.gain, "f", 1)) + " dB"
                        editText: Qt.locale().toString(mixerDelegate.trackViewModel.gain, "f", 1)
                        center: true
                        validator: DoubleValidator { // TODO use svscraft expression validator instead
                            bottom: -96
                            top: 6
                            decimals: 1
                        }
                        onEditingFinished: (text) => mixerDelegate.trackViewModel.gain = Number.fromLocaleString(Qt.locale(), text)
                        onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnGainSpinBox)
                        onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnGainSpinBox)
                        onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnGainSpinBox)
                        onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnGainSpinBox)
                        onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnGainSpinBox)
                        onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnGainSpinBox)
                    }
                }
                ColumnLayout {
                    Layout.topMargin: 8
                    Layout.fillHeight: true
                    spacing: 14
                    width: 12
                    Rectangle {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        color: SFPalette.levelMeterColor
                        LevelMeter {
                            id: leftChannelLevelMeter
                            LayoutMirroring.enabled: false
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.horizontalCenter
                            value: mixerDelegate.trackViewModel.leftLevel
                        }
                        LevelMeter {
                            id: rightChannelLevelMeter
                            LayoutMirroring.enabled: false
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: parent.horizontalCenter
                            anchors.right: parent.right
                            value: mixerDelegate.trackViewModel.rightLevel
                        }
                        MouseArea {
                            anchors.fill: parent
                            hoverEnabled: true
                            onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnLevelMeter)
                            onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnLevelMeter)
                            onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnLevelMeter)
                            onEntered: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverEntered, ScopicFlow.InteractionOnLevelMeter)
                            onExited: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnLevelMeter)
                            onClicked: () => {
                                leftChannelLevelMeter.clipping = false
                                rightChannelLevelMeter.clipping = false
                                peakText.maxValue = -96
                                mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnLevelMeter)
                            }
                            onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnLevelMeter)
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                        Layout.bottomMargin: 4
                        height: peakText.height
                        Text {
                            id: peakText
                            anchors.left: parent.left
                            property double value: Math.max(mixerDelegate.trackViewModel.leftLevel, mixerDelegate.trackViewModel.rightLevel)
                            property double maxValue: -96
                            onValueChanged: maxValue = Math.max(maxValue, value)
                            horizontalAlignment: Text.AlignHCenter
                            color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                            opacity: 0.5
                            text: (Math.abs(maxValue + 96) < 0.05 ? '' : Qt.locale().toString(maxValue, "f", 1))
                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnPeakLabel)
                                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnPeakLabel)
                                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnPeakLabel)
                                onEntered: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverEntered, ScopicFlow.InteractionOnLevelMeter)
                                onExited: mixerDelegate.sendInteractionNotification(ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnLevelMeter)
                                onClicked: () => {
                                    leftChannelLevelMeter.clipping = false
                                    rightChannelLevelMeter.clipping = false
                                    peakText.maxValue = -96
                                    mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnPeakLabel)
                                }
                                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnPeakLabel)
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            Layout.topMargin: 8
            Layout.fillWidth: true
            height: 1
            color: Theme.splitterColor
        }
        ColumnLayout {
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.bottomMargin: 4
            Layout.fillWidth: true
            spacing: 0
            TrackListEditLabel {
                Layout.fillWidth: true
                enabled: !mixerDelegate.trackViewModel.bus
                height: 24
                center: true
                text: mixerDelegate.trackViewModel.name
                onEditingFinished: (text) => mixerDelegate.trackViewModel.name = text
                onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnTrackName)
                onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnTrackName)
                onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnTrackName)
                onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnTrackName)
                onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnTrackName)
                onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnTrackName)
            }
            RowLayout {
                Layout.leftMargin: 8
                Layout.rightMargin: 8
                Layout.fillWidth: true
                spacing: 8
                property double _opacity: mixerDelegate.isCurrent ? 1 : 0
                Behavior on _opacity {
                    NumberAnimation {
                        duration: Theme.colorAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }
                Rectangle {
                    height: 2
                    radius: 1
                    color: Theme.accentColor
                    opacity: parent._opacity
                    Layout.alignment: Qt.AlignVCenter
                    Layout.horizontalStretchFactor: 1
                    Layout.fillWidth: true
                }
                Text {
                    id: trackNumberLabel
                    text: Qt.locale().toString(mixerDelegate.trackNumber)
                    color: mixerDelegate.isCurrent ? Theme.accentColor : SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                    opacity: mixerDelegate.isCurrent ? 1 : 0.5
                    Behavior on color {
                        ColorAnimation {
                            duration: Theme.colorAnimationDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                    Layout.alignment: Qt.AlignVCenter
                }
                Rectangle {
                    height: 2
                    radius: 1
                    color: Theme.accentColor
                    opacity: parent._opacity
                    Layout.alignment: Qt.AlignVCenter
                    Layout.horizontalStretchFactor: 1
                    Layout.fillWidth: true
                }
            }
        }
        Button {
            Layout.fillWidth: true
            height: 8
            background: Rectangle {
                color: mixerDelegate.trackViewModel.color
            }
            onPressed: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Pressed, ScopicFlow.InteractionOnColorIndicator)
            onReleased: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Released, ScopicFlow.InteractionOnColorIndicator)
            onCanceled: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Canceled, ScopicFlow.InteractionOnColorIndicator)
            onHoveredChanged: mixerDelegate.sendInteractionNotification(hovered ? ScopicFlow.II_HoverEntered : ScopicFlow.II_HoverExited, ScopicFlow.InteractionOnColorIndicator)
            onClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_Clicked, ScopicFlow.InteractionOnColorIndicator)
            onDoubleClicked: mixerDelegate.sendInteractionNotification(ScopicFlow.II_DoubleClicked, ScopicFlow.InteractionOnColorIndicator)
        }
    }
}