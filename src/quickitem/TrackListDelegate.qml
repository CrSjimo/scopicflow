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
    required property QtObject trackViewModel

    property bool isCurrent: false
    property bool isLast: false

    required property QtObject palette

    property QtObject animationViewModel: null

    NumberAnimation on height {
        id: fitHeightAnimation
        to: 80
        easing.type: Easing.OutCubic
        duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1)
    }

    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: -1
        anchors.rightMargin: -1
        anchors.bottomMargin: -1
        color: trackListDelegate.trackViewModel.selected ? trackListDelegate.palette.selectedBackgroundColor : trackListDelegate.palette.backgroundColor
        Behavior on color {
            ColorAnimation {
                duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1)
                easing.type: Easing.OutCubic
            }
        }
        border.width: 1
        border.color: trackListDelegate.palette.borderColor

        clip: true

        Rectangle {
            id: selectionIndicator
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 8
            width: 2
            color: trackListDelegate.palette.primaryColor
            visible: opacity !== 0
            opacity: trackListDelegate.isCurrent ? 1 : 0
            Behavior on opacity {
                NumberAnimation {
                    duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1)
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
            color: trackListDelegate.isCurrent ? trackListDelegate.palette.primaryColor : trackListDelegate.palette.foregroundColor
            Behavior on color {
                ColorAnimation {
                    duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1)
                    easing.type: Easing.OutCubic
                }
            }
        }

        Rectangle {
            id: mouseInteractionTaget
            anchors.fill: parent
            opacity: 0
            readonly property bool isMouseInteractionTarget: true
        }

        TrackListButton {
            id: fitHeightButton
            width: 16
            height: 16
            palette: trackListDelegate.palette
            animationRatio: trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0
            anchors.left: selectionIndicator.right
            anchors.leftMargin: 18
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            checkable: false
            opacity: hovered ? 1.0 : 0.0
            Behavior on opacity {
                NumberAnimation {
                    duration: 250 * (trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0)
                    easing.type: Easing.OutCubic
                }
            }
            visible: trackListDelegate.height !== 80
            rotation: trackListDelegate.height > 80 ? 0 : 180
            contentItem: FluentSystemIcon {
                icon: 'chevron_up_20_filled'
                color: trackListDelegate.palette.foregroundColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
            onClicked: {
                fitHeightAnimation.start()
            }
            toolTip: "Fit height"
        }

        Column {
            anchors.left: parent.left
            anchors.leftMargin: 48
            anchors.top: parent.top
            anchors.topMargin: 8
            spacing: 16
            readonly property bool isMouseInteractionTarget: true

            Row {
                spacing: 8
                readonly property bool isMouseInteractionTarget: true
                Row {
                    id: controlsFirstRow
                    spacing: 4
                    TrackListButton {
                        id: muteButton
                        checkedColor: trackListDelegate.palette.muteColor
                        palette: trackListDelegate.palette
                        animationRatio: trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0
                        text: 'M'
                        checked: trackListDelegate.trackViewModel.mute
                        onCheckedChanged: trackListDelegate.trackViewModel.mute = checked
                        toolTip: qsTr("Mute")
                    }
                    TrackListButton {
                        id: soloButton
                        checkedColor: trackListDelegate.palette.soloColor
                        palette: trackListDelegate.palette
                        animationRatio: trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0
                        text: 'S'
                        checked: trackListDelegate.trackViewModel.solo
                        onCheckedChanged: trackListDelegate.trackViewModel.solo = checked
                        toolTip: qsTr("Solo")
                    }
                    TrackListButton {
                        id: recordButton
                        checkedColor: trackListDelegate.palette.recordColor
                        palette: trackListDelegate.palette
                        animationRatio: trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0
                        text: 'R'
                        checked: trackListDelegate.trackViewModel.record
                        onCheckedChanged: trackListDelegate.trackViewModel.record = checked
                        toolTip: qsTr("Record")
                    }
                }
                Text {
                    anchors.verticalCenter: controlsFirstRow.verticalCenter
                    text: trackListDelegate.trackViewModel.name
                    color: trackListDelegate.palette.foregroundColor
                }
            }

            Row {
                id: controlsSecondRow
                spacing: 0
                visible: opacity !== 0.0
                readonly property bool isMouseInteractionTarget: true
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
                    readonly property bool isMouseInteractionTarget: true
                    FluentSystemIcon {
                        anchors.verticalCenter: gainSlider.verticalCenter
                        icon: 'cellular_data_1_20_regular'
                        font.pixelSize: 24
                        color: trackListDelegate.palette.foregroundColor
                    }
                    TrackListSlider {
                        id: gainSlider
                        palette: trackListDelegate.palette
                        animationRatio: trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0
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
                        from: decibelToLinearValue(-96)
                        to: decibelToLinearValue(6)
                        defaultValue: decibelToLinearValue(0)
                        value: decibelToLinearValue(trackListDelegate.trackViewModel.gain)
                        onValueChanged: {
                            let v = linearValueToDecibel(value)
                            if (Math.abs(trackListDelegate.trackViewModel.gain - v) > Number.EPSILON * 1000)
                                trackListDelegate.trackViewModel.gain = v
                        }
                        onReset: {
                            trackListDelegate.trackViewModel.gain = 0
                        }
                        toolTip: enabled ? qsTr("Gain") : ""
                    }
                    Text {
                        anchors.verticalCenter: gainSlider.verticalCenter
                        width: 64
                        text: (Math.abs(gainSlider.value - gainSlider.from) < Number.EPSILON ? '-INF ' : linearValueToDecibel(gainSlider.value).toFixed(1)) + "dB"
                        color: trackListDelegate.palette.foregroundColor
                    }
                }
                Row {
                    spacing: 8
                    readonly property bool isMouseInteractionTarget: true
                    FluentSystemIcon {
                        height: 24
                        icon: 'live_20_regular'
                        font.pixelSize: 24
                        color: trackListDelegate.palette.foregroundColor
                    }
                    TrackListDial {
                        id: panDial
                        palette: trackListDelegate.palette
                        animationRatio: trackListDelegate.animationViewModel?.visualEffectAnimationRatio ?? 1.0
                        height: 24
                        width: 24
                        from: -1.0
                        to: 1.0
                        defaultValue: 0
                        value: trackListDelegate.trackViewModel.pan
                        toolTip: qsTr("Pan")
                        onValueChanged: {
                            trackListDelegate.trackViewModel.pan = value
                        }
                        onReset: {
                            trackListDelegate.trackViewModel.pan = 0
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
                    value: trackListDelegate.trackViewModel.leftLevel
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
                    value: trackListDelegate.trackViewModel.rightLevel
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
            controlsSecondRow.opacity = 0
        } else {
            controlsSecondRow.opacity = 1
        }
    }

}