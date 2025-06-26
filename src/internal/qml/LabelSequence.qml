import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: labelSequence

    property QtObject animationViewModel: null
    property QtObject interactionControllerNotifier: null
    property QtObject labelSequenceBehaviorViewModel: null
    property QtObject labelSequenceViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    property QtObject transactionControllerNotifier: null

    function ensureCurrentItemVisible() {
        if (labelSequenceViewModel?.handle.currentItem && labelSequenceBehaviorViewModel?.editing) {
            labelLengthReference.text = labelSequenceViewModel.handle.currentItem.content;
            timeManipulator.ensureVisible(labelSequenceViewModel.handle.currentItem.position, 0, 0, labelLengthReference.implicitWidth + 8);
        }
    }
    function moveSelectedLabelOnDragScrolling(isBackward, model) {
        let x = isBackward ? 0 : width;
        let alignedTick = isBackward ? timeLocator.alignTickCeil(timeLocator.mapToTick(x)) : timeLocator.alignTickFloor(timeLocator.mapToTick(x));
        moveSelectionTo(alignedTick, model);
    }
    function moveSelectedLabelsTo(x, model) {
        moveSelectionTo(timeLocator.alignTick(timeLocator.mapToTick(x)), model);
    }
    function moveSelectionTo(position, model) {
        if (position !== model.position) {
            let deltaPosition = position - model.position;
            for (let label of labelSequenceViewModel.handle.selection) {
                if (label.position + deltaPosition < 0)
                    return;
                if (label.position + deltaPosition > timeViewModel.end)
                    timeViewModel.end = label.position + deltaPosition;
            }
            for (let label of labelSequenceViewModel.handle.selection) {
                label.position = label.position + deltaPosition;
            }
        }
    }

    clip: true
    implicitHeight: 20

    Text {
        id: labelLengthReference

        visible: false
    }
    Connections {
        function onCurrentItemChanged() {
            labelSequence.ensureCurrentItemVisible();
        }

        target: labelSequence.labelSequenceViewModel?.handle ?? null
    }
    Connections {
        function onEditingChanged() {
            labelSequence.ensureCurrentItemVisible();
        }

        target: labelSequenceBehaviorViewModel
    }
    Component {
        id: labelViewModelComponent

        LabelViewModel {
        }
    }
    TimeAlignmentPositionLocator {
        id: timeLocator

        anchors.fill: parent
        timeLayoutViewModel: labelSequence.timeLayoutViewModel
        timeViewModel: labelSequence.timeViewModel
    }
    TimeManipulator {
        id: timeManipulator

        anchors.fill: parent
        animationViewModel: labelSequence.animationViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel
        timeViewModel: labelSequence.timeViewModel
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

        readonly property double end: labelSequence.timeViewModel?.end ?? 0
        readonly property double pixelDensity: labelSequence.timeLayoutViewModel?.pixelDensity ?? 0
        readonly property double start: labelSequence.timeViewModel?.start ?? 0

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        clip: true
        width: end * pixelDensity
        x: -start * pixelDensity

        MouseArea {
            id: backRightButtonMouseArea

            acceptedButtons: Qt.RightButton
            anchors.fill: parent

            onClicked: mouse => {
                labelSequence.contextMenuRequested(Math.round(mouse.x / labelSequence.timeLayoutViewModel.pixelDensity));
                selectionManipulator.select(null, mouse.button, mouse.modifiers);
            }
        }
        GenericBackPointerMouseArea {
            id: backPointerMouseArea

            emitInteractionNotificationSignalCallback: interactionType => {
                let position = containsMouse || pressed ? timeLocator.mapToTick(mouseX) : -1;
                labelSequence.interactionControllerNotifier?.sceneInteracted(interactionType, labelSequence.labelSequenceViewModel, labelSequence.labelSequenceBehaviorViewModel, position, 0);
            }
            handleBeforeInteractionNotificationCallback: interactionType => {
                let position = containsMouse || pressed ? timeLocator.mapToTick(mouseX) : -1;
                if (labelSequence.interactionControllerNotifier?.handleSceneInteraction(interactionType, labelSequence.labelSequenceViewModel, labelSequence.labelSequenceBehaviorViewModel, position, 0))
                    return false;
                return true;
            }
            paneItem: labelSequence

            onDoubleClicked: mouse => {
                if (!handleBeforeInteractionNotification(ScopicFlow.II_DoubleClicked))
                    return;
                let label = labelViewModelComponent.createObject(null, {
                    position: timeLocator.alignTick(timeLocator.mapToTick(mapToItem(labelSequence, mouse.x, 0).x))
                });
                labelSequence.labelSequenceViewModel.handle.insertItem(label);
                selectionManipulator.select(label, Qt.LeftButton, 0);
                labelSequence.labelSequenceBehaviorViewModel.editing = true;
                emitInteractionNotificationSignal(ScopicFlow.II_DoubleClicked);
            }
            onRubberBandStartRequired: p => {
                rubberBandLayer.startSelection(Qt.point(p.x, 0));
            }
            onRubberBandUpdateRequired: p => {
                rubberBandLayer.updateSelection(Qt.point(p.x, labelSequence.height));
            }
        }
        Item {
            id: labelContainer

            anchors.fill: parent

            SequenceSlicer {
                leftOutBound: 256
                model: labelSequence.labelSequenceViewModel
                sliceWidth: labelSequence.width
                timeLayoutViewModel: labelSequence.timeLayoutViewModel
                timeViewModel: labelSequence.timeViewModel

                delegate: LabelSequenceDelegate {
                    id: labelRect

                    function emitInteractionNotificationSignal(interactionType) {
                        labelSequence.interactionControllerNotifier?.itemInteracted(interactionType, labelRect.model, -1, labelSequence.labelSequenceViewModel);
                    }
                    function handleBeforeInteractionNotification(interactionType) {
                        if (labelSequence.interactionControllerNotifier?.handleItemInteraction(interactionType, labelRect.model, -1, labelSequence.labelSequenceViewModel))
                            return false;
                        return true;
                    }
                    function handleRubberBand() {
                        selectionManipulator.viewModel; // Magic. Do not delete this line.
                        if (visible)
                            rubberBandLayer.insertItem(model, Qt.rect(x, 0, width, 1 << 20));
                        else
                            rubberBandLayer.removeItem(model);
                    }
                    function sendInteractionNotification(interactionType) {
                        if (!handleBeforeInteractionNotification(interactionType))
                            return false;
                        sendInteractionNotification(interactionType);
                        return true;
                    }

                    anchors.bottom: parent.bottom
                    anchors.top: parent.top
                    animationViewModel: labelSequence.animationViewModel
                    labelSequenceBehaviorViewModel: labelSequence.labelSequenceBehaviorViewModel
                    labelSequenceViewModel: labelSequence.labelSequenceViewModel

                    Component.onDestruction: rubberBandLayer.removeItem(model)
                    onVisibleChanged: handleRubberBand()
                    onWidthChanged: handleRubberBand()
                    onXChanged: handleRubberBand()

                    Binding {
                        labelRect.x: labelRect.model.position * viewport.pixelDensity
                        labelRect.z: labelRect.model.selected ? Infinity : model.position
                        when: labelRect.visible
                    }
                    Connections {
                        id: cursorIndicatorBinding

                        function onPositionChanged() {
                            labelSequence.timeLayoutViewModel.cursorPosition = labelRect.model.position;
                        }

                        enabled: false
                        target: labelRect.model
                    }
                    MouseArea {
                        id: rightButtonMouseArea

                        acceptedButtons: Qt.RightButton
                        anchors.fill: parent

                        onClicked: mouse => {
                            if (!labelRect.handleBeforeInteractionNotification(ScopicFlow.II_ContextMenu))
                                return;
                            selectionManipulator.select(labelRect.model, mouse.button, mouse.modifiers);
                            labelRect.emitInteractionNotificationSignal(ScopicFlow.II_ContextMenu);
                        }
                    }
                    GenericPointerMouseArea {
                        id: pointerMouseArea

                        emitInteractionNotificationSignalCallback: interactionType => labelRect.emitInteractionNotificationSignal(interactionType)
                        handleBeforeInteractionNotificationCallback: interactionType => labelRect.handleBeforeInteractionNotification(interactionType)
                        model: labelRect.model
                        paneItem: labelSequence
                        sequenceViewModel: labelSequence.labelSequenceViewModel
                        transactionControllerNotifier: labelSequence.transactionControllerNotifier

                        onDoubleClicked: () => {
                            if (!handleBeforeInteractionNotification(ScopicFlow.II_DoubleClicked))
                                return;
                            labelSequence.labelSequenceViewModel.handle.currentItem = labelRect.model;
                            labelSequence.labelSequenceBehaviorViewModel.editing = true;
                            emitInteractionNotificationSignal(ScopicFlow.II_DoubleClicked);
                        }
                        onDraggingChanged: {
                            if (dragging) {
                                cursorIndicatorBinding.enabled = true;
                                cursorIndicatorBinding.onPositionChanged();
                            } else {
                                cursorIndicatorBinding.enabled = false;
                                labelSequence.timeLayoutViewModel.cursorPosition = -1;
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
            transactionControllerNotifier: labelSequence.transactionControllerNotifier
            z: 2

            rubberBand: RubberBandRectangle {
            }
        }
    }
    PositionIndicators {
        anchors.fill: parent
        playbackViewModel: labelSequence.playbackViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel
        timeViewModel: labelSequence.timeViewModel
    }
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Horizontal
        viewModel: labelSequence.scrollBehaviorViewModel

        onMoved: (x, _, isPhysicalWheel) => timeManipulator.moveViewBy(x, isPhysicalWheel)
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel)
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        direction: Qt.Horizontal
        viewModel: labelSequence.scrollBehaviorViewModel

        onMoved: x => timeManipulator.moveViewBy(x)
    }
}
