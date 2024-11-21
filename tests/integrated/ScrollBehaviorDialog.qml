import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Imagine

Item {
    id: dialog
    required property QtObject scrollBehaviorViewModel

    property var comboBoxModel: [
        { value: Qt.AltModifier, text: "Alt" },
        { value: Qt.ShiftModifier, text: "Shift" },
        { value: Qt.ControlModifier, text: "Ctrl" }
    ]

    GridLayout {
        anchors.fill: parent
        anchors.margins: 8
        Text {
            Layout.row: 0
            Layout.column: 0
            text: "Alternate axis"
        }
        ComboBox {
            Layout.row: 0
            Layout.column: 1
            Layout.fillWidth: true
            textRole: "text"
            valueRole: "value"
            model: dialog.comboBoxModel
            Component.onCompleted: currentIndex = indexOfValue(dialog.scrollBehaviorViewModel.alternateAxisModifier)
            onActivated: dialog.scrollBehaviorViewModel.alternateAxisModifier = currentValue
        }
        Text {
            Layout.row: 1
            Layout.column: 0
            text: "Zoom"
        }
        ComboBox {
            Layout.row: 1
            Layout.column: 1
            Layout.fillWidth: true
            textRole: "text"
            valueRole: "value"
            model: dialog.comboBoxModel
            Component.onCompleted: currentIndex = indexOfValue(dialog.scrollBehaviorViewModel.zoomModifier)
            onActivated: dialog.scrollBehaviorViewModel.zoomModifier = currentValue
        }
        Text {
            Layout.row: 2
            Layout.column: 0
            text: "Page"
        }
        ComboBox {
            Layout.row: 2
            Layout.column: 1
            Layout.fillWidth: true
            textRole: "text"
            valueRole: "value"
            model: dialog.comboBoxModel
            Component.onCompleted: currentIndex = indexOfValue(dialog.scrollBehaviorViewModel.pageModifier)
            onActivated: dialog.scrollBehaviorViewModel.pageModifier = currentValue
        }
        CheckBox {
            Layout.row: 3
            Layout.column: 0
            Layout.columnSpan: 1
            text: "Use page modifier as alternate axis zoom"
            checked: dialog.scrollBehaviorViewModel.usePageModifierAsAlternateAxisZoom
            onCheckedChanged: dialog.scrollBehaviorViewModel.usePageModifierAsAlternateAxisZoom = checked
        }
        CheckBox {
            Layout.row: 4
            Layout.column: 0
            Layout.columnSpan: 1
            text: "Middle button drag"
            checked: !dialog.scrollBehaviorViewModel.affectVelocity
            onCheckedChanged: dialog.scrollBehaviorViewModel.affectVelocity = !checked
        }
    }
}