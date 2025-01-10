import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow.Palette as ScopicFlowPalette

Item {
    id: clavier

    enum LabelStrategy {
        None,
        C,
        All
    }

    property QtObject animationViewModel: null
    property double bottomMargin: 0
    property QtObject clavierViewModel: null
    readonly property int cursorNoteIndex: clavierViewModel?.cursorPosition ?? -1
    readonly property QtObject defaultPalette: ScopicFlowPalette.Clavier {
    }
    readonly property double keyHeight: clavierViewModel?.pixelDensity ?? 24
    readonly property list<double> keyHeightFactor: [5 / 3, 1, 5 / 3, 1, 5 / 3, 7 / 4, 1, 7 / 4, 1, 7 / 4, 1, 7 / 4]
    readonly property list<double> keyYFactor: [5 / 3, 2, 10 / 3, 4, 15 / 3, 27 / 4, 7, 34 / 4, 9, 41 / 4, 11, 12]
    property int labelStrategy: Clavier.LabelStrategy.C
    property int lastNoteIndex: -1
    readonly property QtObject palette: paletteViewModel?.palette?.clavier ?? defaultPalette
    property QtObject paletteViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property double topMargin: 0
    readonly property double viewportY: clavierViewModel ? height - (128 - clavierViewModel.start) * clavierViewModel.pixelDensity : 0

    signal contextMenuRequestedForNote(key: int)
    signal noteDoubleClicked(key: int)
    signal notePressed(key: int)
    signal noteReleased(key: int)

    function calculateYFromKey(key) {
        let indexInGroup = key % 12;
        let group = (key - indexInGroup) / 12;

        return (128 - group * 12 - keyYFactor[indexInGroup]) * clavier.keyHeight;
    }
    function isBlackKey(key) {
        let indexInGroup = key % 12;
        return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10;
    }
    function mapToKey(y) {
        return 127 - Math.floor((y - viewportY) / keyHeight);
    }

    clip: true

    onCursorNoteIndexChanged: {
        if (lastNoteIndex >= 0)
            keyRepeater.itemAt(lastNoteIndex).isLabelVisible = false;
        if (cursorNoteIndex >= 0)
            keyRepeater.itemAt(cursorNoteIndex).isLeftLabelVisible = true;
        lastNoteIndex = cursorNoteIndex;
    }
    onHeightChanged: {
        if (clavier.clavierViewModel) {
            clavier.clavierViewModel.start = Math.min(clavier.clavierViewModel.start, 128 - (clavier.height - clavier.topMargin) / clavier.clavierViewModel.pixelDensity);
        }
    }

    ClavierManipulator {
        id: clavierManipulator

        anchors.fill: parent
        anchors.topMargin: clavier.topMargin
        animationViewModel: clavier.animationViewModel
        clavierViewModel: clavier.clavierViewModel
        startOffset: -clavier.bottomMargin
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
                readonly property color hoverColor: isBlackKey ? clavier.palette.blackKeyHoveredColor : clavier.palette.whiteKeyHoveredColor
                required property int index
                readonly property bool isBlackKey: clavier.isBlackKey(index)
                property bool isLeftLabelVisible: false
                readonly property bool isRightLabelVisible: clavier.labelStrategy === Clavier.All || clavier.labelStrategy === Clavier.C && index % 12 === 0
                readonly property string keyName: ClavierHelper.keyNameImpl(index, clavier.clavierViewModel?.accidentalType ?? 0)
                readonly property color normalColor: isBlackKey ? clavier.palette.blackKeyBackgroundColor : clavier.palette.whiteKeyBackgroundColor
                readonly property color pressedColor: isBlackKey ? clavier.palette.blackKeyPressedColor : clavier.palette.whiteKeyPressedColor
                readonly property double textYOffset: (clavier.keyYFactor[index % 12] - index % 12 - 0.5) * clavier.keyHeight

                anchors.left: parent.left
                border.color: clavier.palette.borderColor
                border.width: 1
                bottomRightRadius: isBlackKey ? 4 : 0
                color: normalColor
                height: clavier.keyHeight * clavier.keyHeightFactor[index % 12]
                topRightRadius: isBlackKey ? 4 : 0
                width: parent.width * (isBlackKey ? 0.75 : 1)
                y: clavier.calculateYFromKey(index)
                z: isBlackKey ? 1 : 0

                Behavior on color {
                    ColorAnimation {
                        duration: (clavier.animationViewModel?.colorAnimationRatio ?? 1.0) * 250
                        easing.type: Easing.OutCubic
                    }
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.top
                    anchors.verticalCenterOffset: parent.textYOffset
                    color: parent.isBlackKey ? clavier.palette.blackKeyTextColor : clavier.palette.whiteKeyTextColor
                    text: parent.keyName
                    visible: parent.isLeftLabelVisible
                }
                Text {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.top
                    anchors.verticalCenterOffset: parent.textYOffset
                    color: parent.isBlackKey ? clavier.palette.blackKeyTextColor : clavier.palette.whiteKeyTextColor
                    text: parent.keyName
                    visible: parent.isRightLabelVisible
                }
                MouseArea {
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    anchors.fill: parent
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true

                    onClicked: function (mouse) {
                        if (mouse.button === Qt.RightButton) {
                            clavier.contextMenuRequestedForNote(parent.index);
                        }
                    }
                    onDoubleClicked: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            clavier.noteDoubleClicked(parent.index);
                        }
                    }
                    onEntered: {
                        parent.color = pressed ? parent.pressedColor : parent.hoverColor;
                    }
                    onExited: {
                        parent.color = pressed ? parent.pressedColor : parent.normalColor;
                    }
                    onPressed: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            parent.color = parent.pressedColor;
                            clavier.notePressed(parent.index);
                        }
                    }
                    onReleased: function (mouse) {
                        if (mouse.button === Qt.LeftButton) {
                            parent.color = containsMouse ? parent.hoverColor : parent.normalColor;
                            clavier.noteReleased(parent.index);
                        }
                    }
                }
            }
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        viewModel: clavier.scrollBehaviorViewModel

        onMoved: (_, y, isPhysicalWheel) => {
            clavierManipulator.moveViewBy(y, isPhysicalWheel);
        }
        onZoomed: (_, ratioY, _, y, isPhysicalWheel) => {
            clavierManipulator.zoomOnWheel(ratioY, y, isPhysicalWheel);
        }
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        viewModel: clavier.scrollBehaviorViewModel
        direction: Qt.Vertical

        onMoved: (_, y) => {
            clavierManipulator.moveViewBy(y);
        }
    }
}
