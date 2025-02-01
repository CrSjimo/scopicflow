import QtQml
import QtQuick
import QtQuick.Controls.Basic

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: labelSequence

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject labelSequenceViewModel: null
    property QtObject labelSequenceBehaviorViewModel: null

    property bool active: false

    readonly property QtObject stylesheet: LabelSequenceStylesheet {}
    readonly property QtObject labelSequenceStyleItem: stylesheet.labelSequence.createObject(labelSequence, {active})
    readonly property QtObject rubberBandLayerStyleItem: stylesheet.rubberBand.createObject(labelSequence)
    readonly property QtObject timeIndicatorsStyleItem: stylesheet.timeIndicators.createObject(labelSequence)

    Text {
        id: labelLengthReference
        visible: false
    }
    function ensureCurrentItemVisible() {
        if (labelSequenceViewModel?.handle.currentItem && labelSequenceBehaviorViewModel?.editing) {
            labelLengthReference.text = labelSequenceViewModel.handle.currentItem.content
            timeManipulator.ensureVisible(labelSequenceViewModel.handle.currentItem.position, 0, 0, labelLengthReference.implicitWidth + 8)
        }
    }

    Connections {
        target: labelSequence.labelSequenceViewModel?.handle ?? null
        function onCurrentItemChanged() {
            labelSequence.ensureCurrentItemVisible()
        }
    }
    Connections {
        target: labelSequenceBehaviorViewModel
        function onEditingChanged() {
            labelSequence.ensureCurrentItemVisible()
        }
    }

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
        color: labelSequence.labelSequenceStyleItem.background
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

        MouseArea {
            id: backMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
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
            onPressed: (mouse) => {
                dragged = false
                pressedX = mouse.x
            }
            onPositionChanged: (mouse) => {
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
            onClicked: (mouse) => {
                if (mouse.button === Qt.RightButton) {
                    let selection = labelSequence.labelSequenceViewModel.handle.selection
                    if (dragged && selection.length)
                        labelSequence.contextMenuRequestedForLabel(selection[0])
                    else
                        labelSequence.contextMenuRequested(Math.round(mouse.x / labelSequence.timeLayoutViewModel.pixelDensity))
                }
                if (dragged)
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
                labelSequence.labelSequenceBehaviorViewModel.editing = true
            }
            onReleased: canceled()
            onCanceled: {
                rubberBandLayer.endSelection()
                rubberBandDragScroller.running = false
            }
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
                    animationViewModel: labelSequence.animationViewModel
                    labelSequenceViewModel: labelSequence.labelSequenceViewModel
                    labelSequenceBehaviorViewModel: labelSequence.labelSequenceBehaviorViewModel
                    stylesheet: labelSequence.stylesheet
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    Binding {
                        when: labelRect.visible
                        labelRect.x: labelRect.model.position * viewport.pixelDensity
                        labelRect.z: labelRect.model.selected ? Infinity : model.position
                    }
                    function handleRubberBand() {
                        selectionManipulator.viewModel // Magic. Do not delete this line.
                        if (visible)
                            rubberBandLayer.insertItem(model, Qt.rect(x, 0, width, 1 << 20))
                        else
                            rubberBandLayer.removeItem(model)
                    }
                    onVisibleChanged: handleRubberBand()
                    onXChanged: handleRubberBand()
                    onWidthChanged: handleRubberBand()
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
                                labelSequence.labelSequenceBehaviorViewModel.editing = true
                            }
                        }
                    }
                }
            }
        }

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            z: 2
            rubberBand: Rectangle {
                color: labelSequence.rubberBandLayerStyleItem.background
                border.width: 1
                border.color: labelSequence.rubberBandLayerStyleItem.border
            }
        }
    }

    Rectangle {
        id: border
        anchors.fill: parent
        color: "transparent"
        border.width: 1
        border.color: labelSequence.labelSequenceStyleItem.border
    }

    PositionIndicators {
        anchors.fill: parent
        styleItem: labelSequence.timeIndicatorsStyleItem
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