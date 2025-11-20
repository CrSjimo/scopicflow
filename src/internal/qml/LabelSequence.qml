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

    Keys.onMenuPressed: () => {
        if (labelSequenceInteractionController) {
            labelSequenceInteractionController.contextMenuRequested(labelSequence, -1)
        }
    }

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
            let alignedTick = isBackward ? timeManipulator.alignPosition(timeManipulator.mapToPosition(x), ScopicFlow.AO_Ceil) : timeManipulator.alignPosition(timeManipulator.mapToPosition(x), ScopicFlow.AO_Floor);
            moveSelectionTo(alignedTick, model);
        }
        function moveSelectedLabelsTo(x, model) {
            moveSelectionTo(timeManipulator.alignPosition(timeManipulator.mapToPosition(x)), model);
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

    GenericBackRightButtonMouseArea {
        id: rightButtonMouseArea

        controller: labelSequence.labelSequenceInteractionController
        timeManipulator: timeManipulator
    }

    GenericBackPointerMouseArea {
        id: backPointerMouseArea
        interactionFlag: LabelSequenceInteractionController.SelectByRubberBand
        timeManipulator: timeManipulator
        rubberBandLayer: rubberBandLayer
        selectionManipulator: selectionManipulator
        mapPoint: (p) => Qt.point(mapToItem(rubberBandLayer, p.x, 0).x, rubberBandLayer.started ? height : 0)
    }

    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: labelSequence.timeViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel

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
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Horizontal
        viewModel: labelSequence.scrollBehaviorViewModel

        onMoved: (x, _, isPhysicalWheel) => timeManipulator.moveViewBy(x, isPhysicalWheel)
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel)
    }
}
