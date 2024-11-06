import QtQml
import QtQuick
import QtQuick.Controls.Basic

import './HelperComponents'

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
    }

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
            anchors.verticalCenter: parent.verticalCenter
            text: trackListDelegate.trackNumber
            color: trackListDelegate.selected ? trackListDelegate.palette.primaryColor : trackListDelegate.palette.foregroundColor
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
                spacing: 0
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
                        height: 24
                        width: 144
                        from: decibelToLinearValue(-96)
                        to: decibelToLinearValue(6)
                        defaultValue: decibelToLinearValue(0)

                    }
                    Text {
                        anchors.verticalCenter: gainSlider.verticalCenter
                        width: 64
                        text: (gainSlider.value - gainSlider.from < Number.EPSILON ? '-INF ' : linearValueToDecibel(gainSlider.value).toFixed(1)) + "dB"
                        color: trackListDelegate.palette.foregroundColor
                    }
                }
                Row {
                    spacing: 4
                    FluentSystemIcon {
                        height: 24
                        icon: 'live_20_regular'
                        font.pixelSize: 24
                        color: trackListDelegate.palette.foregroundColor
                    }
                    Dial {
                        height: 24
                        width: 24
                    }
                }
            }

        }

    }


}