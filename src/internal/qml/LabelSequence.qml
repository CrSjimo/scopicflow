import QtQml
import QtQuick

import SVSCraft.UIComponents

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
    property QtObject interactionControllerNotifier: null
    property QtObject transactionControllerNotifier: null

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
        moveSelectionTo(timeLocator.alignTick(timeLocator.mapToTick(x)), model)
    }
    function moveSelectedLabelOnDragScrolling(isBackward, model) {
        let x = isBackward ? 0 : width
        let alignedTick = isBackward ? timeLocator.alignTickCeil(timeLocator.mapToTick(x)) : timeLocator.alignTickFloor(timeLocator.mapToTick(x))
        moveSelectionTo(alignedTick, model)
    }

    clip: true
    implicitHeight: 20

    Component {
        id: labelViewModelComponent
        LabelViewModel {

        }
    }

    TimeAlignmentPositionLocator {
        id: timeLocator
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
        color: SFPalette.editAreaPrimaryColor
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
            id: backRightButtonMouseArea
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: (mouse) => {
                labelSequence.contextMenuRequested(Math.round(mouse.x / labelSequence.timeLayoutViewModel.pixelDensity))
                selectionManipulator.select(null, mouse.button, mouse.modifiers)
            }
        }

        GenericBackPointerMouseArea {
            id: backPointerMouseArea
            paneItem: labelSequence

            handleBeforeInteractionNotificationCallback: (interactionType) => {
                let position = containsMouse || pressed ? timeLocator.mapToTick(mouseX) : -1
                if (labelSequence.interactionControllerNotifier?.handleSceneInteraction(interactionType, labelSequence.labelSequenceViewModel, labelSequence.labelSequenceBehaviorViewModel, position, 0))
                    return false
                return true
            }

            emitInteractionNotificationSignalCallback: (interactionType) => {
                let position = containsMouse || pressed ? timeLocator.mapToTick(mouseX) : -1
                labelSequence.interactionControllerNotifier?.sceneInteracted(interactionType, labelSequence.labelSequenceViewModel, labelSequence.labelSequenceBehaviorViewModel, position, 0)
            }

            onRubberBandStartRequired: (p) => {
                rubberBandLayer.startSelection(Qt.point(p.x, 0))
            }
            onRubberBandUpdateRequired: (p) => {
                rubberBandLayer.updateSelection(Qt.point(p.x, labelSequence.height))
            }
            onDoubleClicked: (mouse) => {
                if (!handleBeforeInteractionNotification(ScopicFlow.II_DoubleClicked))
                    return
                let label = labelViewModelComponent.createObject(null, {
                    position: timeLocator.alignTick(timeLocator.mapToTick(mapToItem(labelSequence, mouse.x, 0).x))
                })
                labelSequence.labelSequenceViewModel.handle.insertItem(label)
                selectionManipulator.select(label, Qt.LeftButton, 0)
                labelSequence.labelSequenceBehaviorViewModel.editing = true
                emitInteractionNotificationSignal(ScopicFlow.II_DoubleClicked)
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

                    Connections {
                        id: cursorIndicatorBinding
                        target: labelRect.model
                        enabled: false
                        function onPositionChanged() {
                            labelSequence.timeLayoutViewModel.cursorPosition = labelRect.model.position
                        }
                    }

                    function sendInteractionNotification(interactionType) {
                        if (!handleBeforeInteractionNotification(interactionType))
                            return false
                        sendInteractionNotification(interactionType)
                        return true
                    }
                    function handleBeforeInteractionNotification(interactionType) {
                        if (labelSequence.interactionControllerNotifier?.handleItemInteraction(interactionType, labelRect.model, -1, labelSequence.labelSequenceViewModel))
                            return false
                        return true
                    }
                    function emitInteractionNotificationSignal(interactionType) {
                        labelSequence.interactionControllerNotifier?.itemInteracted(interactionType, labelRect.model, -1, labelSequence.labelSequenceViewModel)
                    }

                    MouseArea {
                        id: rightButtonMouseArea
                        anchors.fill: parent
                        acceptedButtons: Qt.RightButton
                        onClicked: (mouse) => {
                            if (!labelRect.handleBeforeInteractionNotification(ScopicFlow.II_ContextMenu))
                                return
                            selectionManipulator.select(labelRect.model, mouse.button, mouse.modifiers)
                            labelRect.emitInteractionNotificationSignal(ScopicFlow.II_ContextMenu)
                        }
                    }

                    GenericPointerMouseArea {
                        id: pointerMouseArea
                        paneItem: labelSequence
                        sequenceViewModel: labelSequence.labelSequenceViewModel
                        model: labelRect.model
                        transactionControllerNotifier: labelSequence.transactionControllerNotifier
                        handleBeforeInteractionNotificationCallback: (interactionType) => labelRect.handleBeforeInteractionNotification(interactionType)
                        emitInteractionNotificationSignalCallback: (interactionType) => labelRect.emitInteractionNotificationSignal(interactionType)

                        onDraggingChanged: {
                            if (dragging) {
                                cursorIndicatorBinding.enabled = true
                                cursorIndicatorBinding.onPositionChanged()
                            } else {
                                cursorIndicatorBinding.enabled = false
                                labelSequence.timeLayoutViewModel.cursorPosition = -1
                            }
                        }

                        onDoubleClicked: () => {
                            if (!handleBeforeInteractionNotification(ScopicFlow.II_DoubleClicked))
                                return
                            labelSequence.labelSequenceViewModel.handle.currentItem = labelRect.model
                            labelSequence.labelSequenceBehaviorViewModel.editing = true
                            emitInteractionNotificationSignal(ScopicFlow.II_DoubleClicked)
                        }
                    }
                }
            }
        }

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            transactionControllerNotifier: labelSequence.transactionControllerNotifier
            z: 2
            rubberBand: RubberBandRectangle {
            }
        }
    }

    PositionIndicators {
        anchors.fill: parent
        timeViewModel: labelSequence.timeViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel
        playbackViewModel: labelSequence.playbackViewModel
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: labelSequence.scrollBehaviorViewModel
        movableOrientation: Qt.Horizontal
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