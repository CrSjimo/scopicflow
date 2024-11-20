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
        anchors.leftMargin: -1
        anchors.rightMargin: -1
        color: labelSequence.palette.backgroundColor
        border.width: 1
        border.color: labelSequence.palette.borderColor
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

        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            property double pressedX: 0
            property int nextIndex: -1
            property bool rejectClick: false
            function doDragRubberBand(targetX) {
                if (targetX >= pressedX) {
                    rubberBand.x = pressedX
                    rubberBand.width = targetX - pressedX
                    for (let i = nextIndex - 1; i >= 0; i--) {
                        let item = labelRepeater.itemAt(i)
                        if (item.labelViewModel.selected)
                            item.labelViewModel.selected = false
                        else
                            break
                    }
                    for (let i = nextIndex; i < labelRepeater.count; i++) {
                        let item = labelRepeater.itemAt(i)
                        if (targetX >= item.x)
                            item.labelViewModel.selected = true
                        else if (item.labelViewModel.selected)
                            item.labelViewModel.selected = false
                        else
                            break
                    }
                } else {
                    rubberBand.x = targetX
                    rubberBand.width = pressedX - targetX
                    for (let i = nextIndex; i < labelRepeater.count; i++) {
                        let item = labelRepeater.itemAt(i)
                        if (item.labelViewModel.selected)
                            item.labelViewModel.selected = false
                        else
                            break
                    }
                    for (let i = nextIndex - 1; i >= 0; i--) {
                        let item = labelRepeater.itemAt(i)
                        if (targetX <= item.x + item.width)
                            item.labelViewModel.selected = true
                        else if (item.labelViewModel.selected)
                            item.labelViewModel.selected = false
                        else
                            break
                    }
                }
            }
            DragScroller {
                id: dragScroller
                onMoved: function (deltaX) {
                    timeManipulator.moveViewBy(deltaX)
                    if (deltaX > 0) {
                        parent.doDragRubberBand(labelSequence.mapToItem(parent, labelSequence.width, 0).x)
                    } else {
                        parent.doDragRubberBand(labelSequence.mapToItem(parent, 0, 0).x)
                    }
                }
            }
            onPressed: function (mouse) {
                rejectClick = false
                pressedX = mouse.x
            }
            onClicked: function (mouse) {
                if (mouse.button === Qt.LeftButton) {
                    if (rejectClick)
                        return
                    labelSequence.currentItem = null
                    labelSequence.deselectAll()
                } else {
                    labelSequence.contextMenuRequested(Math.round(mouse.x / labelSequence.timeAlignmentViewModel.pixelDensity))
                }
            }
            onDoubleClicked: function (mouse) {
                if (mouse.button !== Qt.LeftButton)
                    return
                let label = labelSequence.insertLabelTo(mouse.x, "")
                if (!labelRepeater.itemDict.has(label))
                    return
                let item = labelRepeater.itemAt(labelRepeater.itemDict.get(label))
                item.selectItem(false, false, true)
                item.editing = true

            }
            onPositionChanged: function (mouse) {
                rejectClick = true
                rubberBand.visible = true
                if (nextIndex === -1 && labelRepeater.count) {
                    labelSequence.deselectAll()
                    labelSequence.modelChanged()
                    for (let i = 0; i < labelRepeater.count; i++) {
                        let item = labelRepeater.itemAt(i)
                        if (item.x >= pressedX) {
                            nextIndex = i
                            break
                        }
                    }
                    if (nextIndex === -1)
                        nextIndex = labelRepeater.count
                }
                let parentX = mapToItem(labelSequence, mouse.x, mouse.y).x
                let deltaX = 0
                if (parentX < 0) {
                    dragScroller.distanceX = parentX
                    dragScroller.running = true
                } else if (parentX > labelSequence.width) {
                    dragScroller.distanceX = parentX - labelSequence.width
                    dragScroller.running = true
                } else {
                    doDragRubberBand(mouse.x)
                    dragScroller.running = false
                }
            }
            onReleased: function (mouse) {
                nextIndex = -1
                rubberBand.visible = false
                dragScroller.running = false
            }
        }

        Repeater {
            id: labelRepeater
            model: labelSequence.model
            property QtObject currentItem: labelSequence.currentItem
            property var itemDict: new Map()
            onItemAdded: function (index, item) {
                itemDict.set(item.labelViewModel, index)
            }
            onItemRemoved: function (index, item) {
                itemDict.delete(item.labelViewModel)
            }
            Rectangle {
                required property QtObject modelData
                readonly property QtObject labelViewModel: modelData
                readonly property bool isCurrent: labelViewModel === labelRepeater.currentItem
                property bool editing: false
                onEditingChanged: {
                    if (editing) {
                        labelEdit.focus = true
                    } else {
                        if (!labelEdit.text.length) {
                            labelSequence.removeLabel(labelRect.labelViewModel)
                            return
                        }
                        labelViewModel.content = labelEdit.text
                        labelEdit.focus = false
                    }
                }
                id: labelRect
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                x: labelViewModel ? labelViewModel.position * viewport.pixelDensity - 0.5 : 0
                z: isCurrent ? 1 : 0
                color: labelViewModel?.selected ? labelSequence.palette.labelSelectedColor: labelSequence.palette.labelColor
                Behavior on color {
                    ColorAnimation {
                        duration: (labelSequence.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                        easing.type: Easing.OutCubic
                    }
                }
                border.width: 1
                border.color: labelViewModel?.selected ? labelSequence.palette.labelSelectedBorderColor : labelSequence.palette.labelBorderColor
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
                    text: labelViewModel?.content ?? ""
                    color: labelViewModel?.selected ? labelSequence.palette.labelSelectedTextColor : labelSequence.palette.labelTextColor
                    Behavior on color {
                        ColorAnimation {
                            duration: (labelSequence.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                            easing.type: Easing.OutCubic
                        }
                    }
                }

                function selectItem (multipleSelect, extendingSelect, overrideSelect = false) {
                    let previousSelected = labelViewModel.selected
                    let previousSelectionCount = 0
                    if (!multipleSelect) {
                        previousSelectionCount = labelSequence.deselectAll()
                    }
                    if (extendingSelect) {
                        labelSequence.extendSelection(labelViewModel)
                    } else {
                        labelViewModel.selected = overrideSelect || previousSelectionCount > 1 || !previousSelected
                        labelSequence.currentItem = labelViewModel
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
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
                        if (mouse.button === Qt.LeftButton) {
                            if (rejectClick)
                                return
                            let multipleSelect = Boolean(mouse.modifiers & Qt.ControlModifier)
                            let extendingSelect = Boolean(mouse.modifiers & Qt.ShiftModifier)
                            labelRect.selectItem(multipleSelect, extendingSelect)
                        } else {
                            labelSequence.contextMenuRequestedForLabel(labelRect.labelViewModel)
                        }
                    }
                    onDoubleClicked: function (mouse) {
                        if (mouse.button !== Qt.LeftButton)
                            return
                        labelRect.selectItem(false, false, true)
                        labelRect.editing = true
                    }
                    Connections {
                        id: cursorIndicatorBinding
                        target: labelRect.labelViewModel
                        enabled: false
                        function onPositionChanged() {
                            labelSequence.timeAlignmentViewModel.cursorPosition = labelRect.labelViewModel.position
                        }
                    }
                    onPositionChanged: function (mouse) {
                        if (!rejectClick) {
                            rejectClick = true
                            labelSequence.setSelectionIntermediate(true)
                        }
                        cursorIndicatorBinding.enabled = true
                        if (!labelRect.labelViewModel.selected) {
                            let multipleSelect = Boolean(mouse.modifiers & Qt.ControlModifier)
                            let extendingSelect = Boolean(mouse.modifiers & Qt.ShiftModifier)
                            labelRect.selectItem(multipleSelect, extendingSelect)
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
                        if (rejectClick) {
                            labelSequence.setSelectionIntermediate(false)
                        }
                        dragScroller.running = false
                        cursorIndicatorBinding.enabled = false
                        labelSequence.timeAlignmentViewModel.cursorPosition = -1
                    }
                }

                onIsCurrentChanged: {
                    if (!isCurrent)
                        labelRect.editing = false
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
                    visible: labelRect.editing
                    text: labelRect.labelViewModel?.content ?? ""
                    background: Rectangle {
                        color: labelSequence.palette.labelEditingColor
                        border.width: 1
                        border.color: labelSequence.palette.labelEditingBorderColor
                    }
                    Keys.onEscapePressed: {
                        text = labelRect.labelViewModel.content
                        labelRect.editing = false
                    }
                    Keys.onReturnPressed: {
                        labelRect.editing = false
                    }
                    Keys.onBacktabPressed: {
                        let target = labelSequence.previousItem(labelRect.labelViewModel)
                        if (!labelRepeater.itemDict.has(target))
                            return
                        labelSequence.currentItem = target
                        let item = labelRepeater.itemAt(labelRepeater.itemDict.get(target))
                        item.selectItem(false, false, true)
                        item.editing = true
                    }
                    Keys.onTabPressed: {
                        let target = labelSequence.nextItem(labelRect.labelViewModel)
                        if (!labelRepeater.itemDict.has(target))
                            return
                        labelSequence.currentItem = target
                        let item = labelRepeater.itemAt(labelRepeater.itemDict.get(target))
                        item.selectItem(false, false, true)
                        item.editing = true
                    }
                }
            }
        }

        Rectangle {
            id: rubberBand
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            z: 2
            color: labelSequence.palette.rubberBandColor
            visible: false
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