pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Layouts

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: rangeIndicatorSequence

    property RangeSequenceViewModel rangeIndicatorSequenceViewModel: null
    property RangeIndicatorInteractionController rangeIndicatorInteractionController: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null

    onRangeIndicatorSequenceViewModelChanged: pointerRouter.cancel()

    clip: true
    implicitHeight: 20

    Keys.onMenuPressed: () => {
        rangeIndicatorSequence.rangeIndicatorInteractionController?.contextMenuRequested(
            rangeIndicatorSequence, -1)
    }

    TimeManipulator {
        id: timeManipulator

        target: rangeIndicatorSequence
        timeLayoutViewModel: rangeIndicatorSequence.timeLayoutViewModel
        timeViewModel: rangeIndicatorSequence.timeViewModel
    }

    Rectangle {
        anchors.fill: parent
        color: Theme.backgroundColor(rangeIndicatorSequence.ThemedItem.backgroundLevel)
    }

    PointerInteractionRouter {
        id: pointerRouter
    }

    PointerInputArea {
        anchors.fill: parent
        router: pointerRouter
        coordinateSpace: rangeIndicatorSequence
    }

    Connections {
        target: pointerRouter

        function onClicked(event, hit) {
            const controller = rangeIndicatorSequence.rangeIndicatorInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemClicked(rangeIndicatorSequence, hit.target)
            } else {
                controller.clicked(
                    rangeIndicatorSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onDoubleClicked(event, hit) {
            const controller = rangeIndicatorSequence.rangeIndicatorInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemDoubleClicked(rangeIndicatorSequence, hit.target)
            } else {
                controller.doubleClicked(
                    rangeIndicatorSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onContextMenuRequested(event, hit) {
            const controller = rangeIndicatorSequence.rangeIndicatorInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemContextMenuRequested(rangeIndicatorSequence, hit.target)
            } else {
                controller.contextMenuRequested(
                    rangeIndicatorSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onHoverEntered(event, hit) {
            const controller = rangeIndicatorSequence.rangeIndicatorInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemHoverEntered(rangeIndicatorSequence, hit.target)
            } else {
                controller.hoverEntered(
                    rangeIndicatorSequence,
                    timeManipulator.mapToPosition(event.position.x))
            }
        }

        function onHoverMoved(event, hit) {
            const controller = rangeIndicatorSequence.rangeIndicatorInteractionController
            if (!controller || hit.target)
                return
            controller.hoverMoved(
                rangeIndicatorSequence,
                timeManipulator.mapToPosition(event.position.x))
        }

        function onHoverExited(hit) {
            const controller = rangeIndicatorSequence.rangeIndicatorInteractionController
            if (!controller)
                return
            if (hit.target) {
                controller.itemHoverExited(rangeIndicatorSequence, hit.target)
            } else {
                controller.hoverExited(rangeIndicatorSequence)
            }
        }
    }

    TimeViewportContainer {
        timeViewModel: rangeIndicatorSequence.timeViewModel
        timeLayoutViewModel: rangeIndicatorSequence.timeLayoutViewModel

        Item {
            anchors.fill: parent

            SequenceSlicer {
                id: slicer

                leftOutBound: 256
                viewModel: rangeIndicatorSequence.rangeIndicatorSequenceViewModel
                sliceWidth: rangeIndicatorSequence.width
                timeLayoutViewModel: rangeIndicatorSequence.timeLayoutViewModel
                timeViewModel: rangeIndicatorSequence.timeViewModel

                delegate: Rectangle {
                    id: rangeIndicatorDelegate

                    readonly property RangeIndicatorViewModel rangeIndicatorViewModel:
                        SequenceSlicerLoader.viewModel
                    readonly property real pixelDensity:
                        rangeIndicatorSequence.timeLayoutViewModel?.pixelDensity ?? 0
                    readonly property real headerMargin:
                        ((rangeIndicatorSequence.timeViewModel?.start ?? 0)
                         - (rangeIndicatorViewModel?.position ?? 0)) * pixelDensity

                    x: (rangeIndicatorViewModel?.position ?? 0) * pixelDensity
                    width: (rangeIndicatorViewModel?.length ?? 0) * pixelDensity
                    height: rangeIndicatorSequence.height
                    visible: SequenceSlicerLoader.inRange
                    color: Theme.controlColor(
                        rangeIndicatorViewModel?.type ?? SVS.CT_Normal)
                    border.color: Theme.borderColor

                    RowLayout {
                        anchors.left: parent.left
                        anchors.leftMargin: Math.max(
                            2,
                            Math.min(
                                rangeIndicatorDelegate.headerMargin,
                                parent.width - width))
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 4
                        width: Math.min(implicitWidth, rangeIndicatorDelegate.width)

                        Text {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.fillWidth: true
                            leftPadding: 4
                            rightPadding: 4
                            text: rangeIndicatorDelegate.rangeIndicatorViewModel?.content ?? ""
                            font: Theme.font
                            color: Theme.foregroundPrimaryColor
                            elide: Text.ElideRight
                        }
                    }

                    PointerInputArea {
                        anchors.fill: parent
                        router: pointerRouter
                        coordinateSpace: rangeIndicatorSequence
                        hitResolver: (_surfacePoint, _localPoint) => {
                            const origin = rangeIndicatorDelegate.mapToItem(
                                rangeIndicatorSequence, 0, 0)
                            return {
                                valid: true,
                                target: rangeIndicatorDelegate.rangeIndicatorViewModel,
                                targetRect: Qt.rect(
                                    origin.x, origin.y,
                                    rangeIndicatorDelegate.width,
                                    rangeIndicatorDelegate.height),
                                hoverRegion: 0,
                                payload: undefined,
                            }
                        }
                    }
                }
            }
        }
    }

    StandardScrollHandler {
        movableOrientation: Qt.Horizontal
        viewModel: rangeIndicatorSequence.scrollBehaviorViewModel

        onMoved: (x, _, isPhysicalWheel) =>
            timeManipulator.moveViewBy(x, isPhysicalWheel)
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) =>
            timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel)
    }
}
