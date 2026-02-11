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

    clip: true

    QtObject {
        id: helper
        readonly property double keyHeight: clavier.clavierViewModel?.pixelDensity ?? 24
        function isBlackKey(key) {
            let indexInGroup = key % 12;
            return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10;
        }
    }

    ClavierViewportContainer {
        id: viewportContainer
        clavierViewModel: pianoRollBackground.clavierViewModel
        Repeater {
            id: repeater
            model: 128
            delegate: Rectangle {
                required property int index
                x: -1
                y: (127 - index) * helper.keyHeight
                width: parent.width + 2
                height: helper.keyHeight
                color: helper.isBlackKey(index) ? SFPalette.editAreaPrimaryColor : SFPalette.editAreaSecondaryColor
                border.color: SFPalette.editAreaPrimaryColor
                border.width: 1
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