import QtQml
import QtQml.Models
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {

    id: trackListDelegate

    required property string trackNumber
    required property QtObject trackViewModel

    onTrackViewModelChanged: {
        dummyModel.clear()
        if (trackListDelegate.trackViewModel)
            dummyModel.append({trackViewModel: trackListDelegate.trackViewModel})
    }

    property bool isCurrent: false
    property bool isLast: false

    property QtObject animationViewModel: null
    property Component mouseArea: null
    property Component trackExtraDelegate: null

    NumberAnimation on height {
        id: fitHeightAnimation
        to: 80
        easing.type: Easing.OutCubic
        duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1)
    }

    function fitHeight() {
        fitHeightAnimation.start()
    }

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: -1
        anchors.rightMargin: -1
        anchors.bottomMargin: -1
        color: trackListDelegate.trackViewModel.selected ? SFPalette.trackListSelectedColorChange.apply(SFPalette.trackListBackgroundColor) : SFPalette.trackListBackgroundColor
        Behavior on color {
            ColorAnimation {
                duration: 250 * (trackListDelegate.animationViewModel?.colorAnimationRatio ?? 1)
                easing.type: Easing.OutCubic
            }
        }
        border.width: 1
        border.color: Theme.borderColor

        clip: true

        Rectangle {
            id: colorIndicator
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 1
            width: 8
            color: trackListDelegate.trackViewModel.color
        }

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
                    duration: 250 * (trackListDelegate.animationViewModel?.colorAnimationRatio ?? 1)
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
            text: trackListDelegate.trackNumber
            color: trackListDelegate.isCurrent ? Theme.accentColor : SFPalette.suitableForegroundColor(SFPalette.trackListBackgroundColor)
            Behavior on color {
                ColorAnimation {
                    duration: 250 * (trackListDelegate.animationViewModel?.colorAnimationRatio ?? 1)
                    easing.type: Easing.OutCubic
                }
            }
        }

        Loader {
            sourceComponent: trackListDelegate.mouseArea
            anchors.fill: parent
        }

        Row {
            anchors.top: parent.top
            anchors.topMargin: 8
            anchors.left: parent.left
            anchors.leftMargin: 48
            spacing: 8
            TrackMSR {
                id: controlsFirstRow
                trackViewModel: trackListDelegate.trackViewModel
            }
            TrackListEditLabel {
                anchors.top: controlsFirstRow.top
                anchors.bottom: controlsFirstRow.bottom
                text: trackListDelegate.trackViewModel.name
                onEditingFinished: function (text) {
                    trackListDelegate.trackViewModel.name = text
                }
            }

        }

        Row {
            anchors.top: parent.top
            anchors.topMargin: 48
            anchors.left: parent.left
            anchors.leftMargin: 48
            id: controlsSecondRow
            spacing: 0
            visible: opacity !== 0.0
            Behavior on opacity {
                NumberAnimation {
                    duration: (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0) * 250
                    easing.type: Easing.OutCubic
                }
            }
            readonly property bool intermediate: gainSlider.pressed || panDial.pressed
            onIntermediateChanged: {
                trackListDelegate.trackViewModel.intermediate = intermediate
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
                            duration: (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0) * 250
                            easing.type: Easing.OutCubic
                        }
                    }
                    from: SVS.decibelToLinearValue(-96) - SVS.decibelToLinearValue(0)
                    to: SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(0)
                    value: SVS.decibelToLinearValue(trackListDelegate.trackViewModel.gain)
                    onValueChanged: {
                        let v = SVS.linearValueToDecibel(value + SVS.decibelToLinearValue(0))
                        if (Math.abs(trackListDelegate.trackViewModel.gain - v) > Number.EPSILON * 1000)
                            trackListDelegate.trackViewModel.gain = v
                    }
                }
                TrackListEditLabel {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 64
                    text: (Math.abs(trackListDelegate.trackViewModel.gain + 96) < 0.05 ? '-INF ' : trackListDelegate.trackViewModel.gain.toFixed(1)) + "dB"
                    editText: trackListDelegate.trackViewModel.gain.toFixed(1)
                    validator: DoubleValidator { // TODO use svscraft expression validator instead
                        bottom: -96
                        top: 6
                        decimals: 1
                    }
                    onEditingFinished: function (text) {
                        trackListDelegate.trackViewModel.gain = parseFloat(text)
                    }
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
                    height: 24
                    width: 24
                    from: -1.0
                    to: 1.0
                    value: trackListDelegate.trackViewModel.pan
                    onValueChanged: {
                        trackListDelegate.trackViewModel.pan = value
                    }
                }
                TrackListEditLabel {
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: 64
                    text: Math.round(trackListDelegate.trackViewModel.pan * 100)
                    validator: IntValidator { // TODO use svscraft expression validator instead
                        bottom: -100
                        top: 100
                    }
                    onEditingFinished: function (text) {
                        trackListDelegate.trackViewModel.pan = parseInt(text) * 0.01
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
                onClicked: {
                    leftChannelLevelMeter.clipping = false
                    rightChannelLevelMeter.clipping = false
                }
            }
        }

    }

    onHeightChanged: {
        if (height < 80) {
            controlsSecondRow.opacity = 0
        } else {
            controlsSecondRow.opacity = 1
        }
    }

}