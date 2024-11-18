import ScopicFlowPrivate

import QtQml
import QtQuick
import QtQuick.Controls.Basic

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

    MouseArea {
        anchors.fill: parent
        onClicked: function (mouse) {
            labelSequence.currentItem = null
            labelSequence.deselectAll()
        }
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
                readonly property bool isCurrent: labelViewModel === labelRepeater.currentItem
                id: labelRect
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                x: labelViewModel.position * viewport.pixelDensity
                z: isCurrent ? 1 : 0
                color: labelViewModel.selected ? labelSequence.palette.labelSelectedColor: labelSequence.palette.labelColor
                Behavior on color {
                    ColorAnimation {
                        duration: (labelSequence.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                        easing.type: Easing.OutCubic
                    }
                }
                border.width: 1
                border.color: labelViewModel.selected ? labelSequence.palette.labelSelectedBorderColor : labelSequence.palette.labelBorderColor
                Behavior on border.color {
                    ColorAnimation {
                        duration: (labelSequence.animationViewModel?.visualEffectAnimationRatio ?? 1.0) * 250
                        easing.type: Easing.OutCubic
                    }
                }
                width: labelEdit.visible ? labelEdit.width : labelText.width + 8
                clip: true
                Text {
                    id: labelText
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.leftMargin: 4
                    text: labelViewModel.content
                    color: labelViewModel.selected ? labelSequence.palette.labelSelectedTextColor : labelSequence.palette.labelTextColor
                    Behavior on color {
                        ColorAnimation {
                            duration: (labelSequence.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    function selectItem (multipleSelect, extendingSelect) {
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
                    property double pressedDeltaX: 0
                    property bool rejectClick: false
                    DragScroller {
                        id: dragScroller
                        onMoved: function (deltaX) {
                            timeManipulator.moveViewBy(deltaX)
                            labelSequence.moveSelectedLabelOnDragScrolling(deltaX < 0, labelRect.labelViewModel)
                        }
                    }
                    onPressed: function (mouse) {
                        rejectClick = false
                        pressedDeltaX = mouse.x
                    }
                    onClicked: function (mouse) {
                        if (rejectClick)
                            return
                        let multipleSelect = Boolean(mouse.modifiers & Qt.ControlModifier)
                        let extendingSelect = Boolean(mouse.modifiers & Qt.ShiftModifier)
                        selectItem(multipleSelect, extendingSelect)
                    }
                    onDoubleClicked: function (mouse) {
                        selectItem(false, false)
                        labelEdit.visible = true
                        labelEdit.focus = true
                    }
                    onPositionChanged: function (mouse) {
                        rejectClick = true
                        if (!labelRect.labelViewModel.selected) {
                            let multipleSelect = Boolean(mouse.modifiers & Qt.ControlModifier)
                            let extendingSelect = Boolean(mouse.modifiers & Qt.ShiftModifier)
                            selectItem(multipleSelect, extendingSelect)
                        }
                        let parentX = labelRect.mapToItem(labelSequence, mouse.x, mouse.y).x
                        if (parentX < 0) {
                            dragScroller.distanceX = parentX
                            dragScroller.running = true
                        } else if (parentX > labelSequence.width) {
                            dragScroller.distanceX = parentX - labelSequence.width
                            dragScroller.running = true
                        } else {
                            labelSequence.moveSelectedLabelsTo(parentX - pressedDeltaX, labelRect.labelViewModel)
                            dragScroller.running = false
                        }

                    }
                    onReleased: function (mouse) {
                        dragScroller.running = false
                    }
                }

                onIsCurrentChanged: {
                    if (!isCurrent)
                        labelEdit.visible = false
                }

                TextField {
                    id: labelEdit
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    color: labelSequence.palette.labelEditingTextColor
                    leftPadding: 4
                    rightPadding: 4
                    topPadding: 0
                    bottomPadding: 0
                    visible: false
                    onVisibleChanged: {
                        labelRect.labelViewModel.content = text
                    }
                    text: labelRect.labelViewModel.content
                    background: Rectangle {
                        color: labelSequence.palette.labelEditingColor
                        border.width: 1
                        border.color: labelSequence.palette.labelEditingBorderColor
                    }
                    Keys.onEscapePressed: {
                        text = labelRect.labelViewModel.content
                        visible = false
                    }
                    Keys.onReturnPressed: {
                        visible = false
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