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
    property bool rightAligned: false

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
    }

    RowLayout {
        anchors.fill: parent
        spacing: 8
        layoutDirection: track.rightAligned ? Qt.RightToLeft : Qt.LeftToRight
        T.Button {
            id: colorIndicator

            Layout.fillHeight: true
            implicitWidth: 8

            background: Rectangle {
                color: track.trackViewModel.color
            }

        }
        RowLayout {
            Layout.fillHeight: true
            spacing: 8
            layoutDirection: track.rightAligned ? Qt.RightToLeft : Qt.LeftToRight

            Rectangle {
                id: focusIndicator

                Layout.fillHeight: true
                Layout.topMargin: 8
                Layout.bottomMargin: 7
                color: Theme.accentColor
                opacity: track.current ? 1 : 0
                radius: 1
                implicitWidth: 2

                Behavior on opacity {
                    NumberAnimation {
                        duration: Theme.colorAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }
            }
            Text {
                id: trackNumberLabel
                color: track.current ? Theme.accentColor : Theme.foregroundPrimaryColor
                text: track.trackNumber
                Layout.minimumWidth: 16
                Layout.maximumWidth: 16
                Layout.bottomMargin: 40
                horizontalAlignment: track.rightAligned ? Qt.AlignRight : Qt.AlignLeft

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
            implicitHeight: 32

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
                spacing: 0
                visible: opacity !== 0.0

                Behavior on opacity {
                    NumberAnimation {
                        duration: Theme.visualEffectAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }

                RowLayout {
                    spacing: 2
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

                        Accessible.name: qsTr("Gain")
                        DescriptiveText.toolTip: Accessible.name
                        DescriptiveText.activated: hovered

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
                        implicitWidth: 64

                        validator: DoubleValidator {
                            bottom: -96
                            decimals: 1
                            top: 6
                        }
                    }
                }
                RowLayout {
                    spacing: 4
                    Layout.fillHeight: true

                    IconImage {
                        color: Theme.foregroundPrimaryColor
                        source: "image://fluent-system-icons/live?size=20"
                        sourceSize.width: 24
                        sourceSize.height: 24
                    }
                    Dial {
                        id: panDial

                        Accessible.name: qsTr("Pan")
                        DescriptiveText.toolTip: Accessible.name
                        DescriptiveText.activated: hovered

                        leftPadding: 2

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
                clipping: track.trackViewModel.leftClipping
            }
            LevelMeter {
                id: rightChannelLevelMeter

                anchors.bottom: parent.bottom
                anchors.left: parent.horizontalCenter
                anchors.right: parent.right
                anchors.top: parent.top
                value: track.trackViewModel.rightLevel
                clipping: track.trackViewModel.rightClipping
            }
            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
            }
        }
    }
}
