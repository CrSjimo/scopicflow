pragma ComponentBehavior: Bound

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

    Connections {
        target: labelSequence.selectionController
        function onCurrentItemChanged() {
            let oldItem = slicer.itemForModel(helper.lastCurrentItem)
            if (oldItem) {
                oldItem.current = false
            }
            let newItem = slicer.itemForModel(labelSequence.selectionController.currentItem)
            if (newItem) {
                newItem.current = true
            }
            helper.lastCurrentItem = labelSequence.selectionController.currentItem
        }
    }

    QtObject {
        id: helper

        readonly property Text labelLengthReference: Text {
            id: labelLengthReference

            visible: false
        }

        property LabelViewModel lastCurrentItem: null
        Component.onCompleted: {
            lastCurrentItem = labelSequence.selectionController?.currentItem ?? null
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
    RubberBandDragHandler {
        id: rubberBandDragHandler
        controller: labelSequence.labelSequenceInteractionController
        rubberBandLayer: rubberBandLayer
        selectionController: labelSequence.selectionController
        target: labelSequence
        timeManipulator: timeManipulator
        mode: RubberBandDragHandler.TimeRange
    }
    GenericComboSceneMouseArea {
        controller: labelSequence.labelSequenceInteractionController
        selectionController: labelSequence.selectionController
        target: labelSequence
        timeManipulator: timeManipulator
        dispatchMap: ({
            [LabelSequenceInteractionController.RubberBandSelect]: rubberBandDragHandler,
        })
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

                    height: parent.height
                    selectionController: labelSequence.selectionController

                    onLabelViewModelChanged: () => {
                        current = (labelSequence.selectionController?.currentItem === labelViewModel)
                    }

                    RubberBandItemConnections {
                        target: labelRect
                        viewModel: labelRect.labelViewModel
                        rubberBandLayer: rubberBandLayer
                    }
                    Binding {
                        labelRect.x: labelRect.labelViewModel.position * (labelSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                        when: labelRect.SequenceSlicerLoader.inRange
                    }
                    component LabelMoveDragHandler: MoveDragHandler {
                        controller: labelSequence.labelSequenceInteractionController
                        selectionController: labelSequence.selectionController
                        paneItem: labelSequence
                        viewModel: labelRect.labelViewModel
                        timeManipulator: timeManipulator
                    }
                    LabelMoveDragHandler {
                        id: moveDragHandler
                        moveFlag: LabelSequenceInteractionController.MF_Move
                    }
                    LabelMoveDragHandler {
                        id: copyAndMoveDragHandler
                        moveFlag: LabelSequenceInteractionController.MF_CopyAndMove
                    }
                    GenericComboItemMouseArea {
                        controller: labelSequence.labelSequenceInteractionController
                        selectionController: labelSequence.selectionController
                        paneItem: labelSequence
                        viewModel: labelRect.labelViewModel
                        dispatchMap: ({
                            [LabelSequenceInteractionController.Move]: moveDragHandler,
                            [LabelSequenceInteractionController.CopyAndMove]: copyAndMoveDragHandler,
                            [LabelSequenceInteractionController.RubberBandSelect]: rubberBandDragHandler,
                        })
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
