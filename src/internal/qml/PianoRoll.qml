import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Palette as ScopicFlowPalette

Item {
    id: pianoRoll

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject noteSequenceViewModel: null
    property QtObject paletteViewModel: null

    property double topMargin: 0
    property double bottomMargin: 0

    property QtObject defaultPalette: ScopicFlowPalette.PianoRoll {}
    property QtObject palette: paletteViewModel?.palette?.pianoRoll ?? defaultPalette

    readonly property double keyHeight: clavierViewModel?.pixelDensity ?? 24

    Item {
        id: viewport
        x: -(pianoRoll.timeViewModel?.start ?? 0) * (pianoRoll.timeLayoutViewModel?.pixelDensity ?? 0)
        y: pianoRoll.clavierViewModel ? Math.min(pianoRoll.topMargin, pianoRoll.height - (128 - pianoRoll.clavierViewModel.start) * pianoRoll.clavierViewModel.pixelDensity - pianoRoll.bottomMargin) : pianoRoll.topMargin
        width: (pianoRoll.timeViewModel?.end ?? 0) * (pianoRoll.timeLayoutViewModel?.pixelDensity ?? 0)
        height: 128 * pianoRoll.keyHeight
    }

    clip: true

    function isBlackKey(key) {
        let indexInGroup = key % 12
        return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10
    }

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: pianoRoll.timeViewModel
        timeLayoutViewModel: pianoRoll.timeLayoutViewModel
        animationViewModel: pianoRoll.animationViewModel
    }

    ClavierManipulator {
        id: clavierManipulator
        anchors.fill: parent
        anchors.topMargin: pianoRoll.topMargin
        startOffset: -pianoRoll.bottomMargin
        clavierViewModel: pianoRoll.clavierViewModel
        animationViewModel: pianoRoll.animationViewModel
    }

    Rectangle {
        id: background
        anchors.fill: parent
        color: pianoRoll.palette.blackKeyBackgroundColor
    }

    Item {
        id: backgroundViewport
        anchors.fill: viewport

        Repeater {
            id: keyRepeater
            model: 128
            Rectangle {
                required property int index
                readonly property bool isBlackKey: pianoRoll.isBlackKey(index)
                anchors.left: parent.left
                width: parent.width
                height: pianoRoll.keyHeight
                y: (127 - index) * pianoRoll.keyHeight
                color: isBlackKey ? pianoRoll.palette.blackKeyBackgroundColor : pianoRoll.palette.whiteKeyBackgroundColor
                border.width: 1
                border.color: pianoRoll.palette.blackKeyBackgroundColor
            }
        }

    }

    PianoRollScale {
        anchors.fill: parent
        timeViewModel: pianoRoll.timeViewModel
        timeLayoutViewModel: pianoRoll.timeLayoutViewModel
        barScaleColor: pianoRoll.palette.barScaleColor
        beatScaleColor: pianoRoll.palette.beatScaleColor
        segmentScaleColor: pianoRoll.palette.segmentScaleColor
    }


    Item {
        id: noteAreaViewport
        anchors.fill: viewport

        PianoRollNoteArea {
            anchors.fill: parent
            timeViewModel: pianoRoll.timeViewModel
            timeLayoutViewModel: pianoRoll.timeLayoutViewModel
            clavierViewModel: pianoRoll.clavierViewModel
            animationViewModel: pianoRoll.animationViewModel
            noteSequenceViewModel: pianoRoll.noteSequenceViewModel
            sliceWidth: pianoRoll.width
        }
    }

    PositionIndicators {
        anchors.fill: parent
        palette: pianoRoll.palette
        timeViewModel: pianoRoll.timeViewModel
        timeLayoutViewModel: pianoRoll.timeLayoutViewModel
        playbackViewModel: pianoRoll.playbackViewModel
    }

    StyledScrollBar {
        id: verticalSlider
        anchors.top: parent.top
        anchors.topMargin: pianoRoll.topMargin
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6 + pianoRoll.bottomMargin
        anchors.right: parent.right
        orientation: Qt.Vertical
        normalColor: pianoRoll.palette.scrollBarNormalColor
        pressedColor: pianoRoll.palette.scrollBarPressedColor
        hoveredColor: pianoRoll.palette.scrollBarHoveredColor
        animationViewModel: pianoRoll.animationViewModel
        size: (pianoRoll.height - pianoRoll.bottomMargin - pianoRoll.topMargin) / (backgroundViewport.height)
        position: 1 - (pianoRoll.clavierViewModel?.start ?? 0) / 128 - size
        onPositionChanged: {
            if (pianoRoll.clavierViewModel && Math.abs(pianoRoll.clavierViewModel.start - (1 - (position + size)) * 128) > Number.EPSILON * 100)
                pianoRoll.clavierViewModel.start = (1 - (position + size)) * 128
        }
        onStartDragged: (pos) => {
            if (!pianoRoll.clavierViewModel)
                return
            let newSize = position + size - pos
            let newPixelDensity = (pianoRoll.height - pianoRoll.bottomMargin - pianoRoll.topMargin) / 128 / newSize
            if (newPixelDensity <= pianoRoll.clavierViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.clavierViewModel.maximumPixelDensity)
                return
            pianoRoll.clavierViewModel.pixelDensity = newPixelDensity
        }
        onEndDragged: (pos) => {
            if (!pianoRoll.clavierViewModel)
                return
            let newSize = pos - position
            let newPixelDensity = (pianoRoll.height - pianoRoll.bottomMargin - pianoRoll.topMargin) / 128 / newSize
            if (newPixelDensity <= pianoRoll.clavierViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.clavierViewModel.maximumPixelDensity)
                return
            pianoRoll.clavierViewModel.start = (1 - pos) * 128
            pianoRoll.clavierViewModel.pixelDensity = newPixelDensity
        }
    }

    StyledScrollBar {
        id: horizontalSlider
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        anchors.bottomMargin: pianoRoll.bottomMargin
        orientation: Qt.Horizontal
        normalColor: pianoRoll.palette.scrollBarNormalColor
        pressedColor: pianoRoll.palette.scrollBarPressedColor
        hoveredColor: pianoRoll.palette.scrollBarHoveredColor
        animationViewModel: pianoRoll.animationViewModel
        size: pianoRoll.timeViewModel && pianoRoll.timeLayoutViewModel ? pianoRoll.width / pianoRoll.timeLayoutViewModel.pixelDensity / pianoRoll.timeViewModel.end : 0
        position: pianoRoll.timeViewModel ? pianoRoll.timeViewModel.start / pianoRoll.timeViewModel.end : 0
        onPositionChanged: {
            if (pianoRoll.timeViewModel && Math.abs(pianoRoll.timeViewModel.start - position * pianoRoll.timeViewModel.end) > Number.EPSILON * 100)
                pianoRoll.timeViewModel.start = position * pianoRoll.timeViewModel.end
        }
        onStartDragged: (pos) => {
            if (!pianoRoll.timeViewModel || !pianoRoll.timeLayoutViewModel)
                return
            let newSize = position + size - pos
            let newPixelDensity = pianoRoll.width / pianoRoll.timeViewModel.end / newSize
            if (newPixelDensity <= pianoRoll.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.timeLayoutViewModel.maximumPixelDensity)
                return
            pianoRoll.timeViewModel.start = pianoRoll.timeViewModel.end * pos
            pianoRoll.timeLayoutViewModel.pixelDensity = newPixelDensity
        }
        onEndDragged: (pos) => {
            if (!pianoRoll.timeViewModel || !pianoRoll.timeLayoutViewModel)
                return
            let newSize = pos - position
            let newPixelDensity = pianoRoll.width / pianoRoll.timeViewModel.end / newSize
            if (newPixelDensity <= pianoRoll.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.timeLayoutViewModel.maximumPixelDensity)
                return
            pianoRoll.timeLayoutViewModel.pixelDensity = newPixelDensity
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