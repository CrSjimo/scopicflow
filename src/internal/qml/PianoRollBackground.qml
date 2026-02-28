import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: pianoRollBackground

    property ClavierViewModel clavierViewModel: null
    property TimeViewModel timeViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property RangeSequenceViewModel scaleHighlightSequenceViewModel: null

    clip: true

    QtObject {
        id: helper
        readonly property double keyHeight: pianoRollBackground.clavierViewModel?.pixelDensity ?? 24
        function isBlackKey(key) {
            let indexInGroup = key % 12;
            return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10;
        }
    }

    ClavierViewportContainer {
        id: clavierViewportContainer
        clavierViewModel: pianoRollBackground.clavierViewModel
        Item {
            id: backgroundPattern
            Repeater {
                id: backgroundPatternRepeater
                model: 128
                delegate: Rectangle {
                    required property int index
                    x: 0
                    y: 127 - index
                    width: 1
                    height: 1
                    color: helper.isBlackKey(index) ? SFPalette.editAreaPrimaryColor : SFPalette.editAreaSecondaryColor
                }
            }
            transform: Scale {
                origin.x: 0
                origin.y: 0
                xScale: pianoRollBackground.width
                yScale: helper.keyHeight
            }
        }
        TimeViewportContainer {
            id: timeViewportContainer
            timeViewModel: pianoRollBackground.timeViewModel
            timeLayoutViewModel: pianoRollBackground.timeLayoutViewModel
            SequenceSlicer {
                id: slicer
                viewModel: pianoRollBackground.scaleHighlightSequenceViewModel
                sliceWidth: pianoRollBackground.width
                timeLayoutViewModel: pianoRollBackground.timeLayoutViewModel
                timeViewModel: pianoRollBackground.timeViewModel
                delegate: Item {
                    id: highlightPattern
                    Binding {
                        highlightPattern.x: (highlightPattern.SequenceSlicerLoader.viewModel?.position ?? 0) * (pianoRollBackground.timeLayoutViewModel?.pixelDensity ?? 0)
                        highlightPattern.displayWidth: (highlightPattern.SequenceSlicerLoader.viewModel?.length ?? 0) * (pianoRollBackground.timeLayoutViewModel?.pixelDensity ?? 0)
                        when: highlightPattern.SequenceSlicerLoader.inRange
                    }
                    property double displayWidth: 0
                    Repeater {
                        model: 128
                        delegate: Rectangle {
                            required property int index
                            x: 0
                            y: 127 - index
                            width: 1
                            height: 1
                            color: helper.isBlackKey(index) ? SFPalette.editAreaPrimaryHighlightColor : SFPalette.editAreaSecondaryHighlightColor
                            visible: Boolean((1 << index % 12) & (highlightPattern.SequenceSlicerLoader.viewModel?.cMask ?? 0))
                        }
                    }
                    transform: Scale {
                        origin.x: 0
                        origin.y: 0
                        xScale: highlightPattern.displayWidth
                        yScale: helper.keyHeight
                    }
                }
            }
        }
        Repeater {
            id: separatorRepeater
            model: 24
            delegate: Rectangle {
                required property int index
                x: 0
                y: (3 - index % 2 + index * 6) * helper.keyHeight - 0.5
                width: pianoRollBackground.width
                height: 1
                color: SFPalette.scaleSecondaryColor
            }
        }
    }

    PianoRollScale {
        anchors.fill: parent
        barScaleColor: SFPalette.scalePrimaryColor
        beatScaleColor: SFPalette.scaleSecondaryColor
        segmentScaleColor: SFPalette.scaleTertiaryColor
        timeLayoutViewModel: pianoRollBackground.timeLayoutViewModel
        timeViewModel: pianoRollBackground.timeViewModel
    }
}