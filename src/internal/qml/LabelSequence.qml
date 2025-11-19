import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: labelSequence

    property PointSequenceViewModel labelSequenceViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property LabelSequenceInteractionController labelSequenceInteractionController: null

    clip: true
    implicitHeight: 20

    QtObject {
        id: helper

        function ensureCurrentItemVisibleWhileEditing() {
            if (labelSequence.labelSequenceViewModel?.currentItem) { // TODO and editing
                labelLengthReference.text = labelSequence.labelSequenceViewModel.currentItem.content;
                timeManipulator.ensureVisible(labelSequence.labelSequenceViewModel.currentItem.position, 0, 0, labelLengthReference.implicitWidth + 8);
            }
        }
        function moveSelectedLabelOnDragScrolling(isBackward, model) {
            let x = isBackward ? 0 : width;
            let alignedTick = isBackward ? timeManipulator.alignTick(timeManipulator.mapToTick(x), ScopicFlow.AO_Ceil) : timeManipulator.alignTick(timeManipulator.mapToTick(x), ScopicFlow.AO_Floor);
            moveSelectionTo(alignedTick, model);
        }
        function moveSelectedLabelsTo(x, model) {
            moveSelectionTo(timeManipulator.alignTick(timeManipulator.mapToTick(x)), model);
        }
        function moveSelectionTo(position, model) {
            if (position !== model.position) {
                let deltaPosition = position - model.position;
                for (let label of labelSequence.labelSequenceViewModel.selection) {
                    if (label.position + deltaPosition < 0)
                        return;
                    if (label.position + deltaPosition > labelSequence.timeViewModel.end)
                        labelSequence.timeViewModel.end = label.position + deltaPosition;
                }
                for (let label of labelSequence.labelSequenceViewModel.selection) {
                    label.position = label.position + deltaPosition;
                }
            }
        }

        readonly property Text labelLengthReference: Text {
            id: labelLengthReference

            visible: false
        }

        readonly property Connections connections: Connections {
            function onCurrentItemChanged() {
                helper.ensureCurrentItemVisibleWhileEditing();
            }

            target: labelSequence.labelSequenceViewModel
        }
    }
    Component {
        id: labelViewModelComponent

        LabelViewModel {
        }
    }
    TimeManipulator {
        id: timeManipulator

        target: labelSequence
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
        color: Theme.backgroundColor(labelSequence.ThemedItem.backgroundLevel)
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
                let position = containsMouse || pressed ? timeManipulator.mapToTick(mouseX) : -1;
                labelSequence.interactionControllerNotifier?.sceneInteracted(interactionType, labelSequence.labelSequenceViewModel, labelSequence.labelSequenceBehaviorViewModel, position, 0);
            }
            handleBeforeInteractionNotificationCallback: interactionType => {
                let position = containsMouse || pressed ? timeManipulator.mapToTick(mouseX) : -1;
                if (labelSequence.interactionControllerNotifier?.handleSceneInteraction(interactionType, labelSequence.labelSequenceViewModel, labelSequence.labelSequenceBehaviorViewModel, position, 0))
                    return false;
                return true;
            }
            paneItem: labelSequence

            onDoubleClicked: mouse => {
                if (!handleBeforeInteractionNotification(ScopicFlow.II_DoubleClicked))
                    return;
                let label = labelViewModelComponent.createObject(null, {
                    position: timeManipulator.alignTick(timeManipulator.mapToTick(mapToItem(labelSequence, mouse.x, 0).x))
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
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel)
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        direction: Qt.Horizontal
        viewModel: labelSequence.scrollBehaviorViewModel

        onMoved: x => timeManipulator.moveViewBy(x)
    }
}
