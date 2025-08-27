import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow

Item {
    id: anchorEditor

    property QtObject anchoredCurveViewModel: null
    readonly property int bottomValue: parameterRangeViewModel?.bottomValue ?? 0
    property color nodeBorderColor
    property color nodeColor
    property color nodeSelectedColor
    property QtObject parameterRangeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null
    readonly property int topValue: parameterRangeViewModel?.topValue ?? 0

    clip: true

    TimeAlignmentPositionLocator {
        id: timeLocator

        anchors.fill: parent
        timeLayoutViewModel: anchorEditor.timeLayoutViewModel
        timeViewModel: anchorEditor.timeViewModel
    }
    TimeManipulator {
        id: timeManipulator

        anchors.fill: parent
        timeLayoutViewModel: anchorEditor.timeLayoutViewModel
        timeViewModel: anchorEditor.timeViewModel
    }
    SelectableViewModelManipulator {
        id: selectionManipulator

        viewModel: anchorEditor.anchoredCurveViewModel
    }
    Item {
        id: viewport

        readonly property double end: anchorEditor.timeViewModel?.end ?? 0
        readonly property double pixelDensity: anchorEditor.timeLayoutViewModel?.pixelDensity ?? 0
        readonly property double start: anchorEditor.timeViewModel?.start ?? 0

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        width: end * pixelDensity
        x: -start * pixelDensity

        GenericBackPointerMouseArea {
            id: backPointerMouseArea

            paneItem: anchorEditor

            onDoubleClicked: mouse => {}
            onRubberBandStartRequired: p => {
                rubberBandLayer.startSelection(p);
            }
            onRubberBandUpdateRequired: p => {
                rubberBandLayer.updateSelection(p);
            }
        }
        Item {
            id: nodeContainer

            anchors.fill: parent

            SequenceSlicer {
                leftOutBound: 16
                model: anchorEditor.anchoredCurveViewModel
                rightOutBound: 16
                sliceWidth: anchorEditor.width
                timeLayoutViewModel: anchorEditor.timeLayoutViewModel
                timeViewModel: anchorEditor.timeViewModel

                delegate: Item {
                    id: nodeItem

                    required property QtObject model
                    readonly property bool visibleToRubberBand: visible && !model.free

                    function handleRubberBand() {
                        if (visibleToRubberBand)
                            rubberBandLayer.insertItem(model, Qt.rect(x - 4, y - 4, 8, 8));
                        else
                            rubberBandLayer.removeItem(model);
                    }

                    height: 0
                    width: 0

                    Component.onDestruction: rubberBandLayer.removeItem(model)
                    onVisibleToRubberBandChanged: handleRubberBand()
                    onXChanged: handleRubberBand()
                    onYChanged: handleRubberBand()

                    Binding {
                        nodeItem.x: nodeItem.model.position * viewport.pixelDensity
                        nodeItem.y: (anchorEditor.topValue - nodeItem.model.anchorValue) / (anchorEditor.topValue - anchorEditor.bottomValue) * anchorEditor.height
                        when: nodeItem.visible
                    }
                    AnchorNode {
                        anchorNode: nodeItem.model.anchorNode
                        anchors.centerIn: parent
                        height: 8
                        selected: nodeItem.model.selected
                        visible: !nodeItem.model.free
                        width: 8

                        AnchorEditorPointerMouseArea {
                            id: pointerMouseArea

                            bottomValue: anchorEditor.bottomValue
                            model: nodeItem.model
                            paneItem: anchorEditor
                            sequenceViewModel: anchorEditor.anchoredCurveViewModel
                            topValue: anchorEditor.topValue

                            onDoubleClicked: mouse => {
                                anchorEditor.anchoredCurveViewModel.handle.currentItem = nodeItem.model;
                            }
                            onDraggingChanged: {
                                if (dragging) {
                                    cursorIndicatorBinding.enabled = true;
                                    cursorIndicatorBinding.onPositionChanged();
                                } else {
                                    cursorIndicatorBinding.enabled = false;
                                    anchorEditor.timeLayoutViewModel.cursorPosition = -1;
                                }
                            }
                        }
                    }
                    Connections {
                        id: cursorIndicatorBinding

                        function onPositionChanged() {
                            anchorEditor.timeLayoutViewModel.cursorPosition = nodeItem.model.position;
                        }

                        enabled: false
                        target: nodeItem.model
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

    component AnchorNode: Item {
        id: anchorNode

        property int anchorNode: 0
        property color color: selected ? anchorEditor.nodeSelectedColor : anchorEditor.nodeColor
        property bool selected: false

        LayoutMirroring.childrenInherit: true
        LayoutMirroring.enabled: false

        Behavior on color {
            ColorAnimation {
                duration: Theme.colorAnimationDuration
                easing.type: Easing.OutCubic
            }
        }

        Item {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.top: parent.top
            clip: true

            Item {
                id: leftCenter

                anchors.bottom: parent.bottom
                anchors.left: parent.right
                anchors.right: parent.right
                anchors.top: parent.top
            }
            Rectangle {
                anchors.centerIn: leftCenter
                border.color: anchorEditor.nodeBorderColor
                color: anchorNode.color
                height: anchorNode.height / ((anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftDiamond ? Math.SQRT2 : 1)
                radius: (anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftCircle ? width / 2 : 0
                rotation: (anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftDiamond ? 45 : 0
                width: anchorNode.width / ((anchorNode.anchorNode & ScopicFlow.AN_LeftMask) === ScopicFlow.AN_LeftDiamond ? Math.SQRT2 : 1)
            }
        }
        Item {
            anchors.bottom: parent.bottom
            anchors.left: parent.horizontalCenter
            anchors.right: parent.right
            anchors.top: parent.top
            clip: true

            Item {
                id: rightCenter

                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.right: parent.left
                anchors.top: parent.top
            }
            Rectangle {
                anchors.centerIn: rightCenter
                border.color: anchorEditor.nodeBorderColor
                color: anchorNode.color
                height: anchorNode.height / ((anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightDiamond ? Math.SQRT2 : 1)
                radius: (anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightCircle ? width / 2 : 0
                rotation: (anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightDiamond ? 45 : 0
                width: anchorNode.width / ((anchorNode.anchorNode & ScopicFlow.AN_RightMask) === ScopicFlow.AN_RightDiamond ? Math.SQRT2 : 1)
            }
        }
    }
}
