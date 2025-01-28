import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Palette as ScopicFlowPalette

Item {
    id: labelSequence

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null
    property QtObject labelSequenceViewModel: null
    property QtObject labelSequenceLayoutViewModel: null

    function moveSelectionTo(position, model) {
        if (position !== model.position) {
            let deltaPosition = position - model.position
            for (let label of labelSequenceViewModel.handle.selection) {
                if (label.position + deltaPosition < 0)
                    return
                if (label.position + deltaPosition > timeViewModel.end)
                    timeViewModel.end = label.position + deltaPosition
            }
            for (let label of labelSequenceViewModel.handle.selection) {
                label.position = label.position + deltaPosition
            }
        }
    }
    function moveSelectedLabelsTo(x, model) {
        moveSelectionTo(locator.alignTick(locator.mapToTick(x)), model)
    }
    function moveSelectedLabelOnDragScrolling(isBackward, model) {
        let x = isBackward ? 0 : width
        let alignedTick = isBackward ? locator.alignTickCeil(locator.mapToTick(x)) : locator.alignTickFloor(locator.mapToTick(x))
        moveSelectionTo(alignedTick, model)
    }

    signal contextMenuRequested(tick: int)
    signal contextMenuRequestedForLabel(label: QtObject)

    readonly property QtObject defaultPalette: ScopicFlowPalette.LabelSequence {}

    readonly property QtObject palette: paletteViewModel?.palette?.labelSequence ?? defaultPalette

    clip: true
    implicitHeight: 20

    Component {
        id: labelViewModelComponent
        LabelViewModel {

        }
    }

    TimeAlignmentPositionLocator {
        id: locator
        anchors.fill: parent
        timeViewModel: labelSequence.timeViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel
    }

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: labelSequence.timeViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel
        animationViewModel: labelSequence.animationViewModel
    }

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: labelSequence.labelSequenceViewModel
    }

    Rectangle {
        id: background
        anchors.fill: parent
        anchors.leftMargin: -1 // set to -1 to hide left and right border
        anchors.rightMargin: -1
        color: labelSequence.palette.backgroundColor
        border.width: 1
        border.color: labelSequence.palette.borderColor
    }

    Item {
        id: viewport
        readonly property double start: labelSequence.timeViewModel?.start ?? 0
        readonly property double end: labelSequence.timeViewModel?.end ?? 0
        readonly property double pixelDensity: labelSequence.timeLayoutViewModel?.pixelDensity ?? 0
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        x: -start * pixelDensity
        width: end * pixelDensity
        clip: true

        Item {
            id: rubberBandQuasiMouseArea
            property bool dragged: false
            property double pressedX: 0
            function doDragRubberBand(targetX) {
                rubberBandLayer.updateSelection(Qt.point(targetX, labelSequence.height))
            }
            DragScroller {
                id: rubberBandDragScroller
                onMoved: (deltaX) => {
                    timeManipulator.moveViewBy(deltaX)
                    parent.doDragRubberBand(labelSequence.mapToItem(viewport, deltaX > 0 ? labelSequence.width : 0, 0).x)
                }
            }
            function onPressed(mouse) {
                dragged = false
                pressedX = mouse.x
            }
            function onPositionChanged(mouse) {
                dragged = true
                if (!rubberBandLayer.started) {
                    selectionManipulator.select(null, Qt.RightButton, mouse.modifiers)
                    rubberBandLayer.startSelection(Qt.point(pressedX, 0))
                }
                let parentX = viewport.mapToItem(labelSequence, mouse.x, mouse.y).x
                rubberBandDragScroller.determine(parentX, labelSequence.width, 0, 0, (triggered) => {
                    if (!triggered) {
                        doDragRubberBand(mouse.x)
                    }
                })
            }
            function onCanceled() {
                rubberBandLayer.endSelection()
                rubberBandDragScroller.running = false
            }
            function onClicked(mouse) {
                if (mouse.button === Qt.RightButton) {
                    let selection = labelSequence.labelSequenceViewModel.handle.selection
                    if (dragged && selection.length)
                        labelSequence.contextMenuRequestedForLabel(selection[0])
                    else
                        labelSequence.contextMenuRequested(Math.round(mouse.x / labelSequence.timeLayoutViewModel.pixelDensity))
                }
            }
        }

        MouseArea {
            id: backMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPressed: (mouse) => rubberBandQuasiMouseArea.onPressed(mouse)
            onPositionChanged: (mouse) => rubberBandQuasiMouseArea.onPositionChanged(mouse)
            onClicked: (mouse) => {
                rubberBandQuasiMouseArea.onClicked(mouse)
                if (rubberBandQuasiMouseArea.dragged)
                    return
                selectionManipulator.select(null, mouse.button, mouse.modifiers)
            }
            onDoubleClicked: (mouse) => {
                if (mouse.button !== Qt.LeftButton || mouse.modifiers)
                    return
                let label = labelViewModelComponent.createObject(null, {
                    position: locator.alignTick(locator.mapToTick(mapToItem(labelSequence, mouse.x, 0).x))
                })
                labelSequence.labelSequenceViewModel.handle.insertItem(label)
                selectionManipulator.select(label, Qt.LeftButton, 0)
                labelSequence.labelSequenceLayoutViewModel.editing = true
            }
            onReleased: canceled()
            onCanceled: rubberBandQuasiMouseArea.onCanceled()
        }

        Item {
            id: labelContainer
            anchors.fill: parent
            SequenceSlicer {
                model: labelSequence.labelSequenceViewModel
                timeViewModel: labelSequence.timeViewModel
                timeLayoutViewModel: labelSequence.timeLayoutViewModel
                sliceWidth: labelSequence.width
                leftOutBound: 256
                delegate: LabelSequenceDelegate {
                    id: labelRect
                    palette: labelSequence.palette
                    animationViewModel: labelSequence.animationViewModel
                    labelSequenceViewModel: labelSequence.labelSequenceViewModel
                    labelSequenceLayoutViewModel: labelSequence.labelSequenceLayoutViewModel
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    Binding {
                        when: labelRect.visible
                        labelRect.x: labelRect.model.position * viewport.pixelDensity
                        labelRect.z: labelRect.model.selected ? Infinity : model.position
                    }
                    onXChanged: rubberBandLayer.insertItem(model, Qt.rect(x, 0, width, 1 << 20))
                    onWidthChanged: rubberBandLayer.insertItem(model, Qt.rect(x, 0, width, 1 << 20))
                    Component.onDestruction: rubberBandLayer.removeItem(model)

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        property double pressedDeltaX: 0
                        property bool dragged: false
                        DragScroller {
                            id: labelDragScroller
                            onMoved: function (deltaX) {
                                timeManipulator.moveViewBy(deltaX)
                                labelSequence.moveSelectedLabelOnDragScrolling(deltaX < 0, labelRect.model)
                            }
                        }
                        Connections {
                            id: cursorIndicatorBinding
                            target: labelRect.model
                            enabled: false
                            function onPositionChanged() {
                                labelSequence.timeLayoutViewModel.cursorPosition = labelRect.model.position
                            }
                        }
                        onPressed: (mouse) => {
                            dragged = false
                            pressedDeltaX = mouse.x
                        }
                        onPositionChanged: (mouse) => {
                            if (!dragged) {
                                dragged = true
                                for (let label of labelSequence.labelSequenceViewModel.handle.selection) {
                                    label.intermediate = true
                                }
                            }
                            cursorIndicatorBinding.enabled = true
                            selectionManipulator.select(labelRect.model, Qt.RightButton, mouse.modifiers)
                            let parentX = labelRect.mapToItem(labelSequence, mouse.x, mouse.y).x
                            labelDragScroller.determine(parentX, labelSequence.width, 0, 0, (triggered) => {
                                if (!triggered) {
                                    labelSequence.moveSelectedLabelsTo(parentX - pressedDeltaX, labelRect.model)
                                }
                            })
                        }
                        onReleased: canceled()
                        onCanceled: {
                            if (dragged) {
                                for (let label of labelSequence.labelSequenceViewModel.handle.selection) {
                                    label.intermediate = false
                                }
                            }
                            labelDragScroller.running = false
                            cursorIndicatorBinding.enabled = false
                            labelSequence.timeLayoutViewModel.cursorPosition = -1
                        }
                        onClicked: (mouse) => {
                            if (!dragged)
                                selectionManipulator.select(model, mouse.button, mouse.modifiers)
                            if (mouse.button === Qt.RightButton) {
                                labelSequence.contextMenuRequestedForLabel(model)
                            }
                        }
                        onDoubleClicked: (mouse) => {
                            if (mouse.button === Qt.LeftButton) {
                                labelSequence.labelSequenceViewModel.handle.currentItem = labelRect.model
                                labelSequence.labelSequenceLayoutViewModel.editing = true
                            }
                        }
                    }
                }
            }
        }

        MouseArea {
            id: frontMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onPressed: (mouse) => {
                if (!(mouse.modifiers & Qt.AltModifier)) {
                    mouse.accepted = false
                    return
                }
                rubberBandQuasiMouseArea.onPressed(mouse)
            }
            onPositionChanged: (mouse) => rubberBandQuasiMouseArea.onPositionChanged(mouse)
            onClicked: (mouse) => rubberBandQuasiMouseArea.onClicked(mouse)
            onReleased: canceled()
            onCanceled: rubberBandQuasiMouseArea.onCanceled()
        }

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            z: 2
            rubberBand: Rectangle {
                color: labelSequence.palette.rubberBandColor
                border.width: 1
                border.color: labelSequence.palette.rubberBandBorderColor
            }
        }
    }

    PositionIndicators {
        anchors.fill: parent
        palette: labelSequence.palette
        timeViewModel: labelSequence.timeViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel
        playbackViewModel: labelSequence.playbackViewModel
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: labelSequence.scrollBehaviorViewModel
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel)
        onMoved: (x, _, isPhysicalWheel) => timeManipulator.moveViewBy(x, isPhysicalWheel)
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: labelSequence.scrollBehaviorViewModel
        direction: Qt.Horizontal
        onMoved: (x) => timeManipulator.moveViewBy(x)
    }
}