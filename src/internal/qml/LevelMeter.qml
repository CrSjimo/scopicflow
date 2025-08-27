import QtQuick

import dev.sjimo.ScopicFlow

Item {
    id: levelMeter

    readonly property color backgroundColor: SFPalette.levelMeterColor
    readonly property color borderColor: "transparent"
    property bool clipping: false
    readonly property color highColor: SFPalette.levelHighColor
    readonly property color lowColor: SFPalette.levelLowColor
    readonly property color middleColor: SFPalette.levelMediumColor
    property double minimum: -60
    property double safeThreshold: -18
    property double value: minimum
    property double warningThreshold: -6

    width: 4

    onValueChanged: {
        if (value > 0)
            clipping = true;
    }

    Rectangle {
        id: clippingIndicator

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        border.color: levelMeter.borderColor
        border.width: 1
        color: clipping ? levelMeter.highColor : levelMeter.backgroundColor
        height: 4
    }
    Rectangle {
        id: levelIndicatorContainer

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: clippingIndicator.bottom
        color: levelMeter.backgroundColor

        Rectangle {
            id: colorRange

            anchors.fill: parent

            gradient: Gradient {
                orientation: Gradient.Vertical

                GradientStop {
                    color: levelMeter.lowColor
                    position: 1
                }
                GradientStop {
                    color: levelMeter.lowColor
                    position: levelMeter.safeThreshold / levelMeter.minimum
                }
                GradientStop {
                    color: levelMeter.middleColor
                    position: levelMeter.warningThreshold / levelMeter.minimum
                }
                GradientStop {
                    color: levelMeter.highColor
                    position: 0
                }
            }
        }
        Rectangle {
            id: masque

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            color: levelMeter.backgroundColor
            height: Math.min(Math.max(0, levelMeter.value / levelMeter.minimum), 1) * parent.height
        }
        Rectangle {
            anchors.fill: parent
            border.color: levelMeter.borderColor
            border.width: 1
            color: "transparent"
        }
    }
}
