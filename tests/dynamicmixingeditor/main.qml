pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Views

ApplicationWindow {
    id: main

    visible: true
    width: 1200
    height: 640
    title: qsTr("DynamicMixingEditor interactive test")

    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property TimeViewModel timeViewModel
    required property TimeLayoutViewModel timeLayoutViewModel
    required property ClavierViewModel clavierViewModel
    required property DynamicMixingViewModel dynamicMixingViewModel
    required property SelectionController selectionController
    required property DynamicMixingEditorInteractionController interactionController

    property int currentTool: DynamicMixingEditorInteractionController.Pointer
    property string operationStatus: qsTr("Ready")

    function selectTool(tool: int) {
        currentTool = tool
        interactionController.primaryItemInteraction = DynamicMixingEditorInteractionController.None
        interactionController.secondaryItemInteraction = DynamicMixingEditorInteractionController.None
        interactionController.primarySceneInteraction = DynamicMixingEditorInteractionController.None
        interactionController.secondarySceneInteraction = DynamicMixingEditorInteractionController.None
        interactionController.primarySelectInteraction = DynamicMixingEditorInteractionController.None
        interactionController.secondarySelectInteraction = DynamicMixingEditorInteractionController.None

        switch (tool) {
        case DynamicMixingEditorInteractionController.Pointer:
            interactionController.primaryItemInteraction = tool
            interactionController.primarySceneInteraction = tool
            interactionController.primarySelectInteraction = tool
            break
        case DynamicMixingEditorInteractionController.AddAnchor:
        case DynamicMixingEditorInteractionController.DeleteAnchor:
            interactionController.primaryItemInteraction = tool
            interactionController.primarySceneInteraction = tool
            break
        case DynamicMixingEditorInteractionController.RubberBandSelect:
            interactionController.primaryItemInteraction = tool
            interactionController.primarySceneInteraction = tool
            interactionController.primarySelectInteraction = tool
            break
        default:
            break
        }
    }

    Component.onCompleted: selectTool(currentTool)

    Connections {
        target: main.interactionController

        function onAnchorInsertionCommitted(_, item) {
            main.operationStatus = qsTr("Added anchor at %1").arg(item.position)
        }
        function onAnchorInsertionAborted() {
            main.operationStatus = qsTr("Could not add an anchor at that position")
        }
        function onAnchorMovingCommitted(_, item) {
            main.operationStatus = qsTr("Moved anchor to %1").arg(item.position)
        }
        function onAnchorMovingAborted() {
            main.operationStatus = qsTr("Anchor move canceled")
        }
        function onAnchorDeletionCommitted() {
            main.operationStatus = qsTr("Deleted selected anchor(s)")
        }
        function onAnchorDeletionAborted() {
            main.operationStatus = qsTr("Could not delete every selected anchor")
        }
        function onRubberBandDraggingCommitted() {
            main.operationStatus = qsTr("Updated the anchor selection")
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        ToolBar {
            Layout.fillWidth: true

            RowLayout {
                anchors.fill: parent
                spacing: 2

                Label {
                    Layout.leftMargin: 8
                    Layout.rightMargin: 8
                    text: qsTr("Tool")
                    font.bold: true
                }

                ToolButton {
                    text: qsTr("Pointer")
                    checkable: true
                    checked: main.currentTool === DynamicMixingEditorInteractionController.Pointer
                    onClicked: main.selectTool(DynamicMixingEditorInteractionController.Pointer)
                }
                ToolButton {
                    text: qsTr("Add")
                    checkable: true
                    checked: main.currentTool === DynamicMixingEditorInteractionController.AddAnchor
                    onClicked: main.selectTool(DynamicMixingEditorInteractionController.AddAnchor)
                }
                ToolButton {
                    text: qsTr("Delete")
                    checkable: true
                    checked: main.currentTool === DynamicMixingEditorInteractionController.DeleteAnchor
                    onClicked: main.selectTool(DynamicMixingEditorInteractionController.DeleteAnchor)
                }
                ToolButton {
                    text: qsTr("Rubber band")
                    checkable: true
                    checked: main.currentTool === DynamicMixingEditorInteractionController.RubberBandSelect
                    onClicked: main.selectTool(DynamicMixingEditorInteractionController.RubberBandSelect)
                }
                ToolButton {
                    text: qsTr("None")
                    checkable: true
                    checked: main.currentTool === DynamicMixingEditorInteractionController.None
                    onClicked: main.selectTool(DynamicMixingEditorInteractionController.None)
                }

                ToolSeparator {}

                Label { text: qsTr("Voices") }
                SpinBox {
                    from: 1
                    to: 12
                    editable: true
                    value: main.dynamicMixingViewModel.voiceCount
                    onValueModified: main.dynamicMixingViewModel.voiceCount = value
                }

                Item { Layout.fillWidth: true }

                Label {
                    Layout.rightMargin: 8
                    text: qsTr("%1 anchors").arg(main.dynamicMixingViewModel.count)
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Frame {
                Layout.fillWidth: true
                Layout.fillHeight: true
                padding: 1

                Item {
                    anchors.fill: parent

                    DynamicMixingEditor {
                        id: editor

                        anchors.fill: parent
                        dynamicMixingViewModel: main.dynamicMixingViewModel
                        timeViewModel: main.timeViewModel
                        timeLayoutViewModel: main.timeLayoutViewModel
                        interactionController: main.interactionController
                        selectionController: main.selectionController
                        colors: [
                            Theme.accentColor,
                            SFPalette.routeColor,
                            SFPalette.loopColor,
                        ]
                    }

                    PianoRollScrollLayer {
                        anchors.fill: parent
                        z: 100
                        clavierViewModel: main.clavierViewModel
                        scrollBehaviorViewModel: main.scrollBehaviorViewModel
                        timeViewModel: main.timeViewModel
                        timeLayoutViewModel: main.timeLayoutViewModel
                        movableOrientation: Qt.Horizontal
                        zoomableOrientation: Qt.Horizontal
                        wheelAlternateAxis: true
                    }
                }
            }

            Pane {
                Layout.preferredWidth: 300
                Layout.fillHeight: true

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 12

                    Label {
                        text: qsTr("Selection")
                        font.bold: true
                        font.pixelSize: 16
                    }

                    GridLayout {
                        Layout.fillWidth: true
                        columns: 2

                        Label { text: qsTr("Position") }
                        Label {
                            Layout.fillWidth: true
                            text: main.selectionController.currentItem?.position ?? qsTr("None")
                        }

                        Label { text: qsTr("Ratio") }
                        Label {
                            Layout.fillWidth: true
                            wrapMode: Text.WrapAnywhere
                            text: main.selectionController.currentItem
                                  ? main.selectionController.currentItem.ratio
                                      .map(value => Number(value).toFixed(3)).join(", ")
                                  : qsTr("None")
                        }

                        Label { text: qsTr("Viewport start") }
                        Label { text: Number(main.timeViewModel.start).toFixed(1) }

                        Label { text: qsTr("Pixel density") }
                        Label { text: Number(main.timeLayoutViewModel.pixelDensity).toFixed(3) }
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("Pointer: drag a vertical line horizontally, or drag a circular handle horizontally/vertically. The first and last sample anchors intentionally demonstrate missing and extra ratio entries.")
                    }

                    Label {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("Scroll with the mouse wheel or the horizontal scrollbar. Hold Ctrl while scrolling to zoom. Middle-button dragging is also enabled by the overlaid scroll layer.")
                    }

                    Item { Layout.fillHeight: true }
                }
            }
        }

        Label {
            Layout.fillWidth: true
            leftPadding: 8
            rightPadding: 8
            topPadding: 4
            bottomPadding: 4
            text: main.operationStatus
        }
    }
}
