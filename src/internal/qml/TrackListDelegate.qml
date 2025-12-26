import QtQml
import QtQml.Models
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.impl
import QtQuick.Templates as T

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: track

    property bool current: {
        current = selectionController?.currentItem === trackViewModel;
    }
    property bool editing: false

    required property SelectionController selectionController
    required property QtObject trackViewModel
    required property string trackNumber

    function fitHeight() {
        fitHeightAnimation.start();
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

    Rectangle {
        anchors.fill: parent
        clip: true
        color: track.trackViewModel.selected ? SFPalette.trackListSelectedColorChange.apply(Theme.backgroundTertiaryColor) : Theme.backgroundTertiaryColor

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }

        RowLayout {
            anchors.fill: parent
            spacing: 8
            T.Button {
                id: colorIndicator

                Layout.fillHeight: true
                implicitWidth: 8

                background: Rectangle {
                    color: track.trackViewModel.color
                }

            }
            Item {
                Layout.fillHeight: true
                implicitWidth: 24

                Rectangle {
                    id: focusIndicator

                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.leftMargin: 0
                    anchors.margins: 8
                    anchors.top: parent.top
                    color: Theme.accentColor
                    opacity: track.current ? 1 : 0
                    radius: 1
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

                    anchors.left: focusIndicator.right
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.top
                    anchors.verticalCenterOffset: 20
                    color: track.current ? Theme.accentColor : Theme.foregroundPrimaryColor
                    text: track.trackNumber

                    Behavior on color {
                        ColorAnimation {
                            duration: Theme.colorAnimationDuration
                            easing.type: Easing.OutCubic
                        }
                    }
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.topMargin: 8
                Layout.alignment: Qt.AlignTop
                implicitHeight: 64

                Row {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    spacing: 8

                    TrackMSR {
                        id: controlsFirstRow
                        trackViewModel: track.trackViewModel
                    }
                    TrackEditLabel {
                        anchors.bottom: controlsFirstRow.bottom
                        anchors.top: controlsFirstRow.top
                        text: track.trackViewModel.name
                    }
                }
                RowLayout {
                    id: controlsSecondRow

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: 40
                    spacing: 24
                    visible: opacity !== 0.0

                    Behavior on opacity {
                        NumberAnimation {
                            duration: Theme.visualEffectAnimationDuration
                            easing.type: Easing.OutCubic
                        }
                    }

                    RowLayout {
                        spacing: 4
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        IconImage {
                            color: Theme.foregroundPrimaryColor
                            source: "image://fluent-system-icons/cellular_data_1?size=20"
                            sourceSize.width: 24
                            sourceSize.height: 24
                        }
                        Slider {
                            id: gainSlider
                            Layout.fillWidth: true

                            from: SVS.decibelToLinearValue(-96) - SVS.decibelToLinearValue(0)
                            to: SVS.decibelToLinearValue(6) - SVS.decibelToLinearValue(0)
                            value: SVS.decibelToLinearValue(track.trackViewModel.gain)

                            readonly property bool visualVisible: width > 32
                            enabled: visualVisible
                            opacity: visualVisible ? 1 : 0

                            Behavior on opacity {
                                NumberAnimation {
                                    duration: Theme.visualEffectAnimationDuration
                                    easing.type: Easing.OutCubic
                                }
                            }

                        }
                        TrackEditLabel {
                            Layout.fillHeight: true
                            editText: Qt.locale().toString(track.trackViewModel.gain, "f", 1)
                            text: (track.trackViewModel.gain + 96 < 0.05 ? "-∞" : Qt.locale().toString(track.trackViewModel.gain, "f", 1)) + " dB"
                            implicitWidth: 32

                            validator: DoubleValidator {
                                bottom: -96
                                decimals: 1
                                top: 6
                            }
                        }
                    }
                    RowLayout {
                        spacing: 8
                        Layout.fillHeight: true

                        IconImage {
                            color: Theme.foregroundPrimaryColor
                            source: "image://fluent-system-icons/live?size=20"
                            sourceSize.width: 24
                            sourceSize.height: 24
                        }
                        Dial {
                            id: panDial

                            from: -1.0
                            height: 24
                            to: 1.0
                            value: track.trackViewModel.pan
                        }
                        TrackEditLabel {
                            Layout.fillHeight: true
                            text: Qt.locale().toString(Math.round(track.trackViewModel.pan * 100))
                            implicitWidth: 32

                            validator: IntValidator {
                                bottom: -100
                                top: 100
                            }
                        }
                    }
                }
            }
            Rectangle {
                Layout.fillHeight: true
                color: SFPalette.levelMeterColor
                implicitWidth: 12

                LevelMeter {
                    id: leftChannelLevelMeter

                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.horizontalCenter
                    anchors.top: parent.top
                    value: track.trackViewModel.leftLevel
                }
                LevelMeter {
                    id: rightChannelLevelMeter

                    anchors.bottom: parent.bottom
                    anchors.left: parent.horizontalCenter
                    anchors.right: parent.right
                    anchors.top: parent.top
                    value: track.trackViewModel.rightLevel
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }
    }
}
