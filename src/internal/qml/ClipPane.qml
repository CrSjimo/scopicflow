import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: clipPane

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject trackListViewModel: null
    property QtObject trackListLayoutViewModel: null
    property QtObject clipSequenceViewModel: null
    property QtObject clipPaneBehaviorViewModel: null
    
    property bool active: false

    property QtObject stylesheet: ClipPaneStylesheet {}
    
    readonly property QtObject clipPaneStyleItem: stylesheet.clipPane.createObject(clipPane, {active})
    readonly property QtObject scrollBarStyleItem: stylesheet.scrollBar.createObject(clipPane)
    readonly property QtObject timeIndicatorsStyleItem: stylesheet.timeIndicators.createObject(clipPane)
    readonly property QtObject rubberBandStyleItem: stylesheet.rubberBand.createObject(clipPane)

    clip: true

    signal clipCut(model: QtObject, position: int)
    signal doubleClicked(position: int, trackNumber: int)

    TimeAlignmentPositionLocator {
        id: timeLocator
        anchors.fill: parent
        timeViewModel: clipPane.timeViewModel
        timeLayoutViewModel: clipPane.timeLayoutViewModel
    }

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: clipPane.timeViewModel
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        animationViewModel: clipPane.animationViewModel
    }

    TrackListLocator {
        id: trackListLocator
        trackListViewModel: clipPane.trackListViewModel
    }

    TrackListManipulator {
        id: trackListManipulator
        anchors.fill: parent
        trackListLayoutViewModel: clipPane.trackListLayoutViewModel
        animationViewModel: clipPane.animationViewModel
        viewportHeight: trackListLocator.viewportHeight
    }

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: clipPane.clipSequenceViewModel
    }
    
    Rectangle {
        id: background
        anchors.fill: parent
        color: clipPane.clipPaneStyleItem.background
    }

    PianoRollScale {
        anchors.fill: parent
        timeViewModel: clipPane.timeViewModel
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        barScaleColor: clipPane.clipPaneStyleItem.barScale
        beatScaleColor: clipPane.clipPaneStyleItem.beatScale
        segmentScaleColor: clipPane.clipPaneStyleItem.segmentScale
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
                y: (trackListLocator.map[index] ?? 0) - height / 2
                height: 2
                color: clipPane.clipPaneStyleItem.trackSplitter
            }
        }
    }

    Item {
        id: viewport
        x: -(clipPane.timeViewModel?.start ?? 0) * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
        y: -(clipPane.trackListLayoutViewModel?.viewportOffset ?? 0)
        width: (clipPane.timeViewModel?.end ?? 0) * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
        height: trackListLocator.viewportHeight

        GenericBackPointerMouseArea {
            id: backPointerMouseArea
            visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ClipPaneBehaviorViewModel.Pointer
            paneItem: clipPane
            verticalManipulator: trackListManipulator

            onRubberBandStartRequired: (p) => {
                rubberBandLayer.startSelection(p)
            }
            onRubberBandUpdateRequired: (p) => {
                rubberBandLayer.updateSelection(p)
            }

            onDoubleClicked: (mouse) => {
                let parentPoint = mapToItem(clipPane, mouse.x, mouse.y);
                clipPane.doubleClicked(timeLocator.mapToTick(parentPoint.x), trackListLocator.mapToIndex(mouse.y))
            }
        }
        GenericBackPenMouseArea {
            id: backPenMouseArea
            visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ClipPaneBehaviorViewModel.Pen
            sequenceViewModel: clipPane.clipSequenceViewModel
            paneItem: clipPane
            viewModelComponent: ClipViewModel {}
            lengthHint: clipPane.clipPaneBehaviorViewModel?.lengthHint ?? 0
            mappedYProperty: "trackNumber"
            mapY: (y) => trackListLocator.mapToIndex(y)
            onClicked: (mouse) => {
                if (dragged)
                    return

            }
        }

        Item {
            id: clipContainer
            anchors.fill: parent
            property double incrementZCounter: 0
            SequenceSlicer {
                model: clipPane.clipSequenceViewModel
                timeViewModel: clipPane.timeViewModel
                timeLayoutViewModel: clipPane.timeLayoutViewModel
                sliceWidth: clipPane.width
                delegate: Item {
                    id: clipRect
                    required property QtObject model
                    property color clipColor: {clipColor = clipPane.trackListViewModel?.handle.items[model.trackNumber].color ?? "white"}
                    property bool current: {current = clipPane.clipSequenceViewModel.handle.currentItem === model}
                    property QtObject clipStyleItem: {
                        clipStyleItem = clipPane.stylesheet.clip.createObject(clipRect, {clipViewModel: model, current, clipColor})
                    }
                    opacity: eraserMouseArea.willBeErased ? 0.5 : 1
                    function bringToFront() {
                        if (model.overlapped)
                            z = ++clipContainer.incrementZCounter
                    }
                    Binding {
                        when: clipRect.visible
                        clipRect.x: clipRect.model.position * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipRect.y: trackListLocator.map[model.trackNumber] ?? 0
                        clipRect.width: clipRect.model.length * (clipPane.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipRect.height: clipPane.trackListViewModel?.handle.items[model.trackNumber].rowHeight ?? 0
                        clipRect.clipColor: clipPane.trackListViewModel?.handle.items[model.trackNumber].color ?? "white"
                        clipRect.current: clipPane.clipSequenceViewModel?.handle.currentItem === model
                        clipRect.clipStyleItem: clipPane.stylesheet.clip.createObject(clipRect, {clipViewModel: clipRect.model, current: clipRect.current, clipColor: clipRect.clipColor})
                        clipNameLabel.x: Math.max(-(clipRect.x + viewport.x), 0)
                        clipNameLabel.visible: clipNameLabel.x + clipNameLabel.width <= clipRect.width
                    }
                    Rectangle {
                        id: clipBackground
                        anchors.fill: parent
                        anchors.topMargin: 1
                        anchors.bottomMargin: 1
                        radius: 4
                        color: clipRect.clipStyleItem.background
                        Behavior on color {
                            ColorAnimation {
                                duration: (clipPane.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                                easing.type: Easing.OutCubic
                            }
                        }
                    }
                    Rectangle {
                        id: clipHeader
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 20
                        color: clipRect.clipStyleItem.header
                        radius: clipBackground.radius
                        Behavior on color {
                            ColorAnimation {
                                duration: (clipPane.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                                easing.type: Easing.OutCubic
                            }
                        }
                        Rectangle {
                            anchors.left: parent.left
                            anchors.bottom: parent.bottom
                            anchors.right: parent.right
                            height: parent.radius
                            color: parent.color
                        }
                        Text {
                            id: clipNameLabel
                            anchors.verticalCenter: parent.verticalCenter
                            leftPadding: 4
                            rightPadding: 16
                            text: clipRect.model.name
                            color: clipRect.clipStyleItem.text
                            Behavior on color {
                                ColorAnimation {
                                    duration: (clipPane.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                                    easing.type: Easing.OutCubic
                                }
                            }
                        }
                    }
                    // TODO graph delegate
                    Rectangle {
                        id: border
                        anchors.fill: clipBackground
                        color: "transparent"
                        radius: clipBackground.radius
                        border.width: Math.min(clipRect.model.selected ? 2 : 1, width / 4)
                        opacity: clipRect.model.selected ? 1 : 0.5
                        border.color: clipRect.clipStyleItem.border
                        Behavior on border.color {
                            ColorAnimation {
                                duration: (clipPane.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                                easing.type: Easing.OutCubic
                            }
                        }
                    }

                    Connections {
                        id: cursorIndicatorLeftBinding
                        target: clipRect.model
                        enabled: false
                        function onPositionChanged() {
                            clipPane.timeLayoutViewModel.cursorPosition = clipRect.model.position
                        }
                    }
                    Connections {
                        id: cursorIndicatorRightBinding
                        target: clipRect.model
                        enabled: false
                        function onPositionChanged() {
                            clipPane.timeLayoutViewModel.cursorPosition = clipRect.model.position + clipRect.model.length
                        }
                        function onLengthChanged() {
                            clipPane.timeLayoutViewModel.cursorPosition = clipRect.model.position + clipRect.model.length
                        }
                    }

                    GenericPointerMouseArea {
                        id: pointerMouseArea

                        visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ClipPaneBehaviorViewModel.Pointer || clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ClipPaneBehaviorViewModel.Pen
                        verticalManipulator: trackListManipulator
                        paneItem: clipPane
                        sequenceViewModel: clipPane.clipSequenceViewModel
                        model: clipRect.model

                        onPressedChanged: () => {
                            if (pressed) {
                                clipRect.bringToFront()
                                mappingOffset = 0.5 * clipPane.trackListViewModel.handle.items[model.trackNumber].rowHeight
                            }
                        }

                        onDraggingChanged: {
                            if (dragging) {
                                cursorIndicatorLeftBinding.enabled = true
                                cursorIndicatorLeftBinding.onPositionChanged()
                            } else {
                                cursorIndicatorLeftBinding.enabled = false
                                clipPane.timeLayoutViewModel.cursorPosition = -1
                            }
                        }

                        property double mappingOffset: 0

                        onMoveSelectedNotesToY: (y) => {
                            let trackCount = clipPane.trackListViewModel.handle.items.length
                            let targetIndex = trackListLocator.mapToIndex(y + clipPane.trackListLayoutViewModel.viewportOffset + mappingOffset)
                            if (targetIndex !== model.trackNumber) {
                                let deltaIndex = targetIndex - model.trackNumber
                                for (let clip of sequenceViewModel.handle.selection) {
                                    if (clip.trackNumber + deltaIndex < 0 || clip.trackNumber + deltaIndex >= trackCount)
                                        return
                                }
                                for (let clip of sequenceViewModel.handle.selection) {
                                    clip.trackNumber += deltaIndex
                                }
                            }
                        }

                        onDoubleClicked: (mouse) => {
                            clipPane.clipSequenceViewModel.handle.currentItem = model
                        }
                    }
                    Repeater {
                        model: 2
                        GenericEdgeMouseArea {
                            id: edgeMouseArea
                            required property int index
                            leftEdge: index
                            visible: pointerMouseArea.visible
                            unitedExtend: false
                            unitedExtendEnabled: false
                            model: clipRect.model
                            sequenceViewModel: clipPane.clipSequenceViewModel
                            paneItem: clipPane

                            onPressedChanged: () => {
                                if (pressed)
                                    clipRect.bringToFront()
                            }

                            onDraggingChanged: () => {
                                let binding = leftEdge ? cursorIndicatorLeftBinding : cursorIndicatorRightBinding
                                if (dragging) {
                                    binding.enabled = true
                                    binding.onPositionChanged()
                                } else {
                                    binding.enabled = false
                                    clipPane.timeLayoutViewModel.cursorPosition = -1
                                }
                            }

                            onClicked: (mouse) => {
                                if (dragged)
                                    return
                                pointerMouseArea.clicked(mouse)
                            }
                            onDoubleClicked: (mouse) => {
                                pointerMouseArea.doubleClicked(mouse)
                            }
                        }
                    }
                    GenericScissorMouseArea {
                        id: scissorMouseArea
                        model: clipRect.model
                        paneItem: clipPane
                        visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ClipPaneBehaviorViewModel.Scissor
                        onPressedChanged: () => {
                            if (pressed)
                                clipRect.bringToFront()
                        }
                        onCutPositionChanged: () => {
                            clipPane.timeLayoutViewModel.cursorPosition = cutPosition
                        }
                        onReleased: (mouse) => {
                            if (cutPosition !== -1)
                                clipPane.clipCut(model, cutPosition)
                        }
                    }
                    GenericEraserMouseArea {
                        id: eraserMouseArea
                        visible: clipPane.clipPaneBehaviorViewModel?.mouseBehavior === ClipPaneBehaviorViewModel.Eraser
                        onReleased: (mouse) => {
                            if (willBeErased)
                                clipPane.clipSequenceViewModel.handle.removeItem(clipRect.model)
                        }
                    }
                }

            }
        }

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            rubberBand: Rectangle {
                color: clipPane.rubberBandStyleItem.background
                border.width: 1
                border.color: clipPane.rubberBandStyleItem.border
            }

        }

    }

    PositionIndicators {
        anchors.fill: parent
        styleItem: clipPane.timeIndicatorsStyleItem
        timeViewModel: clipPane.timeViewModel
        timeLayoutViewModel: clipPane.timeLayoutViewModel
        playbackViewModel: clipPane.playbackViewModel
    }

    StyledScrollBar {
        id: verticalSlider
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 6
        anchors.right: parent.right
        orientation: Qt.Vertical
        allowDragAdjustment: false
        normalColor: clipPane.scrollBarStyleItem.normal
        pressedColor: clipPane.scrollBarStyleItem.pressed
        hoveredColor: clipPane.scrollBarStyleItem.hovered
        animationViewModel: clipPane.animationViewModel
        size: clipPane.height / trackListLocator.viewportHeight
        position: (clipPane.trackListLayoutViewModel?.viewportOffset ?? 0) / trackListLocator.viewportHeight
        onPositionChanged: {
            if (clipPane.trackListLayoutViewModel && Math.abs(clipPane.trackListLayoutViewModel.viewportOffset - position * trackListLocator.viewportHeight) > Number.EPSILON * 1000)
                clipPane.trackListLayoutViewModel.viewportOffset = position * trackListLocator.viewportHeight
        }
    }

    StyledScrollBar {
        id: horizontalSlider
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.bottom: parent.bottom
        orientation: Qt.Horizontal
        normalColor: clipPane.scrollBarStyleItem.normal
        pressedColor: clipPane.scrollBarStyleItem.pressed
        hoveredColor: clipPane.scrollBarStyleItem.hovered
        animationViewModel: clipPane.animationViewModel
        size: clipPane.timeViewModel && clipPane.timeLayoutViewModel ? clipPane.width / clipPane.timeLayoutViewModel.pixelDensity / clipPane.timeViewModel.end : 0
        position: clipPane.timeViewModel ? clipPane.timeViewModel.start / clipPane.timeViewModel.end : 0
        onPositionChanged: {
            if (clipPane.timeViewModel && Math.abs(clipPane.timeViewModel.start - position * clipPane.timeViewModel.end) > Number.EPSILON * 100)
                clipPane.timeViewModel.start = position * clipPane.timeViewModel.end
        }
        onStartDragged: (pos) => {
            if (!clipPane.timeViewModel || !clipPane.timeLayoutViewModel)
                return
            let newSize = position + size - pos
            let newPixelDensity = clipPane.width / clipPane.timeViewModel.end / newSize
            if (newPixelDensity <= clipPane.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= clipPane.timeLayoutViewModel.maximumPixelDensity)
                return
            clipPane.timeViewModel.start = clipPane.timeViewModel.end * pos
            clipPane.timeLayoutViewModel.pixelDensity = newPixelDensity
        }
        onEndDragged: (pos) => {
            if (!clipPane.timeViewModel || !clipPane.timeLayoutViewModel)
                return
            let newSize = pos - position
            let newPixelDensity = clipPane.width / clipPane.timeViewModel.end / newSize
            if (newPixelDensity <= clipPane.timeLayoutViewModel.minimumPixelDensity || newPixelDensity >= clipPane.timeLayoutViewModel.maximumPixelDensity)
                return
            clipPane.timeLayoutViewModel.pixelDensity = newPixelDensity
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: clipPane.scrollBehaviorViewModel
        onZoomed: function (ratioX, _, x, _, isPhysicalWheel) {
            timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel)
        }
        onMoved: function (x, y, isPhysicalWheel) {
            timeManipulator.moveViewBy(x, isPhysicalWheel)
            trackListManipulator.moveViewBy(y, isPhysicalWheel)
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: clipPane.scrollBehaviorViewModel
        onMoved: function (x, y) {
            timeManipulator.moveViewBy(x)
            trackListManipulator.moveViewBy(y)
        }
    }

}