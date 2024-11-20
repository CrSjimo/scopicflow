import QtQuick

Item {
    id: levelMeter
    required property color lowColor
    required property color middleColor
    required property color highColor
    required property color backgroundColor
    required property color borderColor

    property double minimum: -60
    property double safeThreshold: -18
    property double warningThreshold: -6

    property double value: minimum

    property bool clipping: false

    width: 4

    Rectangle {
        id: clippingIndicator
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 4
        color: clipping ? levelMeter.highColor : levelMeter.backgroundColor
        border.width: 1
        border.color: levelMeter.borderColor
    }

    Rectangle {
        id: levelIndicatorContainer
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: clippingIndicator.bottom
        anchors.bottom: parent.bottom
        color: levelMeter.backgroundColor

        Rectangle {
            id: colorRange
            anchors.fill: parent
            gradient: Gradient {
                orientation: Gradient.Vertical
                GradientStop { position: 1; color: levelMeter.lowColor }
                GradientStop { position: levelMeter.safeThreshold / levelMeter.minimum; color: levelMeter.lowColor }
                GradientStop { position: levelMeter.warningThreshold / levelMeter.minimum; color: levelMeter.middleColor }
                GradientStop { position: 0; color: levelMeter.highColor }
            }
        }

        Rectangle {
            id: masque
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            height: Math.max(0, levelMeter.value / levelMeter.minimum * parent.height)
            color: levelMeter.backgroundColor
        }

        Rectangle {
            anchors.fill: parent
            color: "transparent"
            border.width: 1
            border.color: levelMeter.borderColor
        }

    }

    onValueChanged: {
        if (value > 0)
            clipping = true
    }

}