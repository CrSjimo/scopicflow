import ScopicFlowPrivate

import QtQml
import QtQuick

import './HelperComponents'

Clavier {
    id: clavier

    readonly property list<double> keyYFactor: [5 / 3, 2, 10 / 3, 4, 15 / 3, 27 / 4, 7, 34 / 4, 9, 41 / 4, 11, 12]
    readonly property list<double> keyHeightFactor: [5 / 3, 1, 5 / 3, 1, 5 / 3, 7 / 4, 1, 7 / 4, 1, 7 / 4, 1, 7 / 4]

    function calculateYFromKey(key) {
        let indexInGroup = key % 12;
        let group = (key - indexInGroup) / 12;

        return (128 - group * 12 - keyYFactor[indexInGroup]) * clavier.keyHeight;
    }

    function isBlackKey(key) {
        let indexInGroup = key % 12;
        return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10
    }

    property int lastNoteIndex: -1

    onCursorNoteIndexChanged: function (index) {
        if (lastNoteIndex >= 0)
            keyRepeater.itemAt(lastNoteIndex).isLabelVisible = false
        if (index >= 0)
            keyRepeater.itemAt(index).isLeftLabelVisible = true
        lastNoteIndex = index
    }

    ClavierManipulator {
        id: clavierManipulator
        anchors.fill: parent
        clavierViewModel: clavier.clavierViewModel
        animationViewModel: clavier.animationViewModel
    }

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 128 * clavier.keyHeight
        y: clavier.viewportY

        Repeater {
            id: keyRepeater
            model: 128
            Rectangle {
                required property int index
                readonly property string keyName: clavier.dummyKeyName + clavier.keyName(index)
                readonly property bool isBlackKey: clavier.isBlackKey(index)
                readonly property double textYOffset: (clavier.keyYFactor[index % 12] - index % 12 - 0.5) * clavier.keyHeight
                property bool isLeftLabelVisible: false
                readonly property bool isRightLabelVisible: clavier.labelStrategy === Clavier.All || labelStrategy === Clavier.C && index % 12 === 0
                readonly property color normalColor: isBlackKey ? clavier.palette.blackKeyBackgroundColor : clavier.palette.whiteKeyBackgroundColor
                readonly property color hoverColor: isBlackKey ? clavier.palette.blackKeyForegroundColor : clavier.palette.whiteKeyForegroundColor
                readonly property color pressedColor: Qt.rgba(normalColor.r / 2 + hoverColor.r / 2, normalColor.g / 2 + hoverColor.g / 2, normalColor.b / 2 + hoverColor.b / 2, normalColor.a / 2 + hoverColor.a / 2)
                anchors.left: parent.left
                width: parent.width * (isBlackKey ? 0.75 : 1)
                height: clavier.keyHeight * clavier.keyHeightFactor[index % 12]
                y: clavier.calculateYFromKey(index)
                z: isBlackKey ? 1 : 0
                color: normalColor
                border.width: 1
                border.color: clavier.palette.borderColor
                topRightRadius: isBlackKey ? 4 : 0
                bottomRightRadius: isBlackKey ? 4 : 0
                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.top
                    anchors.verticalCenterOffset: textYOffset
                    text: parent.keyName
                    visible: parent.isLeftLabelVisible
                    color: isBlackKey ? clavier.palette.blackKeyTextColor : clavier.palette.whiteKeyTextColor
                }
                Text {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.top
                    anchors.verticalCenterOffset: textYOffset
                    text: parent.keyName
                    visible: parent.isRightLabelVisible
                    color: isBlackKey ? clavier.palette.blackKeyTextColor : clavier.palette.whiteKeyTextColor
                }
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onPressed: {
                        parent.color = parent.pressedColor
                        clavier.noteOn(parent.index)
                    }
                    onReleased: {
                        parent.color = containsMouse ? parent.hoverColor : parent.normalColor
                        clavier.noteOff(parent.index)
                    }
                    onEntered: {
                        parent.color = pressed ? parent.pressedColor : parent.hoverColor
                    }
                    onExited: {
                        parent.color = pressed ? parent.pressedColor : parent.normalColor
                    }
                }
            }
        }
    }

    StandardScrollHandler {
        anchors.fill: parent
        viewModel: clavier.scrollBehaviorViewModel
        onZoomed: function (_, ratioY, _, y, isPhysicalWheel) {
            clavierManipulator.zoomOnWheel(ratioY, y, isPhysicalWheel)
        }
        onMoved: function (_, y, isPhysicalWheel) {
            clavierManipulator.moveViewBy(y, isPhysicalWheel)
        }
    }

    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: clavier.scrollBehaviorViewModel
        onMoved: function (_, y) {
            clavierManipulator.moveViewBy(y)
        }
    }

}