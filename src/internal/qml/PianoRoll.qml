import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: pianoRoll

    property QtObject animationViewModel: null
    property double bottomMargin: 0
    property QtObject clavierViewModel: null
    readonly property double keyHeight: clavierViewModel?.pixelDensity ?? 24
    property QtObject noteSequenceViewModel: null
    property QtObject pianoRollNoteAreaBehaviorViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    property double topMargin: 0
    property list<Component> viewItems: []
    readonly property rect viewport: Qt.rect(-(pianoRoll.timeViewModel?.start ?? 0) * (pianoRoll.timeLayoutViewModel?.pixelDensity ?? 0), pianoRoll.clavierViewModel ? Math.min(pianoRoll.topMargin, pianoRoll.height - (128 - pianoRoll.clavierViewModel.start) * pianoRoll.clavierViewModel.pixelDensity - pianoRoll.bottomMargin) : pianoRoll.topMargin, (pianoRoll.timeViewModel?.end ?? 0) * (pianoRoll.timeLayoutViewModel?.pixelDensity ?? 0), 128 * pianoRoll.keyHeight)

    function isBlackKey(key) {
        let indexInGroup = key % 12;
        return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10;
    }

    clip: true

    onViewItemsChanged: () => {
        for (let item of viewLayer.children) {
            item.destroy();
        }
        if (!viewItems)
            return;
        for (let component of viewItems) {
            let item = component.createObject(viewLayer, {
                viewport: pianoRoll.viewport
            });
            item.viewport = Qt.binding(() => pianoRoll.viewport);
        }
    }

    TimeManipulator {
        id: timeManipulator
        timeLayoutViewModel: pianoRoll.timeLayoutViewModel
        timeViewModel: pianoRoll.timeViewModel
    }
    ClavierManipulator {
        id: clavierManipulator
        clavierViewModel: pianoRoll.clavierViewModel
        topExpansion: pianoRoll.topMargin
        bottomExpansion: -pianoRoll.bottomMargin
    }
    Rectangle {
        id: background

        anchors.fill: parent
        color: SFPalette.editAreaPrimaryColor
    }
    Item {
        id: backgroundViewport

        anchors.left: parent.left
        anchors.right: parent.right
        height: pianoRoll.viewport.height
        y: pianoRoll.viewport.y

        Repeater {
            id: keyRepeater

            model: 128

            Rectangle {
                required property int index
                readonly property bool isBlackKey: pianoRoll.isBlackKey(index)

                anchors.left: parent.left
                border.color: SFPalette.editAreaPrimaryColor
                border.width: 1
                color: isBlackKey ? SFPalette.editAreaPrimaryColor : SFPalette.editAreaSecondaryColor
                height: pianoRoll.keyHeight
                width: parent.width
                y: (127 - index) * pianoRoll.keyHeight
            }
        }
    }
    PianoRollScale {
        anchors.fill: parent
        barScaleColor: SFPalette.scalePrimaryColor
        beatScaleColor: SFPalette.scaleSecondaryColor
        segmentScaleColor: SFPalette.scaleTertiaryColor
        timeLayoutViewModel: pianoRoll.timeLayoutViewModel
        timeViewModel: pianoRoll.timeViewModel
    }
    Item {
        id: viewLayer

        anchors.bottomMargin: pianoRoll.bottomMargin
        anchors.fill: parent
    }
    PositionIndicators {
        anchors.fill: parent
        playbackViewModel: pianoRoll.playbackViewModel
        timeLayoutViewModel: pianoRoll.timeLayoutViewModel
        timeViewModel: pianoRoll.timeViewModel
    }
    StyledScrollBar {
        id: verticalSlider

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6 + pianoRoll.bottomMargin
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: pianoRoll.topMargin
        orientation: Qt.Vertical
        position: 1 - (pianoRoll.clavierViewModel?.start ?? 0) / 128 - size
        size: (pianoRoll.height - pianoRoll.bottomMargin - pianoRoll.topMargin) / (backgroundViewport.height)

        onEndDragged: pos => {
            if (!pianoRoll.clavierViewModel)
                return;
            let newSize = pos - position;
            let newPixelDensity = (pianoRoll.height - pianoRoll.bottomMargin - pianoRoll.topMargin) / 128 / newSize;
            if (newPixelDensity <= pianoRoll.clavierViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.clavierViewModel.maximumPixelDensity)
                return;
            pianoRoll.clavierViewModel.start = (1 - pos) * 128;
            pianoRoll.clavierViewModel.pixelDensity = newPixelDensity;
        }
        onPositionChanged: {
            if (pianoRoll.clavierViewModel && Math.abs(pianoRoll.clavierViewModel.start - (1 - (position + size)) * 128) > Number.EPSILON * 100)
                pianoRoll.clavierViewModel.start = (1 - (position + size)) * 128;
        }
        onStartDragged: pos => {
            if (!pianoRoll.clavierViewModel)
                return;
            let newSize = position + size - pos;
            let newPixelDensity = (pianoRoll.height - pianoRoll.bottomMargin - pianoRoll.topMargin) / 128 / newSize;
            if (newPixelDensity <= pianoRoll.clavierViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.clavierViewModel.maximumPixelDensity)
                return;
            pianoRoll.clavierViewModel.pixelDensity = newPixelDensity;
        }
    }
    StyledScrollBar {
        id: horizontalSlider

        anchors.bottom: parent.bottom
        anchors.bottomMargin: pianoRoll.bottomMargin
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 6
        orientation: Qt.Horizontal
        position: pianoRoll.timeViewModel ? pianoRoll.timeViewModel.start / pianoRoll.timeViewModel.end : 0
        size: pianoRoll.timeViewModel && pianoRoll.timeLayoutViewModel ? pianoRoll.width / pianoRoll.timeLayoutViewModel.pixelDensity / pianoRoll.timeViewModel.end : 0

        onEndDragged: pos => {
            if (!pianoRoll.timeViewModel || !pianoRoll.timeLayoutViewModel)
                return;
            let newSize = pos - position;
            let newPixelDensity = pianoRoll.width / pianoRoll.timeViewModel.end / newSize;
            if (newPixelDensity <= pianoRoll.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.timeLayoutViewModel.maximumPixelDensity)
                return;
            pianoRoll.timeLayoutViewModel.pixelDensity = newPixelDensity;
        }
        onPositionChanged: {
            if (pianoRoll.timeViewModel && Math.abs(pianoRoll.timeViewModel.start - position * pianoRoll.timeViewModel.end) > 0.01)
                pianoRoll.timeViewModel.start = position * pianoRoll.timeViewModel.end;
        }
        onStartDragged: pos => {
            if (!pianoRoll.timeViewModel || !pianoRoll.timeLayoutViewModel)
                return;
            let newSize = position + size - pos;
            let newPixelDensity = pianoRoll.width / pianoRoll.timeViewModel.end / newSize;
            if (newPixelDensity <= pianoRoll.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= pianoRoll.timeLayoutViewModel.maximumPixelDensity)
                return;
            pianoRoll.timeViewModel.start = pianoRoll.timeViewModel.end * pos;
            pianoRoll.timeLayoutViewModel.pixelDensity = newPixelDensity;
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Horizontal | Qt.Vertical
        pinchZoomOrientationHint: Qt.Horizontal
        viewModel: pianoRoll.scrollBehaviorViewModel

        onMoved: function (x, y, isPhysicalWheel) {
            timeManipulator.moveViewBy(x, isPhysicalWheel);
            clavierManipulator.moveViewBy(y, isPhysicalWheel);
        }
        onZoomed: function (ratioX, ratioY, x, y, isPhysicalWheel) {
            timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel);
            clavierManipulator.zoomViewBy(ratioY, y, isPhysicalWheel);
        }
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: pianoRoll.scrollBehaviorViewModel

        onMoved: function (x, y) {
            timeManipulator.moveViewBy(x);
            clavierManipulator.moveViewBy(y);
        }
    }
}
