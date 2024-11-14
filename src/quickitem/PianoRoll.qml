import ScopicFlowPrivate
import QtQml
import QtQuick

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

    Item {
        anchors.fill: parent

        Item {
            id: backgroundViewport
            anchors.left: parent.left
            anchors.right: parent.right
            height: 128 * pianoRoll.keyHeight
            y: pianoRoll.clavierViewModel ? 0 : height - (128 - pianoRoll.clavierViewModel.start) * pianoRoll.clavierViewModel.pixelDensity

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
}