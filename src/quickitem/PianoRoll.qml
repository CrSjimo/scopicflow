import ScopicFlowPrivate
import QtQml
import QtQuick

import "./HelperComponents"
import "../palette" as ScopicFlowPalette

PianoRoll {
    id: pianoRoll
    property QtObject defaultPalette: ScopicFlowPalette.PianoRoll {}
    property QtObject palette: paletteViewModel?.palette?.pianoRoll ?? defaultPalette
    property double keyHeight: clavierViewModel?.pixelDensity ?? 24

    function isBlackKey(key) {
        let indexInGroup = key % 12;
        return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10
    }

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: pianoRoll.timeAlignmentViewModel
        animationViewModel: pianoRoll.animationViewModel
    }

    ClavierManipulator {
        id: clavierManipulator
        anchors.fill: parent
        clavierViewModel: pianoRoll.clavierViewModel
        animationViewModel: pianoRoll.animationViewModel
    }

    Item {
        anchors.fill: parent

        Item {
            id: backgroundViewport
            anchors.left: parent.left
            anchors.right: parent.right
            height: 128 * pianoRoll.keyHeight
            y: pianoRoll.clavierViewModel ? Math.min(0, pianoRoll.height - (128 - pianoRoll.clavierViewModel.start) * pianoRoll.clavierViewModel.pixelDensity) : 0

            Repeater {
                id: keyRepeater
                model: 128
                Rectangle {
                    required property int index
                    readonly property bool isBlackKey: pianoRoll.isBlackKey(index)
                    anchors.left: parent.left
                    width: parent.width
                    height: pianoRoll.keyHeight
                    visible: y + height >= -backgroundViewport.y && y <= 128 * pianoRoll.height - backgroundViewport.y
                    y: (127 - index) * pianoRoll.keyHeight
                    color: isBlackKey ? pianoRoll.palette.blackKeyBackgroundColor : pianoRoll.palette.whiteKeyBackgroundColor
                    border.width: 1
                    border.color: pianoRoll.palette.blackKeyBackgroundColor
                }
            }

        }

        PianoRollBackground {
            anchors.fill: parent
            timeAlignmentViewModel: pianoRoll.timeAlignmentViewModel
            barScaleColor: palette.barScaleColor
            beatScaleColor: palette.beatScaleColor
            segmentScaleColor: palette.segmentScaleColor
        }
    }

    Rectangle {
        id: secondaryIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: pianoRoll.palette.secondaryIndicatorColor
        x: pianoRoll.timeAlignmentViewModel && pianoRoll.playbackViewModel ? (pianoRoll.playbackViewModel.secondaryPositon - pianoRoll.timeAlignmentViewModel.start) * pianoRoll.timeAlignmentViewModel.pixelDensity - 0.5 : 0
    }

    Rectangle {
        id: primaryIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: pianoRoll.palette.primaryIndicatorColor
        x: pianoRoll.timeAlignmentViewModel && pianoRoll.playbackViewModel ? (pianoRoll.playbackViewModel.primaryPosition - pianoRoll.timeAlignmentViewModel.start) * pianoRoll.timeAlignmentViewModel.pixelDensity - 0.5 : 0
    }

    StyledScrollBar {
        id: verticalSlider
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        anchors.right: parent.right
        orientation: Qt.Vertical
        normalColor: pianoRoll.palette.scrollBarNormalColor
        pressedColor: pianoRoll.palette.scrollBarPressedColor
        hoveredColor: pianoRoll.palette.scrollBarHoveredColor
        animationRatio: pianoRoll.animationViewModel?.visualEffectAnimationRatio ?? 1.0
        size: pianoRoll.height / backgroundViewport.height
        position: 1 - (pianoRoll.clavierViewModel?.start ?? 0) / 128 - size
        onPositionChanged: {
            if (pianoRoll.clavierViewModel && Math.abs(pianoRoll.clavierViewModel.start - (1 - (position + size)) * 128) > Number.EPSILON * 100)
                pianoRoll.clavierViewModel.start = (1 - (position + size)) * 128
        }
    }

    StyledScrollBar {
        id: horizontalSlider
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        orientation: Qt.Horizontal
        normalColor: pianoRoll.palette.scrollBarNormalColor
        pressedColor: pianoRoll.palette.scrollBarPressedColor
        hoveredColor: pianoRoll.palette.scrollBarHoveredColor
        animationRatio: pianoRoll.animationViewModel?.visualEffectAnimationRatio ?? 1.0
        size: pianoRoll.timeAlignmentViewModel ? pianoRoll.width / pianoRoll.timeAlignmentViewModel.pixelDensity / pianoRoll.timeAlignmentViewModel.end : 0
        position: pianoRoll.timeAlignmentViewModel ? pianoRoll.timeAlignmentViewModel.start / pianoRoll.timeAlignmentViewModel.end : 0
        onPositionChanged: {
            if (pianoRoll.timeAlignmentViewModel && Math.abs(pianoRoll.timeAlignmentViewModel.start - position * pianoRoll.timeAlignmentViewModel.end) > Number.EPSILON * 100)
                pianoRoll.timeAlignmentViewModel.start = position * pianoRoll.timeAlignmentViewModel.end
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: pianoRoll.scrollBehaviorViewModel
        onZoomed: function (ratioX, ratioY, x, y, isPhysicalWheel) {
            timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel)
            clavierManipulator.zoomOnWheel(ratioY, y, isPhysicalWheel)
        }
        onMoved: function (x, y, isPhysicalWheel) {
            timeManipulator.moveViewBy(x, isPhysicalWheel)
            clavierManipulator.moveViewBy(y, isPhysicalWheel)
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: pianoRoll.scrollBehaviorViewModel
        onMoved: function (x, y) {
            timeManipulator.moveViewBy(x)
            clavierManipulator.moveViewBy(y)
        }
    }
}