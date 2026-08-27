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
    readonly property alias pointerTimePosition: helper.pointerTimePosition

    property int textHorizontalAlignment: Text.AlignLeft

    onPhonemeSequenceViewModelChanged: pointerRouter.cancel()
    onClipViewModelChanged: pointerRouter.cancel()

    function hasPointerHover(): bool {
        return pointerRouter.hasHover()
    }

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

    PointerInteractionRouter {
        id: pointerRouter
    }

    QtObject {
        id: helper

        property int pointerTimePosition: 0

        function updateHoverPointer(event) {
            pointerTimePosition =
                timeManipulator.mapToPosition(event.position.x)
        }
    }

    DispatchedDragHandler {
        id: moveDragHandler

        property PhonemeViewModel activeViewModel: null

        onStarted: (event, hit) => {
            activeViewModel = hit.target
            phonemeSequence.phonemeSequenceInteractionController?.movingStarted(
                phonemeSequence, activeViewModel)
        }

        onMoved: event => {
            const viewModel = activeViewModel
            const noteViewModel = viewModel?.associatedNote ?? null
            const pixelDensity = phonemeSequence.timeLayoutViewModel?.pixelDensity ?? 0
            if (!viewModel || !noteViewModel || pixelDensity <= 0)
                return
            const handleCenter = hit.targetRect.x + 3
                + event.position.x - pressPosition.x
            const sequencePosition = proxyTimeViewModel.start
                + handleCenter / pixelDensity
            viewModel.position = sequencePosition - noteViewModel.position
        }

        onFinished: {
            if (activeViewModel) {
                phonemeSequence.phonemeSequenceInteractionController?.movingCommitted(
                    phonemeSequence, activeViewModel)
            }
            activeViewModel = null
        }

        onCanceled: {
            if (activeViewModel) {
                phonemeSequence.phonemeSequenceInteractionController?.movingAborted(
                    phonemeSequence, activeViewModel)
            }
            activeViewModel = null
        }
    }

    PointerInputArea {
        anchors.fill: parent
        router: pointerRouter
        coordinateSpace: phonemeSequence
    }

    Connections {
        target: pointerRouter

        function onDoubleClicked(event, hit) {
            const controller = phonemeSequence.phonemeSequenceInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemDoubleClicked(phonemeSequence, hit.target)
            } else {
                controller.doubleClicked(
                    phonemeSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onContextMenuRequested(event, hit) {
            const controller = phonemeSequence.phonemeSequenceInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemContextMenuRequested(phonemeSequence, hit.target)
            } else {
                controller.contextMenuRequested(
                    phonemeSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onHoverEntered(event, hit) {
            helper.updateHoverPointer(event)
            const controller = phonemeSequence.phonemeSequenceInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemHoverEntered(phonemeSequence, hit.target)
            } else {
                controller.hoverEntered(
                    phonemeSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onHoverMoved(event, hit) {
            helper.updateHoverPointer(event)
            if (!hit.target) {
                phonemeSequence.phonemeSequenceInteractionController?.hoverMoved(
                    phonemeSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onHoverExited(hit) {
            const controller = phonemeSequence.phonemeSequenceInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemHoverExited(phonemeSequence, hit.target)
            } else {
                controller.hoverExited(phonemeSequence)
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

                        PointerInputArea {
                            anchors.fill: parent
                            router: pointerRouter
                            coordinateSpace: phonemeSequence
                            hitResolver: (surfacePoint, localPoint) => {
                                const origin = itemInteractionArea.mapToItem(
                                    phonemeSequence, 0, 0)
                                return {
                                    valid: true,
                                    target: phonemeDelegate.phonemeViewModel,
                                    targetRect: Qt.rect(
                                        origin.x, origin.y,
                                        itemInteractionArea.width,
                                        itemInteractionArea.height),
                                    hoverRegion: 0,
                                    payload: localPoint.x >= 0 && localPoint.x < 6,
                                }
                            }
                            handlerResolver: (event, hit) => {
                                if (!hit.payload)
                                    return null
                                const controller =
                                    phonemeSequence.phonemeSequenceInteractionController
                                if (!controller)
                                    return null
                                const interaction = event.modifiers & Qt.AltModifier
                                    ? controller.secondaryItemInteraction
                                    : controller.primaryItemInteraction
                                return interaction
                                        === PhonemeSequenceInteractionController.Move
                                    ? moveDragHandler : null
                            }
                            cursorResolver: hit => hit.payload
                                ? Qt.SizeHorCursor : undefined
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
