import QtQml
import QtQuick
import QtQuick.Controls

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
    property SelectionController selectionController: null

    clip: true
    implicitHeight: 20

    function editInPlace(labelViewModel: LabelViewModel) {
        inPlaceEditPopup.close()
        if (labelViewModel) {
            timeManipulator.ensureVisible(labelViewModel.position, 0, 0, labelItemWidth(labelViewModel));
            inPlaceEditPopup.model = labelViewModel
            labelSequenceInteractionController.inPlaceEditOperationTriggered(labelSequence, labelViewModel, LabelSequenceInteractionController.StartEditing)
            inPlaceEditPopup.open()
        }

    }

    function labelItemWidth(labelViewModel: LabelViewModel): double {
        if (!labelViewModel) {
            return 0
        }
        helper.labelLengthReference.text = labelViewModel.content
        return labelLengthReference.implicitWidth + 8
    }

    Keys.onMenuPressed: () => {
        if (labelSequenceInteractionController) {
            labelSequenceInteractionController.contextMenuRequested(labelSequence, -1)
        }
    }

    QtObject {
        id: helper

        readonly property Text labelLengthReference: Text {
            id: labelLengthReference

            visible: false
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
    Rectangle {
        id: background

        anchors.fill: parent
        color: Theme.backgroundColor(labelSequence.ThemedItem.backgroundLevel)
    }

    GenericBackRightButtonMouseArea {
        id: rightButtonMouseArea

        controller: labelSequence.labelSequenceInteractionController
        selectInteractionFlag: LabelSequenceInteractionController.Select
        selectionController: labelSequence.selectionController
        timeManipulator: timeManipulator
    }

    GenericBackPointerMouseArea {
        id: backPointerMouseArea

        controller: labelSequence.labelSequenceInteractionController
        selectByRubberBandInteractionFlag: LabelSequenceInteractionController.SelectByRubberBand
        selectInteractionFlag: LabelSequenceInteractionController.Select
        timeManipulator: timeManipulator
        rubberBandLayer: rubberBandLayer
        selectionController: labelSequence.selectionController
        mapPoint: (p) => Qt.point(mapToItem(rubberBandLayer, p.x, 0).x, rubberBandLayer.started ? height : 0)
    }

    GenericBackHoverMouseArea {
        id: hoverMouseArea

        controller: labelSequence.labelSequenceInteractionController
        target: labelSequence
        timeManipulator: timeManipulator

    }

    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: labelSequence.timeViewModel
        timeLayoutViewModel: labelSequence.timeLayoutViewModel

        Item {
            anchors.fill: parent
            SequenceSlicer {
                id: slicer
                leftOutBound: 256
                viewModel: labelSequence.labelSequenceViewModel
                sliceWidth: labelSequence.width
                timeLayoutViewModel: labelSequence.timeLayoutViewModel
                timeViewModel: labelSequence.timeViewModel

                delegate: LabelSequenceDelegate {
                    id: labelRect
                    function handleRubberBand() {
                        if (visible)
                            rubberBandLayer.insertItem(labelViewModel, Qt.rect(x, 0, width, 1 << 20));
                        else
                            rubberBandLayer.removeItem(labelViewModel);
                    }

                    height: parent.height
                    labelSequenceViewModel: labelSequence.labelSequenceViewModel
                    selectionController: labelSequence.selectionController

                    Component.onDestruction: rubberBandLayer.removeItem(labelViewModel)
                    onVisibleChanged: handleRubberBand()
                    onWidthChanged: handleRubberBand()
                    onXChanged: handleRubberBand()

                    Binding {
                        labelRect.x: labelRect.labelViewModel.position * (labelSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                        when: labelRect.SequenceSlicerLoader.inRange
                    }

                    readonly property TimeManipulator timeManipulator_: timeManipulator
                    GenericRightButtonMouseArea {
                        id: rightButtonMouseArea

                        controller: labelSequence.labelSequenceInteractionController
                        selectInteractionFlag: LabelSequenceInteractionController.Select
                        selectionController: labelSequence.selectionController
                        paneItem: labelSequence
                        viewModel: labelRect.labelViewModel
                    }
                    GenericHoverMouseArea {
                        id: hoverMouseArea

                        controller: labelSequence.labelSequenceInteractionController
                        paneItem: labelSequence
                        viewModel: labelRect.labelViewModel
                    }
                    GenericPointerMouseArea {
                        id: pointerMouseArea
                        controller: labelSequence.labelSequenceInteractionController
                        moveInteractionFlag: LabelSequenceInteractionController.Move
                        selectInteractionFlag: LabelSequenceInteractionController.Select
                        paneItem: labelSequence
                        viewModel: labelRect.labelViewModel
                        sequenceViewModel: labelSequence.labelSequenceViewModel
                        timeManipulator: labelRect.timeManipulator_
                        selectionController: labelSequence.selectionController
                    }

                    Connections {
                        id: cursorIndicatorBinding

                        function onPositionChanged() {
                            labelSequence.timeLayoutViewModel.cursorPosition = labelRect.labelViewModel.position;
                        }

                        enabled: false
                        target: labelRect.labelViewModel
                    }
                }
            }
        }
        RubberBandLayer {
            id: rubberBandLayer

            anchors.fill: parent
            selectionController: labelSequence.selectionController
            z: 2

            rubberBand: RubberBandRectangle {
            }
        }
        ItemPopupEdit {
            id: inPlaceEditPopup

            readonly property Item associatedItem: slicer.itemForModel(model)
            containerModel: labelSequence.labelSequenceViewModel
            targetProperty: "content"
            radius: 2
            width: associatedItem?.width ?? 0
            height: parent.height
            x: associatedItem?.x ?? 0

            onEditPreviousRequested: labelSequence.labelSequenceInteractionController.inPlaceEditOperationTriggered(labelSequence, model, LabelSequenceInteractionController.MovePrevious)
            onEditNextRequested: labelSequence.labelSequenceInteractionController.inPlaceEditOperationTriggered(labelSequence, model, LabelSequenceInteractionController.MoveNext)
            onEditHomeRequested: labelSequence.labelSequenceInteractionController.inPlaceEditOperationTriggered(labelSequence, model, LabelSequenceInteractionController.MoveHome)
            onEditEndRequested: labelSequence.labelSequenceInteractionController.inPlaceEditOperationTriggered(labelSequence, model, LabelSequenceInteractionController.MoveEnd)
            onAccepted: labelSequence.labelSequenceInteractionController.inPlaceEditOperationTriggered(labelSequence, model, LabelSequenceInteractionController.CommitEditing)
            onRejected: labelSequence.labelSequenceInteractionController.inPlaceEditOperationTriggered(labelSequence, model, LabelSequenceInteractionController.AbortEditing)
            onVisibleChanged: () => {
                if (associatedItem)
                    associatedItem.editing = visible
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
