import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Templates as T

import SVSCraft
import SVSCraft.UIComponents
import SVSCraft.UIComponents.impl

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: mixerDelegate

    property bool current: selectionController?.currentItem === trackViewModel

    required property SelectionController selectionController
    required property QtObject trackViewModel
    required property string trackNumber
    property Item mouseArea: null

    readonly property Dial panDial: panDial
    readonly property TrackEditLabel panEditLabel: panEditLabel
    readonly property ToolButton multiChannelOutputButton: multiChannelOutputButton
    readonly property TrackMSR trackMSR: msr
    readonly property MixerSlider gainSlider: gainSlider
    readonly property TrackEditLabel gainEditLabel: gainEditLabel
    readonly property MouseArea levelMeterMouseArea: levelMeterMouseArea
    readonly property LevelMeter leftLevelMeter: leftChannelLevelMeter
    readonly property LevelMeter rightLevelMeter: rightChannelLevelMeter
    readonly property MouseArea peakMouseArea: peakMouseArea
    readonly property Text peakLabel: peakText
    readonly property TrackEditLabel trackNameEditLabel: trackNameEditLabel
    readonly property T.Button colorIndicator: colorIndicator

    width: 128

    Rectangle {
        anchors.fill: parent
        color: mixerDelegate.trackViewModel.selected ? SFPalette.trackListSelectedColorChange.apply(Theme.backgroundTertiaryColor) : Theme.backgroundTertiaryColor

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
    }

    StackLayout {
        anchors.fill: parent
        data: [mixerDelegate.mouseArea]
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: multiChannelOutputButton.visible ? 28 : 42
            Layout.rightMargin: 8
            Layout.topMargin: 8

            Dial {
                id: panDial

                LayoutMirroring.enabled: false
                from: -1.0
                to: 1.0
                value: mixerDelegate.trackViewModel.pan
            }
            TrackEditLabel {
                id: panEditLabel

                height: 24
                text: Qt.locale().toString(Math.round(mixerDelegate.trackViewModel.pan * 100))
                implicitWidth: 24

                validator: IntValidator {
                    bottom: -100
                    top: 100
                }
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                id: multiChannelOutputButton

                Theme.accentColor: SFPalette.routeColor
                ThemedItem.controlType: SVS.CT_Accent
                checkable: true
                checked: mixerDelegate.trackViewModel.multiChannelOutput
                flat: false
                display: T.AbstractButton.IconOnly
                text: "Multi-Channel Output"
                icon.source: "image://fluent-system-icons/list" // TODO find a proper icon
            }
        }
        Rectangle {
            Layout.bottomMargin: 8
            Layout.fillWidth: true
            Layout.topMargin: 8
            color: Theme.borderColor
            height: 1
        }
        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true

            TrackMSR {
                id: msr

                anchors.horizontalCenter: parent.horizontalCenter
                trackViewModel: mixerDelegate.trackViewModel
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
                    }
                    TrackEditLabel {
                        id: gainEditLabel

                        center: true
                        editText: Qt.locale().toString(mixerDelegate.trackViewModel.gain, "f", 1)
                        height: 24
                        text: (mixerDelegate.trackViewModel.gain + 96 < 0.05 ? "-∞" : Qt.locale().toString(mixerDelegate.trackViewModel.gain, "f", 1)) + " dB"
                        implicitWidth: 48

                        validator: DoubleValidator {
                            bottom: -96
                            decimals: 1
                            top: 6
                        }
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
                            clipping: mixerDelegate.trackViewModel.leftClipping
                        }
                        LevelMeter {
                            id: rightChannelLevelMeter

                            LayoutMirroring.enabled: false
                            anchors.bottom: parent.bottom
                            anchors.left: parent.horizontalCenter
                            anchors.right: parent.right
                            anchors.top: parent.top
                            value: mixerDelegate.trackViewModel.rightLevel
                            clipping: mixerDelegate.trackViewModel.rightClipping
                        }
                        MouseArea {
                            id: levelMeterMouseArea

                            anchors.fill: parent
                            hoverEnabled: true
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
                            color: Theme.foregroundPrimaryColor
                            horizontalAlignment: Text.AlignHCenter
                            opacity: 0.5
                            text: (Math.abs(maxValue + 96) < 0.05 ? "" : Qt.locale().toString(maxValue, "f", 1))

                            onValueChanged: maxValue = Math.max(maxValue, value)

                            MouseArea {
                                id: peakMouseArea

                                anchors.fill: parent
                                hoverEnabled: true
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.topMargin: 8
            color: Theme.borderColor
            height: 1
        }
        ColumnLayout {
            Layout.bottomMargin: 4
            Layout.fillWidth: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            spacing: 0

            TrackEditLabel {
                id: trackNameEditLabel

                Layout.fillWidth: true
                center: true
                height: 24
                text: mixerDelegate.trackViewModel.name
            }
            RowLayout {
                property double _opacity: mixerDelegate.current ? 1 : 0

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
                    color: mixerDelegate.current ? Theme.accentColor : Theme.foregroundPrimaryColor
                    opacity: mixerDelegate.current ? 1 : 0.5
                    text: mixerDelegate.trackNumber

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
            id: colorIndicator

            Layout.fillWidth: true
            height: 8

            background: Rectangle {
                color: mixerDelegate.trackViewModel.color
            }
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
            color: Theme.borderColor
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
                        color: Theme.foregroundPrimaryColor
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
                control.ThemedItem.doubleClickReset()
                control.handle._doubleClickTriggered = false;
            }
        }
    }
}
