import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: clipPane

    property QtObject animationViewModel: null
    property Component clipGraph: null
    property QtObject clipPaneBehaviorViewModel: null
    property QtObject clipSequenceViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject trackListViewModel: null

    signal clipContextMenuRequired(model: QtObject)
    signal clipCut(model: QtObject, position: int)
    signal clipDoubleClicked(model: QtObject)
    signal contextMenuRequired(position: int, trackNumber: int)
    signal doubleClicked(position: int, trackNumber: int)

    clip: true

    TimeAlignmentPositionLocator {
        id: timeLocator

        anchors.fill: parent
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        timeViewModel: clipPane.timeViewModel
    }
    TimeManipulator {
        id: timeManipulator

        anchors.fill: parent
        animationViewModel: clipPane.animationViewModel
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        timeViewModel: clipPane.timeViewModel
    }
    TrackListLocator {
        id: trackListLocator

        trackListViewModel: clipPane.trackListViewModel
    }
    TrackListManipulator {
        id: trackListManipulator

        anchors.fill: parent
        animationViewModel: clipPane.animationViewModel
        trackListLayoutViewModel: clipPane.trackListLayoutViewModel
        viewportHeight: trackListLocator.viewportHeight
    }
    SelectableViewModelManipulator {
        id: selectionManipulator

        viewModel: clipPane.clipSequenceViewModel
    }
    Rectangle {
        id: background

        anchors.fill: parent
        color: SFPalette.editAreaPrimaryColor
    }
    PianoRollScale {
        anchors.fill: parent
        barScaleColor: SFPalette.scalePrimaryColor
        beatScaleColor: SFPalette.scaleSecondaryColor
        segmentScaleColor: SFPalette.scaleTertiaryColor
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        timeViewModel: clipPane.timeViewModel
    }
    Item {
        id: trackSplitters

        anchors.left: parent.left
        anchors.right: parent.right
        y: -(clipPane.trackListLayoutViewModel?.viewportOffset ?? 0)

        Repeater {
            id: trackSplitterssRepeater

            model: (clipPane.trackListViewModel?.handle.count ?? 0) + 1

            Rectangle {
                id: trackSplitter

                required property int index
                property QtObject trackViewModel: index !== 0 ? clipPane.trackListViewModel.handle.items[index - 1] : null

                anchors.left: parent.left
                anchors.right: parent.right
                color: Theme.splitterColor
                height: 2
                y: (trackListLocator.map[index] ?? 0) - height / 2
            }
        }
    }
    Item {
        id: viewport

        height: trackListLocator.viewportHeight
        width: (clipPane.timeViewModel?.end ?? 0) * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
        x: -(clipPane.timeViewModel?.start ?? 0) * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
        y: -(clipPane.trackListLayoutViewModel?.viewportOffset ?? 0)

        MouseArea {
            id: backRightButtonMouseArea

            acceptedButtons: Qt.RightButton
            anchors.fill: parent
            visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior !== ScopicFlow.MB_None

            onClicked: mouse => {
                selectionManipulator.select(null, mouse.button, mouse.modifiers);
                let parentPoint = mapToItem(clipPane, mouse.x, mouse.y);
                clipPane.contextMenuRequired(timeLocator.mapToTick(parentPoint.x), trackListLocator.mapToIndex(mouse.y));
            }
        }
        GenericBackPointerMouseArea {
            id: backPointerMouseArea

            paneItem: clipPane
            verticalManipulator: trackListManipulator
            visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pointer

            onDoubleClicked: mouse => {
                let parentPoint = mapToItem(clipPane, mouse.x, mouse.y);
                clipPane.doubleClicked(timeLocator.mapToTick(parentPoint.x), trackListLocator.mapToIndex(mouse.y));
            }
            onRubberBandStartRequired: p => {
                rubberBandLayer.startSelection(p);
            }
            onRubberBandUpdateRequired: p => {
                rubberBandLayer.updateSelection(p);
            }
        }
        GenericBackPenMouseArea {
            id: backPenMouseArea

            lengthHint: clipPane.clipPaneBehaviorViewModel?.lengthHint ?? 0
            mapY: y => trackListLocator.mapToIndex(y)
            mappedYProperty: "trackNumber"
            paneItem: clipPane
            sequenceViewModel: clipPane.clipSequenceViewModel
            visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pen

            viewModelComponent: ClipViewModel {
            }

            onClicked: mouse => {
                if (dragged)
                    return;
            }
        }
        Item {
            id: clipContainer

            property double incrementZCounter: 0

            anchors.fill: parent

            SequenceSlicer {
                model: clipPane.clipSequenceViewModel
                sliceWidth: clipPane.width
                timeLayoutViewModel: clipPane.timeLayoutViewModel
                timeViewModel: clipPane.timeViewModel

                delegate: Item {
                    id: clipRect

                    property color clipColor: {
                        clipColor = clipPane.trackListViewModel?.handle.items[model.trackNumber].color ?? "white";
                    }
                    property bool current: {
                        current = clipPane.clipSequenceViewModel.handle.currentItem === model;
                    }
                    required property QtObject model

                    function bringToFront() {
                        if (model.overlapped)
                            z = ++clipContainer.incrementZCounter;
                    }

                    opacity: eraserMouseArea.willBeErased ? 0.5 : 1

                    Binding {
                        clipNameLabel.visible: clipNameLabel.x + clipNameLabel.width <= clipRect.width
                        clipNameLabel.x: Math.max(-(clipRect.x + viewport.x), 0)
                        clipRect.clipColor: clipPane.trackListViewModel?.handle.items[model.trackNumber].color ?? "white"
                        clipRect.current: clipPane.clipSequenceViewModel?.handle.currentItem === model
                        clipRect.height: clipPane.trackListViewModel?.handle.items[model.trackNumber].rowHeight ?? 0
                        clipRect.width: clipRect.model.length * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipRect.x: clipRect.model.position * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipRect.y: trackListLocator.map[model.trackNumber] ?? 0
                        when: clipRect.visible
                    }
                    Rectangle {
                        id: clipBackground

                        anchors.bottomMargin: 1
                        anchors.fill: parent
                        anchors.topMargin: 1
                        color: clipRect.model.selected ? SFPalette.clipSelectedColorChange.apply(SFPalette.clipThumbnailColorChange.apply(clipRect.clipColor)) : SFPalette.clipThumbnailColorChange.apply(clipRect.clipColor)
                        radius: 4

                        Behavior on color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                    Rectangle {
                        id: clipHeader

                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: parent.top
                        color: clipRect.clipColor
                        height: 20
                        radius: clipBackground.radius

                        Behavior on color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }

                        Rectangle {
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.right: parent.right
                            color: parent.color
                            height: parent.radius
                        }
                        Text {
                            id: clipNameLabel

                            anchors.verticalCenter: parent.verticalCenter
                            color: SFPalette.suitableForegroundColor(clipRect.clipColor)
                            leftPadding: 4
                            rightPadding: 16
                            text: clipRect.model.name

                            Behavior on color {
                                ColorAnimation {
                                    duration: Theme.colorAnimationDuration
                                    easing.type: Easing.OutCubic
                                }
                            }
                        }
                    }
                    Connections {
                        function onClipGraphChanged() {
                            clipGraphContainer.load();
                        }

                        target: clipPane
                    }
                    Item {
                        id: clipGraphContainer

                        function load() {
                            for (let item of clipGraphContainer.children) {
                                item.destroy();
                            }
                            if (!clipPane.clipGraph)
                                return;
                            clipPane.clipGraph.createObject(clipGraphContainer, {
                                model: clipRect.model,
                                color: SFPalette.suitableForegroundColor(SFPalette.clipThumbnailColorChange.apply(clipRect.clipColor))
                            });
                        }

                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.top: clipHeader.bottom

                        Component.onCompleted: load()
                    }
                    Rectangle {
                        id: border

                        anchors.fill: clipBackground
                        border.color: clipRect.clipColor
                        border.width: Math.min(clipRect.model.selected ? 2 : 1, width / 4)
                        color: "transparent"
                        opacity: clipRect.model.selected ? 1 : 0.5
                        radius: clipBackground.radius

                        Behavior on border.color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                    Connections {
                        id: cursorIndicatorLeftBinding

                        function onPositionChanged() {
                            clipPane.timeLayoutViewModel.cursorPosition = clipRect.model.position;
                        }

                        enabled: false
                        target: clipRect.model
                    }
                    Connections {
                        id: cursorIndicatorRightBinding

                        function onLengthChanged() {
                            clipPane.timeLayoutViewModel.cursorPosition = clipRect.model.position + clipRect.model.length;
                        }
                        function onPositionChanged() {
                            clipPane.timeLayoutViewModel.cursorPosition = clipRect.model.position + clipRect.model.length;
                        }

                        enabled: false
                        target: clipRect.model
                    }
                    MouseArea {
                        id: rightButtonMouseArea

                        acceptedButtons: Qt.RightButton
                        anchors.fill: parent
                        visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior !== ScopicFlow.MB_None

                        onClicked: mouse => {
                            selectionManipulator.select(model, mouse.button, mouse.modifiers);
                            clipPane.clipContextMenuRequired(model);
                        }
                    }
                    GenericPointerMouseArea {
                        id: pointerMouseArea

                        property double mappingOffset: 0

                        model: clipRect.model
                        paneItem: clipPane
                        sequenceViewModel: clipPane.clipSequenceViewModel
                        verticalManipulator: trackListManipulator
                        visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pointer || clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Pen

                        onDoubleClicked: mouse => {
                            clipPane.clipSequenceViewModel.handle.currentItem = model;
                            clipPane.clipDoubleClicked(model);
                        }
                        onDraggingChanged: {
                            if (dragging) {
                                cursorIndicatorLeftBinding.enabled = true;
                                cursorIndicatorLeftBinding.onPositionChanged();
                            } else {
                                cursorIndicatorLeftBinding.enabled = false;
                                clipPane.timeLayoutViewModel.cursorPosition = -1;
                            }
                        }
                        onMoveSelectedNotesToY: y => {
                            let trackCount = clipPane.trackListViewModel.handle.items.length;
                            let targetIndex = trackListLocator.mapToIndex(y + clipPane.trackListLayoutViewModel.viewportOffset + mappingOffset);
                            if (targetIndex !== model.trackNumber) {
                                let deltaIndex = targetIndex - model.trackNumber;
                                for (let clip of sequenceViewModel.handle.selection) {
                                    if (clip.trackNumber + deltaIndex < 0 || clip.trackNumber + deltaIndex >= trackCount)
                                        return;
                                }
                                for (let clip of sequenceViewModel.handle.selection) {
                                    clip.trackNumber += deltaIndex;
                                }
                            }
                        }
                        onPressedChanged: () => {
                            if (pressed) {
                                clipRect.bringToFront();
                                mappingOffset = 0.5 * clipPane.trackListViewModel.handle.items[model.trackNumber].rowHeight;
                            }
                        }
                    }
                    Repeater {
                        model: 2

                        GenericEdgeMouseArea {
                            id: edgeMouseArea

                            required property int index

                            leftEdge: index
                            model: clipRect.model
                            paneItem: clipPane
                            sequenceViewModel: clipPane.clipSequenceViewModel
                            unitedExtend: false
                            unitedExtendEnabled: false
                            visible: pointerMouseArea.visible

                            onClicked: mouse => {
                                if (dragged)
                                    return;
                                pointerMouseArea.clicked(mouse);
                            }
                            onDoubleClicked: mouse => {
                                pointerMouseArea.doubleClicked(mouse);
                            }
                            onDraggingChanged: () => {
                                let binding = leftEdge ? cursorIndicatorLeftBinding : cursorIndicatorRightBinding;
                                if (dragging) {
                                    binding.enabled = true;
                                    binding.onPositionChanged();
                                } else {
                                    binding.enabled = false;
                                    clipPane.timeLayoutViewModel.cursorPosition = -1;
                                }
                            }
                            onPressedChanged: () => {
                                if (pressed)
                                    clipRect.bringToFront();
                            }
                        }
                    }
                    GenericScissorMouseArea {
                        id: scissorMouseArea

                        model: clipRect.model
                        paneItem: clipPane
                        visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Scissor

                        onCutPositionChanged: () => {
                            clipPane.timeLayoutViewModel.cursorPosition = cutPosition;
                        }
                        onPressedChanged: () => {
                            if (pressed)
                                clipRect.bringToFront();
                        }
                        onReleased: mouse => {
                            if (cutPosition !== -1)
                                clipPane.clipCut(model, cutPosition);
                        }
                    }
                    GenericEraserMouseArea {
                        id: eraserMouseArea

                        visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ScopicFlow.MB_Eraser

                        onReleased: mouse => {
                            if (willBeErased)
                                clipPane.clipSequenceViewModel.handle.removeItem(clipRect.model);
                        }
                    }
                }
            }
        }
        RubberBandLayer {
            id: rubberBandLayer

            anchors.fill: parent
            selectionManipulator: selectionManipulator

            rubberBand: RubberBandRectangle {
            }
        }
    }
    PositionIndicators {
        anchors.fill: parent
        playbackViewModel: clipPane.playbackViewModel
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        timeViewModel: clipPane.timeViewModel
    }
    StyledScrollBar {
        id: verticalSlider

        allowDragAdjustment: false
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        anchors.right: parent.right
        anchors.top: parent.top
        orientation: Qt.Vertical
        position: (clipPane.trackListLayoutViewModel?.viewportOffset ?? 0) / trackListLocator.viewportHeight
        size: clipPane.height / trackListLocator.viewportHeight

        onPositionChanged: {
            if (clipPane.trackListLayoutViewModel && Math.abs(clipPane.trackListLayoutViewModel.viewportOffset - position * trackListLocator.viewportHeight) > Number.EPSILON * 1000)
                clipPane.trackListLayoutViewModel.viewportOffset = position * trackListLocator.viewportHeight;
        }
    }
    StyledScrollBar {
        id: horizontalSlider

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 6
        orientation: Qt.Horizontal
        position: clipPane.timeViewModel ? clipPane.timeViewModel.start / clipPane.timeViewModel.end : 0
        size: clipPane.timeViewModel && clipPane.timeLayoutViewModel ? clipPane.width / clipPane.timeLayoutViewModel.pixelDensity / clipPane.timeViewModel.end : 0

        onEndDragged: pos => {
            if (!clipPane.timeViewModel || !clipPane.timeLayoutViewModel)
                return;
            let newSize = pos - position;
            let newPixelDensity = clipPane.width / clipPane.timeViewModel.end / newSize;
            if (newPixelDensity <= clipPane.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= clipPane.timeLayoutViewModel.maximumPixelDensity)
                return;
            clipPane.timeLayoutViewModel.pixelDensity = newPixelDensity;
        }
        onPositionChanged: {
            if (clipPane.timeViewModel && Math.abs(clipPane.timeViewModel.start - position * clipPane.timeViewModel.end) > 0.01)
                clipPane.timeViewModel.start = position * clipPane.timeViewModel.end;
        }
        onStartDragged: pos => {
            if (!clipPane.timeViewModel || !clipPane.timeLayoutViewModel)
                return;
            let newSize = position + size - pos;
            let newPixelDensity = clipPane.width / clipPane.timeViewModel.end / newSize;
            if (newPixelDensity <= clipPane.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= clipPane.timeLayoutViewModel.maximumPixelDensity)
                return;
            clipPane.timeViewModel.start = clipPane.timeViewModel.end * pos;
            clipPane.timeLayoutViewModel.pixelDensity = newPixelDensity;
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        viewModel: clipPane.scrollBehaviorViewModel
        zoomableOrientation: Qt.Horizontal

        onMoved: function (x, y, isPhysicalWheel) {
            timeManipulator.moveViewBy(x, isPhysicalWheel);
            trackListManipulator.moveViewBy(y, isPhysicalWheel);
        }
        onZoomed: function (ratioX, _, x, _, isPhysicalWheel) {
            timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel);
        }
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: clipPane.scrollBehaviorViewModel

        onMoved: function (x, y) {
            timeManipulator.moveViewBy(x);
            trackListManipulator.moveViewBy(y);
        }
    }
}
