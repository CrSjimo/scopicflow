/******************************************************************************
 * Copyright (c) 2025 OpenVPI                                                 *
 *                                                                            *
 * This file is part of SVSCraft                                              *
 *                                                                            *
 * SVSCraft is free software: you can redistribute it and/or modify it under  *
 * the terms of the GNU Lesser General Public License as published by the     *
 * Free Software Foundation, either version 3 of the License, or (at your     *
 * option) any later version.                                                 *
 *                                                                            *
 * SVSCraft is distributed in the hope that it will be useful, but WITHOUT    *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or      *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public        *
 * License for more details.                                                  *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public License   *
 * along with SVSCraft. If not, see <https://www.gnu.org/licenses/>.          *
 ******************************************************************************/

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
    required property int trackNumber
    property bool isCurrent: false
    property QtObject animationViewModel: null
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

    Rectangle {
        anchors.fill: parent
        color: mixerDelegate.trackViewModel.selected ? SFPalette.trackListSelectedColorChange.apply(SFPalette.trackListBackgroundColor) : SFPalette.trackListBackgroundColor
        Behavior on color {
            ColorAnimation {
                duration: 250 * (mixerDelegate.animationViewModel?.colorAnimationRatio ?? 1)
                easing.type: Easing.OutCubic
            }
        }
    }
    Loader {
        sourceComponent: mixerDelegate.mouseArea
        anchors.fill: parent
    }
    ColumnLayout {
        spacing: 0
        anchors.fill: parent
        RowLayout {
            Layout.leftMargin: routeButton.visible ? 28 : 42
            Layout.rightMargin: 8
            Layout.topMargin: 8
            Layout.fillWidth: true
            Dial {
                id: panDial
                from: -1.0
                to: 1.0
                value: mixerDelegate.trackViewModel.pan
                onValueChanged: {
                    mixerDelegate.trackViewModel.pan = value
                }
            }
            TrackListEditLabel {
                width: 24
                height: 24
                text: Math.round(mixerDelegate.trackViewModel.pan * 100)
                validator: IntValidator { // TODO use svscraft expression validator instead
                    bottom: -100
                    top: 100
                }
                onEditingFinished: function (text) {
                    mixerDelegate.trackViewModel.pan = parseInt(text) * 0.01
                }
            }
            Item {
                Layout.fillWidth: true
            }
            ToolButton {
                id: routeButton
                ThemedItem.controlType: SVS.CT_Accent
                checkable: true
                flat: false
                text: 'R'
                visible: typeof(mixerDelegate.trackViewModel.route) === "boolean"
                checked: mixerDelegate.trackViewModel.route ?? false
                onCheckedChanged: mixerDelegate.trackViewModel.route = checked
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
            }
            RowLayout {
                anchors.top: msr.bottom
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                ColumnLayout {
                    Layout.fillHeight: true
                    MixerSlider {
                        Layout.alignment: Qt.AlignHCenter
                        Layout.fillHeight: true
                        value: SVS.decibelToLinearValue(mixerDelegate.trackViewModel.gain)
                        onValueChanged: {
                            let v = SVS.linearValueToDecibel(value + SVS.decibelToLinearValue(0))
                            if (Math.abs(mixerDelegate.trackViewModel.gain - v) > Number.EPSILON * 1000)
                                mixerDelegate.trackViewModel.gain = v
                        }
                    }
                    TrackListEditLabel {
                        width: 48
                        height: 24
                        text: (Math.abs(mixerDelegate.trackViewModel.gain + 96) < 0.05 ? '-INF ' : mixerDelegate.trackViewModel.gain.toFixed(1)) + "dB"
                        editText: mixerDelegate.trackViewModel.gain.toFixed(1)
                        center: true
                        validator: DoubleValidator { // TODO use svscraft expression validator instead
                            bottom: -96
                            top: 6
                            decimals: 1
                        }
                        onEditingFinished: function (text) {
                            mixerDelegate.trackViewModel.gain = parseFloat(text)
                        }
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
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.horizontalCenter
                            value: mixerDelegate.trackViewModel.leftLevel
                        }
                        LevelMeter {
                            id: rightChannelLevelMeter
                            anchors.top: parent.top
                            anchors.bottom: parent.bottom
                            anchors.left: parent.horizontalCenter
                            anchors.right: parent.right
                            value: mixerDelegate.trackViewModel.rightLevel
                        }
                        TapHandler {
                            onSingleTapped: () => {
                                leftChannelLevelMeter.clipping = false
                                rightChannelLevelMeter.clipping = false
                                peakText.maxValue = -96
                            }
                        }
                    }
                    Item {
                        Layout.fillWidth: true
                        Layout.bottomMargin: 4
                        height: peakText.height
                        Text {
                            id: peakText
                            property double value: Math.max(mixerDelegate.trackViewModel.leftLevel, mixerDelegate.trackViewModel.rightLevel)
                            property double maxValue: -96
                            onValueChanged: maxValue = Math.max(maxValue, value)
                            horizontalAlignment: Text.AlignHCenter
                            color: SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                            opacity: 0.5
                            text: (Math.abs(maxValue + 96) < 0.05 ? '' : maxValue.toFixed(1))
                            TapHandler {
                                onSingleTapped: () => {
                                    leftChannelLevelMeter.clipping = false
                                    rightChannelLevelMeter.clipping = false
                                    peakText.maxValue = -96
                                }
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
                height: 24
                center: true
                text: mixerDelegate.trackViewModel.name
                onEditingFinished: function (text) {
                    mixerDelegate.trackViewModel.name = text
                }
            }
            RowLayout {
                Layout.leftMargin: 8
                Layout.rightMargin: 8
                Layout.fillWidth: true
                spacing: 8
                property double _opacity: mixerDelegate.isCurrent ? 1 : 0
                Behavior on _opacity {
                    NumberAnimation {
                        duration: 250 * (mixerDelegate.animationViewModel?.colorAnimationRatio ?? 1)
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
                    text: mixerDelegate.trackNumber
                    color: mixerDelegate.isCurrent ? Theme.accentColor : SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
                    opacity: mixerDelegate.isCurrent ? 1 : 0.5
                    Behavior on color {
                        ColorAnimation {
                            duration: 250 * (mixerDelegate.animationViewModel?.colorAnimationRatio ?? 1)
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
        Rectangle {
            Layout.fillWidth: true
            height: 8
            color: mixerDelegate.trackViewModel.color
        }
    }
}