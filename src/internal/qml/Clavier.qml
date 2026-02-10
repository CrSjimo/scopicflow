pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Templates as T

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: clavier

    property ClavierViewModel clavierViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property ClavierInteractionController clavierInteractionController: null

    property double bottomExpansion: 0

    Accessible.name: qsTr("Piano keyboard")
    clip: true
    focus: true
    focusPolicy: Qt.StrongFocus
    implicitWidth: 96

    QtObject {
        id: helper
        readonly property double keyHeight: clavier.clavierViewModel?.pixelDensity ?? 24
        readonly property bool isRealistic: clavier.clavierInteractionController?.displayStyle === ClavierInteractionController.Realistic
        readonly property list<double> realisticTopMarginFactorList: [-2/3, 0, -1/3, 0, 0, -4/7, 0, -3/7, 0, -2/7, 0, 0]
        readonly property list<double> realisticBottomMarginFactorList: [0, 0, -1/3, 0, -2/3, 0, 0, -3/7, 0, -4/7, 0, -5/7]
        readonly property double realisticRightMarginFactor: 0.25
        readonly property int cursorPosition: clavier.clavierViewModel?.cursorPosition ?? -1
        property Item currentCursorItem: null
        onCursorPositionChanged: () => {
            if (currentCursorItem) {
                currentCursorItem.cursorActive = false
            }
            if (cursorPosition >= 0 && cursorPosition < 128) {
                currentCursorItem = repeater.itemAt(cursorPosition)
                currentCursorItem.cursorActive = true
            } else {
                currentCursorItem = null
            }
        }
        function isBlackKey(key) {
            let indexInGroup = key % 12;
            return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10;
        }

        function realisticTopMarginFactor(key) {
            return helper.realisticTopMarginFactorList[key % 12];
        }

        function realisticBottomMarginFactor(key) {
            return helper.realisticBottomMarginFactorList[key % 12];
        }

        function getIndexFromPoint(p) {
            let position = clavierManipulator.mapToPosition(p.y)
            let index = Math.floor(position)
            if (index < 0 || index >= 128) {
                return -1
            }
            if (!isRealistic || p.x < clavier.width * (1 - realisticRightMarginFactor) || !isBlackKey(index)) {
                return Math.floor(index)
            }
            if (index - position >= realisticTopMarginFactor(index - 1)) {
                return index - 1
            }
            return index + 1
        }
    }

    ClavierManipulator {
        id: clavierManipulator
        clavierViewModel: clavier.clavierViewModel
        target: clavier
        viewSize: clavier.height - clavier.bottomExpansion
    }

    ClavierViewportContainer {
        id: viewportContainer
        clavierViewModel: clavier.clavierViewModel
        Item {
            id: keyContainer
            anchors.fill: parent
            Repeater {
                id: repeater
                model: 128
                delegate: Item {
                    id: keyItem
                    required property int index
                    x: 0
                    y: (127 - index) * helper.keyHeight
                    width: parent.width
                    height: helper.keyHeight
                    z: helper.isBlackKey(index) ? 1 : 0
                    property bool pressed: false
                    property bool hovered: false
                    property bool cursorActive: false
                    Rectangle {
                        id: keyRect
                        anchors.fill: parent
                        anchors.topMargin: helper.isRealistic ? helper.realisticTopMarginFactor(keyItem.index) * helper.keyHeight : 0
                        anchors.bottomMargin: helper.isRealistic ? helper.realisticBottomMarginFactor(keyItem.index) * helper.keyHeight : 0
                        anchors.rightMargin: helper.isRealistic && helper.isBlackKey(keyItem.index) ? helper.realisticRightMarginFactor * clavier.width : 0
                        readonly property color inactiveColor: helper.isBlackKey(keyItem.index) ? SFPalette.blackKeyColor : SFPalette.whiteKeyColor
                        color: {
                            if (keyItem.pressed) {
                                return Theme.controlPressedColorChange.apply(Theme.accentColor)
                            }
                            if (keyItem.hovered) {
                                return Theme.controlHoveredColorChange.apply(Theme.accentColor)
                            }
                            return keyRect.inactiveColor
                        }
                        Behavior on color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                        border.color: Theme.borderColor
                        topRightRadius: helper.isRealistic && helper.isBlackKey(keyItem.index) ? 4 : 0
                        bottomRightRadius: helper.isRealistic && helper.isBlackKey(keyItem.index) ? 4 : 0
                    }
                    component KeyLabel: Text {
                        readonly property color inactiveColor: helper.isBlackKey(keyItem.index) ? SFPalette.blackKeyTextColor : SFPalette.whiteKeyTextColor
                        color: {
                            if (keyItem.pressed) {
                                return Theme.foregroundPressedColorChange.apply(Theme.foregroundPrimaryColor)
                            }
                            if (keyItem.hovered) {
                                return Theme.foregroundHoveredColorChange.apply(Theme.foregroundPrimaryColor)
                            }
                            return rightLabel.inactiveColor
                        }
                        Behavior on color {
                            ColorAnimation {
                                duration: Theme.colorAnimationDuration
                                easing.type: Easing.OutCubic
                            }
                        }
                        font: Theme.font
                        anchors.verticalCenter: parent.verticalCenter
                        text: SVS.musicPitch(keyItem.index).toString(clavier.clavierInteractionController?.accidentalType ?? 0)
                    }
                    KeyLabel {
                        id: leftLabel
                        anchors.left: keyRect.left
                        anchors.leftMargin: 4
                        visible: keyItem.cursorActive
                    }
                    KeyLabel {
                        id: rightLabel
                        anchors.right: keyRect.right
                        anchors.rightMargin: 4
                        visible: clavier.clavierInteractionController?.labelStrategy === ClavierInteractionController.LabelAll || clavier.clavierInteractionController?.labelStrategy === ClavierInteractionController.LabelC && keyItem.index % 12 === 0
                    }
                }
            }
        }
        MouseArea {
            id: mouseArea
            anchors.fill: parent
            property int hoveredIndex: -1
            property int pressedIndex: -1
            hoverEnabled: true

            function updateHoveredIndex(newIndex) {
                if (hoveredIndex === newIndex) {
                    return
                }
                
                // Exit previous hovered key
                if (hoveredIndex >= 0 && hoveredIndex < 128) {
                    let prevItem = repeater.itemAt(hoveredIndex)
                    prevItem.hovered = false
                    clavier.clavierInteractionController?.hoverExited(clavier, hoveredIndex)
                }
                
                hoveredIndex = newIndex
                
                // Enter new hovered key
                if (hoveredIndex >= 0 && hoveredIndex < 128) {
                    let newItem = repeater.itemAt(hoveredIndex)
                    newItem.hovered = true
                    clavier.clavierInteractionController?.hoverEntered(clavier, hoveredIndex)
                }
            }

            function updatePressedIndex(newIndex) {
                if (pressedIndex === newIndex) {
                    return
                }
                
                // Release previous pressed key
                if (pressedIndex >= 0 && pressedIndex < 128) {
                    let prevItem = repeater.itemAt(pressedIndex)
                    prevItem.pressed = false
                    clavier.clavierInteractionController?.released(clavier, pressedIndex)
                }
                
                pressedIndex = newIndex
                
                // Press new key
                if (pressedIndex >= 0 && pressedIndex < 128) {
                    let newItem = repeater.itemAt(pressedIndex)
                    newItem.pressed = true
                    clavier.clavierInteractionController?.pressed(clavier, pressedIndex)
                }
            }

            onPressed: (mouse) => {
                let p = mapToItem(clavier, mouse.x, mouse.y)
                let index = helper.getIndexFromPoint(p)
                updatePressedIndex(index)
            }

            onReleased: (mouse) => {
                updatePressedIndex(-1)
            }

            onCanceled: () => {
                updatePressedIndex(-1)
            }

            onEntered: () => {
                // Mouse entered the MouseArea, update hovered index
                let p = mapToItem(clavier, mouseX, mouseY)
                let index = helper.getIndexFromPoint(p)
                updateHoveredIndex(index)
            }

            onExited: () => {
                updateHoveredIndex(-1)
            }

            onPositionChanged: (mouse) => {
                let p = mapToItem(clavier, mouse.x, mouse.y)
                let index = helper.getIndexFromPoint(p)
                
                updateHoveredIndex(index)
                
                // Update pressed index if mouse is pressed
                if (pressed) {
                    updatePressedIndex(index)
                }
            }

            onDoubleClicked: (mouse) => {
                let index = helper.getIndexFromPoint(mapToItem(clavier, mouse.x, mouse.y))
                if (index >= 0 && index < 128) {
                    clavier.clavierInteractionController?.doubleClicked(clavier, index)
                }
            }
        }
    }

    StandardScrollHandler {
        movableOrientation: Qt.Vertical
        viewModel: clavier.scrollBehaviorViewModel

        onMoved: (_, deltaY, isPhysicalWheel) => {
            clavierManipulator.moveViewBy(deltaY, isPhysicalWheel);
        }
    }
}
