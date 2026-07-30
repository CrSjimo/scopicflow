pragma ComponentBehavior: Bound

import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: phonemeSequence

    property PointSequenceViewModel phonemeSequenceViewModel: null
    property RangeSequenceViewModel noteSequenceViewModel: null
    property ClipViewModel clipViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property PhonemeSequenceInteractionController phonemeSequenceInteractionController: null

    property int textHorizontalAlignment: Text.AlignLeft

    clip: true
    implicitHeight: 20

    function editInPlace(phonemeViewModel: PhonemeViewModel) {
        inPlaceEditPopup.close()
        if (!phonemeViewModel) {
            return
        }
        const associatedItem = phonemeSlicer.itemForModel(phonemeViewModel)
        if (!associatedItem || !associatedItem.visible) {
            return
        }
        inPlaceEditPopup.associatedItem = associatedItem
        inPlaceEditPopup.model = phonemeViewModel
        phonemeSequence.phonemeSequenceInteractionController?.inPlaceEditOperationTriggered(
            phonemeSequence, phonemeViewModel,
            PhonemeSequenceInteractionController.StartEditing)
        inPlaceEditPopup.open()
    }

    Keys.onMenuPressed: () => {
        phonemeSequence.phonemeSequenceInteractionController?.contextMenuRequested(
            phonemeSequence, -1)
    }

    ClipMappedProxyTimeViewModel {
        id: proxyTimeViewModel

        clipViewModel: phonemeSequence.clipViewModel
        timeViewModel: phonemeSequence.timeViewModel
    }

    TimeManipulator {
        id: timeManipulator

        target: phonemeSequence
        timeLayoutViewModel: phonemeSequence.timeLayoutViewModel
        timeViewModel: proxyTimeViewModel
    }

    GenericBackRightButtonMouseArea {
        controller: phonemeSequence.phonemeSequenceInteractionController
        target: phonemeSequence
        timeManipulator: timeManipulator
    }

    DispatcherMouseArea {
        onDoubleClicked: mouse => {
            phonemeSequence.phonemeSequenceInteractionController?.doubleClicked(
                phonemeSequence, timeManipulator.mapToPosition(mouse.x))
        }
    }

    HoverHandler {
        blocking: false
        cursorShape: undefined

        onHoveredChanged: {
            const controller =
                phonemeSequence.phonemeSequenceInteractionController
            if (!controller)
                return
            if (hovered) {
                controller.hoverEntered(
                    phonemeSequence,
                    timeManipulator.mapToPosition(point.position.x))
            } else {
                controller.hoverExited(phonemeSequence)
            }
        }

        onPointChanged: {
            if (hovered) {
                phonemeSequence.phonemeSequenceInteractionController?.hoverMoved(
                    phonemeSequence,
                    timeManipulator.mapToPosition(point.position.x))
            }
        }
    }

    TimeViewportContainer {
        id: viewportContainer

        timeLayoutViewModel: phonemeSequence.timeLayoutViewModel
        timeViewModel: proxyTimeViewModel

        Item {
            id: noteBoundaryLayer

            anchors.fill: parent
            z: 0

            SequenceSlicer {
                id: noteSlicer

                sliceWidth: phonemeSequence.width
                timeLayoutViewModel: phonemeSequence.timeLayoutViewModel
                timeViewModel: proxyTimeViewModel
                viewModel: phonemeSequence.noteSequenceViewModel

                delegate: Item {
                    id: noteBoundaryDelegate

                    readonly property NoteViewModel noteViewModel: SequenceSlicerLoader.viewModel

                    height: parent.height
                    visible: SequenceSlicerLoader.inRange

                    Binding {
                        noteBoundaryDelegate.width: (noteBoundaryDelegate.noteViewModel?.length ?? 0) * (phonemeSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                        noteBoundaryDelegate.x: (noteBoundaryDelegate.noteViewModel?.position ?? 0) * (phonemeSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                        when: noteBoundaryDelegate.SequenceSlicerLoader.inRange
                    }

                    Rectangle {
                        x: -0.5
                        width: 1
                        height: parent.height
                        color: Theme.foregroundSecondaryColor
                    }

                    Rectangle {
                        x: parent.width - 0.5
                        width: 1
                        height: parent.height
                        color: Theme.foregroundSecondaryColor
                    }
                }
            }
        }

        Item {
            id: phonemeLayer

            anchors.fill: parent
            z: 1

            SequenceSlicer {
                id: phonemeSlicer

                leftOutBound: 256
                rightOutBound: 2
                sliceWidth: phonemeSequence.width
                timeLayoutViewModel: phonemeSequence.timeLayoutViewModel
                timeViewModel: proxyTimeViewModel
                viewModel: phonemeSequence.phonemeSequenceViewModel

                delegate: PhonemeSequenceDelegate {
                    id: phonemeDelegate

                    height: parent.height
                    textHorizontalAlignment: phonemeSequence.textHorizontalAlignment

                    Binding {
                        phonemeDelegate.content: phonemeDelegate.phonemeViewModel?.content ?? ""
                        phonemeDelegate.edited: phonemeDelegate.phonemeViewModel?.isEdited ?? false
                        phonemeDelegate.textAreaWidth: {
                            const viewModel = phonemeDelegate.phonemeViewModel
                            const noteViewModel = viewModel?.associatedNote ?? null
                            if (!viewModel || !noteViewModel) {
                                return 0
                            }
                            const endPosition = viewModel.nextPhoneme
                                ? viewModel.nextPhoneme.sequencePosition
                                : noteViewModel.position + noteViewModel.length
                            return Math.max(0, (endPosition - viewModel.sequencePosition)
                                * (phonemeSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                                - phonemeDelegate.width / 2)
                        }
                        phonemeDelegate.x: (phonemeDelegate.phonemeViewModel?.sequencePosition ?? 0)
                            * (phonemeSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                            - phonemeDelegate.width / 2
                        when: phonemeDelegate.SequenceSlicerLoader.inRange
                    }

                    Item {
                        id: itemInteractionArea

                        x: -2
                        width: Math.max(
                            6, phonemeDelegate.width
                               + phonemeDelegate.textAreaWidth + 2)
                        height: parent.height
                        enabled: !phonemeDelegate.editing

                        HoverHandler {
                            blocking: false

                            onHoveredChanged: {
                                const controller =
                                    phonemeSequence.phonemeSequenceInteractionController
                                if (!controller)
                                    return
                                if (hovered) {
                                    controller.itemHoverEntered(
                                        phonemeSequence,
                                        phonemeDelegate.phonemeViewModel)
                                } else {
                                    controller.itemHoverExited(
                                        phonemeSequence,
                                        phonemeDelegate.phonemeViewModel)
                                }
                            }
                        }

                        GenericRightButtonMouseArea {
                            enabled:
                                phonemeSequence.phonemeSequenceInteractionController
                                !== null
                            controller:
                                phonemeSequence.phonemeSequenceInteractionController
                            paneItem: phonemeSequence
                            viewModel: phonemeDelegate.phonemeViewModel
                        }

                        DispatcherMouseArea {
                            determineDragHandler: mouse => {
                                if (mouse.x < 0
                                        || mouse.x >= dragHandleArea.width) {
                                    return null
                                }
                                const controller =
                                    phonemeSequence.phonemeSequenceInteractionController
                                if (!controller)
                                    return null
                                const interaction =
                                    mouse.modifiers & Qt.AltModifier
                                    ? controller.secondaryItemInteraction
                                    : controller.primaryItemInteraction
                                return interaction
                                        === PhonemeSequenceInteractionController.Move
                                    ? moveDragHandler : null
                            }

                            onDoubleClicked: () =>
                                phonemeSequence.phonemeSequenceInteractionController?.itemDoubleClicked(
                                    phonemeSequence, phonemeDelegate.phonemeViewModel)
                        }

                        Item {
                            id: dragHandleArea

                            width: 6
                            height: parent.height

                            DispatchedDragHandler {
                                id: moveDragHandler

                                CursorBinding.cursorShape: Qt.SizeHorCursor
                                CursorBinding.enabled: dragged

                                onDragStarted: () => {
                                    phonemeSequence.phonemeSequenceInteractionController?.movingStarted(
                                        phonemeSequence, phonemeDelegate.phonemeViewModel)
                                }

                                onDragMoved: (x) => {
                                    const viewModel = phonemeDelegate.phonemeViewModel
                                    const noteViewModel = viewModel?.associatedNote ?? null
                                    const pixelDensity = phonemeSequence.timeLayoutViewModel?.pixelDensity ?? 0
                                    if (!viewModel || !noteViewModel || pixelDensity <= 0) {
                                        return
                                    }
                                    const centerPoint = moveDragHandler.mapToItem(
                                        phonemeSequence,
                                        x - moveDragHandler.startPoint.x
                                            + moveDragHandler.width / 2,
                                        0)
                                    const sequencePosition = proxyTimeViewModel.start
                                        + centerPoint.x / pixelDensity
                                    viewModel.position = sequencePosition
                                        - noteViewModel.position
                                }

                                onDragFinished: () => {
                                    phonemeSequence.phonemeSequenceInteractionController?.movingCommitted(
                                        phonemeSequence, phonemeDelegate.phonemeViewModel)
                                }

                                onDragCanceled: () => {
                                    phonemeSequence.phonemeSequenceInteractionController?.movingAborted(
                                        phonemeSequence, phonemeDelegate.phonemeViewModel)
                                }
                            }

                            HoverHandler {
                                blocking: false
                                cursorShape: Qt.SizeHorCursor
                            }
                        }
                    }
                }
            }
        }

        ItemPopupEdit {
            id: inPlaceEditPopup

            property Item associatedItem: null

            containerModel: phonemeSequence.phonemeSequenceViewModel
            targetProperty: "content"
            radius: 2
            width: associatedItem?.implicitWidth ?? 0
            height: associatedItem?.implicitHeight ?? 0
            x: (associatedItem?.x ?? 0) + (associatedItem?.width ?? 0)
            y: (parent.height - height) / 2
            z: 2

            onAccepted: {
                phonemeSequence.phonemeSequenceInteractionController?.inPlaceEditOperationTriggered(
                    phonemeSequence, model,
                    PhonemeSequenceInteractionController.CommitEditing)
            }

            onRejected: {
                phonemeSequence.phonemeSequenceInteractionController?.inPlaceEditOperationTriggered(
                    phonemeSequence, model,
                    PhonemeSequenceInteractionController.AbortEditing)
            }

            onVisibleChanged: () => {
                if (associatedItem) {
                    associatedItem.editing = visible
                }
            }
        }
    }
}
