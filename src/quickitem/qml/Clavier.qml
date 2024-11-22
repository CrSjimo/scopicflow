import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Private.Internal as ScopicFlowInternal
import dev.sjimo.ScopicFlow.Palettes as ScopicFlowPalette
import "."

ScopicFlowInternal.Clavier {
    id: clavier

    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null
    enum LabelStrategy {
        None,
        C,
        All
    }
    property int labelStrategy: ScopicFlowInternal.Clavier.LabelStrategy.C

    readonly property double keyHeight: animationViewModel?.pixelDensity ?? 24
    readonly property double viewportY: clavierViewModel ? height - (128 - clavierViewModel.start) * clavierViewModel.pixelDensity : 0
    readonly property int cursorNoteIndex: clavierViewModel?.cursorPosition ?? -1

    function mapToKey(y) {
        return 127 - Math.floor((y - viewportY) / keyHeight);
    }

    signal notePressed(key: int)
    signal noteReleased(key: int)
    signal noteDoubleClicked(key: int)
    signal contextMenuRequestedForNote(key: int)

    property double topMargin: 0
    property double bottomMargin: 0

    readonly property QtObject defaultPalette: ScopicFlowPalette.Clavier {}
    readonly property QtObject palette: paletteViewModel?.palette?.clavier ?? defaultPalette

    clip: true


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

    onCursorNoteIndexChanged: {
        if (lastNoteIndex >= 0)
            keyRepeater.itemAt(lastNoteIndex).isLabelVisible = false
        if (cursorNoteIndex >= 0)
            keyRepeater.itemAt(cursorNoteIndex).isLeftLabelVisible = true
        lastNoteIndex = cursorNoteIndex
    }

    onHeightChanged: {
        if (clavier.clavierViewModel) {
            clavier.clavierViewModel.start = Math.min(clavier.clavierViewModel.start, 128 - (clavier.height - clavier.topMargin) / clavier.clavierViewModel.pixelDensity)
        }
    }

    ClavierManipulator {
        id: clavierManipulator
        anchors.fill: parent
        anchors.topMargin: clavier.topMargin
        startOffset: -clavier.bottomMargin
        clavierViewModel: clavier.clavierViewModel
        animationViewModel: clavier.animationViewModel
    }

    Rectangle {
        anchors.fill: parent
        color: clavier.palette.borderColor
    }

    Item {
        id: clavierViewport
        anchors.left: parent.left
        anchors.right: parent.right
        height: 128 * clavier.keyHeight
        y: clavier.clavierViewModel ? Math.min(clavier.topMargin, clavier.height - (128 - clavier.clavierViewModel.start) * clavier.clavierViewModel.pixelDensity - clavier.bottomMargin) : clavier.topMargin

        Repeater {
            id: keyRepeater
            model: 128
            Rectangle {
                required property int index
                readonly property string keyName: clavier.keyNameImpl(index, clavier.clavierViewModel?.accidentalType ?? 0)
                readonly property bool isBlackKey: clavier.isBlackKey(index)
                readonly property double textYOffset: (clavier.keyYFactor[index % 12] - index % 12 - 0.5) * clavier.keyHeight
                property bool isLeftLabelVisible: false
                readonly property bool isRightLabelVisible: clavier.labelStrategy === Clavier.All || labelStrategy === Clavier.C && index % 12 === 0
                readonly property color normalColor: isBlackKey ? clavier.palette.blackKeyBackgroundColor : clavier.palette.whiteKeyBackgroundColor
                readonly property color hoverColor: isBlackKey ? clavier.palette.blackKeyHoveredColor : clavier.palette.whiteKeyHoveredColor
                readonly property color pressedColor: isBlackKey ? clavier.palette.blackKeyPressedColor : clavier.palette.whiteKeyPressedColor
                anchors.left: parent.left
                width: parent.width * (isBlackKey ? 0.75 : 1)
                height: clavier.keyHeight * clavier.keyHeightFactor[index % 12]
                y: clavier.calculateYFromKey(index)
                z: isBlackKey ? 1 : 0
                color: normalColor
                Behavior on color {
                    ColorAnimation {
                        duration: (clavier.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                        easing.type: Easing.OutCubic
                    }
                }
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
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    focusPolicy: Qt.StrongFocus
                    onPressed: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            parent.color = parent.pressedColor
                            clavier.notePressed(parent.index)
                        }
                    }
                    onReleased: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            parent.color = containsMouse ? parent.hoverColor : parent.normalColor
                            clavier.noteReleased(parent.index)
                        }
                    }
                    onClicked: function (mouse) {
                        if (mouse.button === Qt.RightButton) {
                            clavier.contextMenuRequestedForNote(parent.index)
                        }
                    }
                    onDoubleClicked: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            clavier.noteDoubleClicked(parent.index)
                        }
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
