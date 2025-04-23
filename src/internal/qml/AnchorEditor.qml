import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Style

Item {
    id: anchorEditor
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject anchoredCurveViewModel: null
    property int topValue: 0
    property int bottomValue: 0
    property color nodeColor
    property color nodeBorderColor
    property color nodeSelectedColor

    clip: true

    component AnchorNode: Item {
        id: anchorNode
        property int anchorNode: 0
        property bool selected: false
        LayoutMirroring.enabled: false
        LayoutMirroring.childrenInherit: true
        property color color: selected ? anchorEditor.nodeSelectedColor : anchorEditor.nodeColor
        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }
        Item {
            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            clip: true
            Item {
                id: leftCenter
                anchors.left: parent.right
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
            }
            Rectangle {
                anchors.centerIn: leftCenter
                width: anchorNode.width / ((anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftDiamond ? Math.SQRT2 : 1)
                height: anchorNode.height / ((anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftDiamond ? Math.SQRT2 : 1)
                color: anchorNode.color
                border.color: anchorEditor.nodeBorderColor
                radius: (anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftCircle ? width / 2 : 0
                rotation: (anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftDiamond ? 45 : 0
            }
        }
        Item {
            anchors.left: parent.horizontalCenter
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            clip: true
            Item {
                id: rightCenter
                anchors.left: parent.left
                anchors.right: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
            }
            Rectangle {
                anchors.centerIn: rightCenter
                width: anchorNode.width / ((anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightDiamond ? Math.SQRT2 : 1)
                height: anchorNode.height / ((anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightDiamond ? Math.SQRT2 : 1)
                color: anchorNode.color
                border.color: anchorEditor.nodeBorderColor
                radius: (anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightCircle ? width / 2 : 0
                rotation: (anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightDiamond ? 45 : 0
            }
        }
    }

    TimeAlignmentPositionLocator {
        id: timeLocator
        anchors.fill: parent
        timeViewModel: anchorEditor.timeViewModel
        timeLayoutViewModel: anchorEditor.timeLayoutViewModel
    }

    TimeManipulator {
        id: timeManipulator
        anchors.fill: parent
        timeViewModel: anchorEditor.timeViewModel
        timeLayoutViewModel: anchorEditor.timeLayoutViewModel
    }

    SelectableViewModelManipulator {
        id: selectionManipulator
        viewModel: anchorEditor.anchoredCurveViewModel
    }

    Item {
        id: viewport
        readonly property double start: anchorEditor.timeViewModel?.start ?? 0
        readonly property double end: anchorEditor.timeViewModel?.end ?? 0
        readonly property double pixelDensity: anchorEditor.timeLayoutViewModel?.pixelDensity ?? 0
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        x: -start * pixelDensity
        width: end * pixelDensity

        GenericBackPointerMouseArea {
            id: backPointerMouseArea
            paneItem: anchorEditor
            onRubberBandStartRequired: (p) => {
                rubberBandLayer.startSelection(p)
            }
            onRubberBandUpdateRequired: (p) => {
                rubberBandLayer.updateSelection(p)
            }
            onDoubleClicked: (mouse) => {

            }
        }

        Item {
            id: nodeContainer
            anchors.fill: parent
            SequenceSlicer {
                model: anchorEditor.anchoredCurveViewModel
                timeViewModel: anchorEditor.timeViewModel
                timeLayoutViewModel: anchorEditor.timeLayoutViewModel
                sliceWidth: anchorEditor.width
                leftOutBound: 16
                rightOutBound: 16
                delegate: Item {
                    id: nodeItem
                    width: 0
                    height: 0
                    required property QtObject model
                    Binding {
                        when: nodeItem.visible
                        nodeItem.x: nodeItem.model.position * viewport.pixelDensity
                        nodeItem.y: (anchorEditor.topValue - nodeItem.model.anchorValue) / (anchorEditor.topValue - anchorEditor.bottomValue) * anchorEditor.height
                    }
                    readonly property bool visibleToRubberBand: visible && !model.free
                    function handleRubberBand() {
                        if (visibleToRubberBand)
                            rubberBandLayer.insertItem(model, Qt.rect(x - 4, y - 4, 8, 8))
                        else
                            rubberBandLayer.removeItem(model)
                    }
                    onVisibleToRubberBandChanged: handleRubberBand()
                    onXChanged: handleRubberBand()
                    onYChanged: handleRubberBand()
                    Component.onDestruction: rubberBandLayer.removeItem(model)
                    AnchorNode {
                        visible: !nodeItem.model.free
                        anchorNode: nodeItem.model.anchorNode
                        selected: nodeItem.model.selected
                        anchors.centerIn: parent
                        width: 8
                        height: 8
                        AnchorEditorPointerMouseArea {
                            id: pointerMouseArea
                            paneItem: anchorEditor
                            sequenceViewModel: anchorEditor.anchoredCurveViewModel
                            model: nodeItem.model
                            topValue: anchorEditor.topValue
                            bottomValue: anchorEditor.bottomValue

                            onDraggingChanged: {
                                if (dragging) {
                                    cursorIndicatorBinding.enabled = true
                                    cursorIndicatorBinding.onPositionChanged()
                                } else {
                                    cursorIndicatorBinding.enabled = false
                                    anchorEditor.timeLayoutViewModel.cursorPosition = -1
                                }
                            }

                            onDoubleClicked: (mouse) => {
                                anchorEditor.anchoredCurveViewModel.handle.currentItem = nodeItem.model
                            }
                        }
                    }
                    Connections {
                        id: cursorIndicatorBinding
                        target: nodeItem.model
                        enabled: false
                        function onPositionChanged() {
                            anchorEditor.timeLayoutViewModel.cursorPosition = nodeItem.model.position
                        }
                    }
                }
            }
        }

        RubberBandLayer {
            id: rubberBandLayer
            anchors.fill: parent
            selectionManipulator: selectionManipulator
            z: Infinity
            rubberBand: RubberBandRectangle {
            }
        }
    }

}