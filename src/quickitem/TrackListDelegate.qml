import QtQml
import QtQuick
import QtQuick.Controls.Basic

import './'
import './HelperComponents'
import './HelperComponents/FluentSystemIconCharset.mjs' as Charset

Item {

    id: trackListDelegate

    function decibelToLinearValue(decibel, factor = -15) {
        return Math.exp((decibel - factor) / - factor) - Math.E;
    }

    function linearValueToDecibel(linearValue, factor = -15) {
        return -factor * Math.log(linearValue + Math.E) + factor;
    }

    required property string trackNumber
    required property string trackName

    property bool mute: muteButton.checked
    property bool solo: soloButton.checked
    property bool recordReady: recordReadyButton.checked

    property double gain: 0
    property double pan: 0
    property bool intermediate: false

    property bool selected: false

    property double leftLevel: 0
    property double rightLevel: 0

    readonly property var palette: Item {
        property color backgroundColor: "#333333"
        property color foregroundColor: "#FFFFFF"
        property color primaryColor: "#00FFFF"
        property color borderColor: "#CCCCCC"

        property color muteColor: "#CC6600"
        property color soloColor: "#00CC00"
        property color recordReadyColor: "#CC3333"

        property color levelLowColor: "#33CC33"
        property color levelMiddleColor: "#FFCC33"
        property color levelHighColor: "#FF3333"
        property color levelBackgroundColor: "#222222"
        property color levelBorderColor: "#000000"
    }

    property var animationViewModel: null

    Rectangle {
        anchors.fill: parent
        color: trackListDelegate.palette.backgroundColor
        border.width: 1
        border.color: trackListDelegate.palette.borderColor

        Rectangle {
            id: selectionIndicator
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 8
            width: 2
            color: trackListDelegate.palette.primaryColor
            visible: trackListDelegate.selected
        }

        Text {
            id: trackNumberLabel
            anchors.left: selectionIndicator.right
            anchors.leftMargin: 4
            anchors.verticalCenter: parent.top
            anchors.verticalCenterOffset: 20
            text: trackListDelegate.trackNumber
            color: trackListDelegate.selected ? trackListDelegate.palette.primaryColor : trackListDelegate.palette.foregroundColor
        }

        TrackListButton {
            id: fitHeightButton
            width: 16
            height: 16
            borderColor: trackListDelegate.palette.borderColor; foregroundColor: trackListDelegate.palette.foregroundColor
            anchors.left: selectionIndicator.right
            anchors.leftMargin: 18
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            checkable: false
            opacity: hovered ? 1.0 : 0.0
            visible: trackListDelegate.height !== 80
            rotation: trackListDelegate.height > 80 ? 0 : 180
            contentItem: FluentSystemIcon {
                icon: 'chevron_up_20_filled'
                color: trackListDelegate.palette.foregroundColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                trackListDelegate.height = 80
            }
            toolTip: "Fit height"
        }

        Column {
            anchors.left: parent.left
            anchors.leftMargin: 48
            anchors.top: parent.top
            anchors.topMargin: 8
            spacing: 16

            Row {
                spacing: 8
                Row {
                    id: controlsFirstRow
                    spacing: 4
                    TrackListButton {
                        id: muteButton
                        checkedColor: trackListDelegate.palette.muteColor; borderColor: trackListDelegate.palette.borderColor; foregroundColor: trackListDelegate.palette.foregroundColor
                        text: 'M'
                        checked: trackListDelegate.mute
                        toolTip: qsTr("Mute")
                    }
                    TrackListButton {
                        id: soloButton
                        checkedColor: trackListDelegate.palette.soloColor; borderColor: trackListDelegate.palette.borderColor; foregroundColor: trackListDelegate.palette.foregroundColor
                        text: 'S'
                        checked: trackListDelegate.solo
                        toolTip: qsTr("Solo")
                    }
                    TrackListButton {
                        id: recordReadyButton
                        checkedColor: trackListDelegate.palette.recordReadyColor; borderColor: trackListDelegate.palette.borderColor; foregroundColor: trackListDelegate.palette.foregroundColor
                        text: 'R'
                        checked: trackListDelegate.recordReady
                        toolTip: qsTr("Record")
                    }
                }
                Text {
                    anchors.verticalCenter: controlsFirstRow.verticalCenter
                    text: trackListDelegate.trackName
                    color: trackListDelegate.palette.foregroundColor
                }
            }

            Row {
                id: controlsSecondRow
                spacing: 0
                visible: opacity !== 0.0
                NumberAnimation on opacity {
                    id: controlsSecondRowAppear
                    to: 1.0
                    // duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1)
                }
                NumberAnimation on opacity {
                    id: controlsSecondRowDisappear
                    to: 0.0
                    // duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1)
                }
                Row {
                    spacing: 4
                    FluentSystemIcon {
                        anchors.verticalCenter: gainSlider.verticalCenter
                        icon: 'cellular_data_1_20_regular'
                        font.pixelSize: 24
                        color: trackListDelegate.palette.foregroundColor
                    }
                    TrackListSlider {
                        id: gainSlider
                        foregroundColor: trackListDelegate.palette.foregroundColor
                        primaryColor: trackListDelegate.palette.primaryColor
                        backgroundColor: trackListDelegate.palette.backgroundColor
                        animationRatio: trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0
                        height: 24
                        width: trackListDelegate. width - 256
                        from: decibelToLinearValue(-96)
                        to: decibelToLinearValue(6)
                        defaultValue: decibelToLinearValue(0)
                        value: decibelToLinearValue(trackListDelegate.gain)
                        toolTip: qsTr("Gain")
                        onValueChanged: {
                            trackListDelegate.gain = linearValueToDecibel(value)
                        }
                        onPressedChanged: {
                            trackListDelegate.intermediate = pressed
                        }

                    }
                    Text {
                        anchors.verticalCenter: gainSlider.verticalCenter
                        width: 64
                        text: (gainSlider.value - gainSlider.from < Number.EPSILON ? '-INF ' : linearValueToDecibel(gainSlider.value).toFixed(1)) + "dB"
                        color: trackListDelegate.palette.foregroundColor
                    }
                }
                Row {
                    spacing: 8
                    FluentSystemIcon {
                        height: 24
                        icon: 'live_20_regular'
                        font.pixelSize: 24
                        color: trackListDelegate.palette.foregroundColor
                    }
                    TrackListDial {
                        id: panDial
                        foregroundColor: trackListDelegate.palette.foregroundColor
                        primaryColor: trackListDelegate.palette.primaryColor
                        backgroundColor: trackListDelegate.palette.backgroundColor
                        height: 24
                        width: 24
                        from: -1.0
                        to: 1.0
                        defaultValue: 0
                        value: trackListDelegate.pan
                        toolTip: qsTr("Pan")
                        onValueChanged: {
                            trackListDelegate.pan = value
                        }
                    }
                    Text {
                        anchors.verticalCenter: panDial.verticalCenter
                        width: 64
                        text: Math.round(panDial.value * 100)
                        color: trackListDelegate.palette.foregroundColor
                    }
                }
            }

        }

        Rectangle {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.margins: 4
            color: trackListDelegate.palette.levelBackgroundColor
            width: 14
            radius: 2
            Row {
                id: levelMeterRow
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.right: parent.right
                anchors.margins: 2
                spacing: 2
                LevelMeter {
                    id: leftChannelLevelMeter
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    lowColor: trackListDelegate.palette.levelLowColor
                    middleColor: trackListDelegate.palette.levelMiddleColor
                    highColor: trackListDelegate.palette.levelHighColor
                    backgroundColor: trackListDelegate.palette.levelBackgroundColor
                    borderColor: trackListDelegate.palette.levelBorderColor
                    value: trackListDelegate.leftLevel
                }

                LevelMeter {
                    id: rightChannelLevelMeter
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    lowColor: trackListDelegate.palette.levelLowColor
                    middleColor: trackListDelegate.palette.levelMiddleColor
                    highColor: trackListDelegate.palette.levelHighColor
                    backgroundColor: trackListDelegate.palette.levelBackgroundColor
                    borderColor: trackListDelegate.palette.levelBorderColor
                    value: trackListDelegate.rightLevel
                }

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
            controlsSecondRowAppear.stop()
            if (!controlsSecondRowDisappear.running && controlsSecondRow.opacity !== 0.0) {
                controlsSecondRowDisappear.start()
            }
        } else {
            controlsSecondRowDisappear.stop()
            if (!controlsSecondRowAppear.running && controlsSecondRow.opacity !== 1.0) {
                controlsSecondRowAppear.start()
            }
        }
    }

}