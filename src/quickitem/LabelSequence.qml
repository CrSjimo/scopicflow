import ScopicFlowPrivate

import QtQml
import QtQuick

import "./HelperComponents"
import "qrc:/ScopicFlow/modules/dev/sjimo/ScopicFlow/Palettes" as ScopicFlowPalette

LabelSequence {
    id: labelSequence

    readonly property QtObject defaultPalette: ScopicFlowPalette.LabelSequence {}

    readonly property QtObject palette: paletteViewModel?.palette?.labelSequence ?? defaultPalette

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: labelSequence.timeAlignmentViewModel
        animationViewModel: labelSequence.animationViewModel
    }

    Rectangle {
        anchors.fill: parent
        color: labelSequence.palette.backgroundColor
    }

    Item {
        id: viewport
        readonly property double start: labelSequence.timeAlignmentViewModel?.start ?? 0
        readonly property double end: labelSequence.timeAlignmentViewModel?.end ?? 0
        readonly property double pixelDensity: labelSequence.timeAlignmentViewModel?.pixelDensity ?? 0
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        x: -start * pixelDensity
        width: end * pixelDensity
        clip: true
        Repeater {
            id: labelRepeater
            model: labelSequence.model
            property QtObject currentItem: labelSequence.currentItem
            Rectangle {
                required property QtObject modelData
                readonly property QtObject labelViewModel: modelData
                id: labelRect
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                x: labelViewModel.position * viewport.pixelDensity
                z: labelViewModel === labelRepeater.currentItem
                color: labelViewModel.selected ? labelSequence.palette.labelSelectedColor: labelSequence.palette.labelColor
                border.width: 1
                border.color: labelViewModel.selected ? labelSequence.palette.labelSelectedBorderColor : labelSequence.palette.labelBorderColor
                width: labelText.width + 8
                Text {
                    id: labelText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 4
                    text: labelViewModel.content
                    color: labelViewModel.selected ? labelSequence.palette.labelSelectedTextColor : labelSequence.palette.labelTextColor
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            let multipleSelect = Boolean(mouse.modifiers & Qt.ControlModifier)
                            let extendingSelect = Boolean(mouse.modifiers & Qt.ShiftModifier)
                            let previousSelected = labelRect.labelViewModel.selected
                            let previousSelectionCount = 0
                            if (!multipleSelect) {
                                previousSelectionCount = labelSequence.deselectAll()
                            }
                            if (extendingSelect) {
                                labelSequence.extendSelection(labelRect.labelViewModel)
                            } else {
                                labelRect.labelViewModel.selected = previousSelectionCount > 1 || !previousSelected
                                labelSequence.currentItem = labelRect.labelViewModel
                            }

                        }
                    }
                }
            }
        }
    }

    PositionIndicators {
        anchors.fill: parent
        palette: labelSequence.palette
        timeViewModel: labelSequence.timeAlignmentViewModel
        playbackViewModel: labelSequence.playbackViewModel
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: labelSequence.scrollBehaviorViewModel
        onZoomed: function (ratioX, _, x, _, isPhysicalWheel) {
            timeManipulator.zoomOnWheel(ratioX, x, isPhysicalWheel)
        }
        onMoved: function (x, _, isPhysicalWheel) {
            timeManipulator.moveViewBy(x, isPhysicalWheel)
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: labelSequence.scrollBehaviorViewModel
        onMoved: function (x) {
            timeManipulator.moveViewBy(x)
        }
    }
}