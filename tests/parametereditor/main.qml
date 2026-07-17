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
    width: 1280
    height: 800
    title: qsTr("ParameterEditor interactive test")

    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property TimeViewModel timeViewModel
    required property TimeLayoutViewModel timeLayoutViewModel
    required property ClavierViewModel clavierViewModel

    required property FreeParameterViewModel mainFreeParameterViewModel
    required property AnchorParameterViewModel mainAnchorParameterViewModel
    required property FreeParameterViewModel originalParameterViewModel
    required property FreeParameterViewModel transformFreeParameterViewModel
    required property AnchorParameterViewModel transformAnchorParameterViewModel

    required property ParameterRangeSelectionViewModel mainRangeSelectionViewModel
    required property ParameterRangeSelectionViewModel originalRangeSelectionViewModel
    required property ParameterRangeSelectionViewModel transformRangeSelectionViewModel
    required property SelectionController mainAnchorSelectionController
    required property SelectionController transformAnchorSelectionController

    required property ParameterEditorInteractionController mainInteractionController
    required property ParameterEditorInteractionController originalInteractionController
    required property ParameterEditorInteractionController transformInteractionController

    component ScaledSpinBox: SpinBox {
        property real realValue: 0.0
        signal realValueModified(real newValue)

        editable: true
        from: -200
        to: 200
        stepSize: 5
        value: Math.round(realValue * 100)

        textFromValue: (value, locale) => Number(value / 100).toLocaleString(locale, "f", 2)
        valueFromText: (text, locale) => Math.round(Number.fromLocaleString(locale, text) * 100)
        onValueModified: realValueModified(value / 100)
    }

    component ToolSelector: ToolBar {
        id: toolSelector

        required property ParameterEditorInteractionController controller
        property bool anchorToolsVisible: true
        property int currentTool: ParameterEditorInteractionController.None
        property string editorName

        function selectTool(tool: int) {
            currentTool = tool
            controller.primaryItemInteraction = ParameterEditorInteractionController.None
            controller.secondaryItemInteraction = ParameterEditorInteractionController.None
            controller.primarySceneInteraction = ParameterEditorInteractionController.None
            controller.secondarySceneInteraction = ParameterEditorInteractionController.None
            controller.primarySelectInteraction = ParameterEditorInteractionController.None
            controller.secondarySelectInteraction = ParameterEditorInteractionController.None

            switch (tool) {
            case ParameterEditorInteractionController.Pencil:
            case ParameterEditorInteractionController.Eraser:
            case ParameterEditorInteractionController.FreeRangeSelect:
                controller.primaryItemInteraction = tool
                controller.primarySceneInteraction = tool
                break
            case ParameterEditorInteractionController.Pen:
                controller.primaryItemInteraction = tool
                controller.primarySceneInteraction = tool
                break
            case ParameterEditorInteractionController.Pointer:
            case ParameterEditorInteractionController.AnchorRubberBandSelect:
            case ParameterEditorInteractionController.AnchorTimeRangeSelect:
                controller.primaryItemInteraction = tool
                controller.primarySceneInteraction = tool
                controller.primarySelectInteraction = tool
                break
            case ParameterEditorInteractionController.ConvertAnchor:
                controller.primaryItemInteraction = tool
                break
            default:
                break
            }
        }

        RowLayout {
            anchors.fill: parent
            spacing: 2

            Label {
                Layout.leftMargin: 8
                Layout.rightMargin: 8
                text: toolSelector.editorName
                font.bold: true
            }

            ToolButton {
                text: qsTr("Blank")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.None
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.None)
            }
            ToolButton {
                text: qsTr("Pencil")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.Pencil
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.Pencil)
            }
            ToolButton {
                text: qsTr("Eraser")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.Eraser
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.Eraser)
            }
            ToolButton {
                visible: toolSelector.anchorToolsVisible
                text: qsTr("Pointer")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.Pointer
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.Pointer)
            }
            ToolButton {
                visible: toolSelector.anchorToolsVisible
                text: qsTr("Pen")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.Pen
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.Pen)
            }
            ToolButton {
                visible: toolSelector.anchorToolsVisible
                text: qsTr("Convert")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.ConvertAnchor
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.ConvertAnchor)
            }
            ToolButton {
                text: qsTr("Free range")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.FreeRangeSelect
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.FreeRangeSelect)
            }
            ToolButton {
                visible: toolSelector.anchorToolsVisible
                text: qsTr("Anchor band")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.AnchorRubberBandSelect
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.AnchorRubberBandSelect)
            }
            ToolButton {
                visible: toolSelector.anchorToolsVisible
                text: qsTr("Anchor time")
                checkable: true
                checked: toolSelector.currentTool === ParameterEditorInteractionController.AnchorTimeRangeSelect
                onClicked: toolSelector.selectTool(ParameterEditorInteractionController.AnchorTimeRangeSelect)
            }

            Item {
                Layout.fillWidth: true
            }
        }
    }

    component EditorPanel: ColumnLayout {
        id: editorPanel

        required property string title
        required property ParameterEditorInteractionController controller
        required property FreeParameterViewModel freeParameterViewModel
        required property ParameterRangeSelectionViewModel rangeSelectionViewModel
        property AnchorParameterViewModel anchorParameterViewModel: null
        property FreeParameterViewModel originalParameterViewModel: null
        property FreeParameterViewModel freeTransformParameterViewModel: null
        property AnchorParameterViewModel anchorTransformParameterViewModel: null
        property SelectionController anchorSelectionController: null
        property bool anchorToolsVisible: true

        spacing: 0

        ToolSelector {
            Layout.fillWidth: true
            controller: editorPanel.controller
            editorName: editorPanel.title
            anchorToolsVisible: editorPanel.anchorToolsVisible
        }

        Frame {
            Layout.fillWidth: true
            Layout.fillHeight: true
            padding: 1

            ParameterEditor {
                anchors.fill: parent
                freeParameterViewModel: editorPanel.freeParameterViewModel
                anchorParameterViewModel: editorPanel.anchorParameterViewModel
                originalParameterViewModel: editorPanel.originalParameterViewModel
                freeTransformParameterViewModel: editorPanel.freeTransformParameterViewModel
                anchorTransformParameterViewModel: editorPanel.anchorTransformParameterViewModel
                freeParameterSelectionViewModel: editorPanel.rangeSelectionViewModel
                anchorSelectionController: editorPanel.anchorSelectionController
                timeViewModel: main.timeViewModel
                timeLayoutViewModel: main.timeLayoutViewModel
                interactionController: editorPanel.controller
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            id: editorArea

            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumWidth: 720

            ColumnLayout {
                anchors.fill: parent
                spacing: 2

                EditorPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    title: qsTr("Main")
                    controller: main.mainInteractionController
                    freeParameterViewModel: main.mainFreeParameterViewModel
                    anchorParameterViewModel: main.mainAnchorParameterViewModel
                    originalParameterViewModel: main.originalParameterViewModel
                    freeTransformParameterViewModel: main.transformFreeParameterViewModel
                    anchorTransformParameterViewModel: main.transformAnchorParameterViewModel
                    rangeSelectionViewModel: main.mainRangeSelectionViewModel
                    anchorSelectionController: main.mainAnchorSelectionController
                }

                EditorPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    title: qsTr("Original free parameter")
                    controller: main.originalInteractionController
                    freeParameterViewModel: main.originalParameterViewModel
                    rangeSelectionViewModel: main.originalRangeSelectionViewModel
                    anchorToolsVisible: false
                }

                EditorPanel {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    title: qsTr("Free and anchor transform")
                    controller: main.transformInteractionController
                    freeParameterViewModel: main.transformFreeParameterViewModel
                    anchorParameterViewModel: main.transformAnchorParameterViewModel
                    rangeSelectionViewModel: main.transformRangeSelectionViewModel
                    anchorSelectionController: main.transformAnchorSelectionController
                }
            }

            PianoRollScrollLayer {
                anchors.fill: parent
                z: 100
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                clavierViewModel: main.clavierViewModel
                timeViewModel: main.timeViewModel
                timeLayoutViewModel: main.timeLayoutViewModel
                movableOrientation: Qt.Horizontal
                zoomableOrientation: Qt.Horizontal
                wheelAlternateAxis: true
            }
        }

        ScrollView {
            id: settingsScrollView

            Layout.preferredWidth: 330
            Layout.fillHeight: true
            contentWidth: availableWidth
            clip: true

            ColumnLayout {
                width: settingsScrollView.availableWidth
                spacing: 12

                Label {
                    Layout.leftMargin: 12
                    Layout.rightMargin: 12
                    Layout.topMargin: 12
                    text: qsTr("Main editor controller")
                    font.bold: true
                    font.pixelSize: 16
                }

                GridLayout {
                    Layout.fillWidth: true
                    Layout.leftMargin: 12
                    Layout.rightMargin: 12
                    columns: 2
                    columnSpacing: 8
                    rowSpacing: 8

                    Label { text: qsTr("Click selectable") }
                    Switch {
                        checked: main.mainInteractionController.clickSelectable
                        onToggled: main.mainInteractionController.clickSelectable = checked
                    }

                    Label { text: qsTr("New anchor interpolation") }
                    ComboBox {
                        Layout.fillWidth: true
                        model: [qsTr("Hermite"), qsTr("Linear"), qsTr("None")]
                        currentIndex: main.mainInteractionController.newAnchorInterpolationMode
                        onActivated: main.mainInteractionController.newAnchorInterpolationMode = currentIndex
                    }

                    Label { text: qsTr("Fill mode") }
                    ComboBox {
                        Layout.fillWidth: true
                        model: [qsTr("None"), qsTr("Top"), qsTr("Bottom"), qsTr("Baseline")]
                        currentIndex: main.mainInteractionController.fillMode
                        onActivated: main.mainInteractionController.fillMode = currentIndex
                    }

                    Label { text: qsTr("Fill baseline") }
                    ScaledSpinBox {
                        Layout.fillWidth: true
                        realValue: main.mainInteractionController.fillBaseline
                        onRealValueModified: newValue => main.mainInteractionController.fillBaseline = newValue
                    }

                    Label { text: qsTr("Show reference") }
                    Switch {
                        checked: main.mainInteractionController.referenceVisible
                        onToggled: main.mainInteractionController.referenceVisible = checked
                    }

                    Label { text: qsTr("Reference baseline") }
                    ScaledSpinBox {
                        Layout.fillWidth: true
                        realValue: main.mainInteractionController.referenceBaseline
                        onRealValueModified: newValue => main.mainInteractionController.referenceBaseline = newValue
                    }

                    Label { text: qsTr("Enable default") }
                    Switch {
                        checked: main.mainInteractionController.defaultValueEnabled
                        onToggled: main.mainInteractionController.defaultValueEnabled = checked
                    }

                    Label { text: qsTr("Default value") }
                    ScaledSpinBox {
                        Layout.fillWidth: true
                        realValue: main.mainInteractionController.defaultValue
                        onRealValueModified: newValue => main.mainInteractionController.defaultValue = newValue
                    }

                    Label { text: qsTr("Fallback curve") }
                    ComboBox {
                        Layout.fillWidth: true
                        model: [qsTr("Hidden"), qsTr("Dashed"), qsTr("Solid")]
                        currentIndex: main.mainInteractionController.originalAndDefaultCurveDisplayMode
                        onActivated: main.mainInteractionController.originalAndDefaultCurveDisplayMode = currentIndex
                    }
                }

                GroupBox {
                    Layout.fillWidth: true
                    Layout.leftMargin: 12
                    Layout.rightMargin: 12
                    title: qsTr("Shared horizontal viewport")

                    GridLayout {
                        anchors.fill: parent
                        columns: 2

                        Label { text: qsTr("Start") }
                        Label { text: Number(main.timeViewModel.start).toFixed(1) }
                        Label { text: qsTr("End") }
                        Label { text: Number(main.timeViewModel.end).toFixed(1) }
                        Label { text: qsTr("Pixel density") }
                        Label { text: Number(main.timeLayoutViewModel.pixelDensity).toFixed(3) }
                    }
                }

                Label {
                    Layout.fillWidth: true
                    Layout.leftMargin: 12
                    Layout.rightMargin: 12
                    Layout.bottomMargin: 12
                    wrapMode: Text.WordWrap
                    text: qsTr("Use the mouse wheel over any editor to scroll all three editors. Hold Ctrl while scrolling to zoom horizontally; the middle mouse button also uses the shared scroll layer.")
                }

            }
        }
    }
}
