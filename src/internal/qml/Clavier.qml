import QtQml
import QtQuick
import QtQuick.Templates as T

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

T.Pane {
    id: clavier

    property QtObject clavierViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject interactionControllerNotifier: null

    property double topExpansion: 0
    property double bottomExpansion: 0

    Accessible.name: qsTr("Piano keyboard")
    clip: true
    focus: true
    focusPolicy: Qt.StrongFocus
    implicitWidth: 96
    background: Rectangle { color: Theme.borderColor }

    onHeightChanged: () => {
        if (clavier.clavierViewModel) {
            clavier.clavierViewModel.start = Math.min(clavier.clavierViewModel.start, 128 - (clavier.height - clavier.topExpansion) / clavier.clavierViewModel.pixelDensity);
        }
    }

    // helpers
    QtObject {
        id: d
        readonly property double keyHeight: clavier.clavierViewModel?.pixelDensity ?? 24
        readonly property list<double> keyHeightFactor: [5 / 3, 1, 5 / 3, 1, 5 / 3, 7 / 4, 1, 7 / 4, 1, 7 / 4, 1, 7 / 4]
        readonly property list<double> keyYFactor: [5 / 3, 2, 10 / 3, 4, 15 / 3, 27 / 4, 7, 34 / 4, 9, 41 / 4, 11, 12]
        property int lastNoteIndex: -1
        readonly property int cursorNoteIndex: clavier.clavierViewModel?.cursorPosition ?? -1

        onCursorNoteIndexChanged: () => {
            if (lastNoteIndex >= 0)
                keyRepeater.itemAt(lastNoteIndex).isLeftLabelVisible = false;
            if (cursorNoteIndex >= 0)
                keyRepeater.itemAt(cursorNoteIndex).isLeftLabelVisible = true;
            lastNoteIndex = cursorNoteIndex;
        }

        function calculateYFromKey(key) {
            let indexInGroup = key % 12;
            let group = (key - indexInGroup) / 12;

            return (128 - group * 12 - keyYFactor[indexInGroup]) * keyHeight;
        }
        function isBlackKey(key) {
            let indexInGroup = key % 12;
            return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10;
        }
        function isRightLabelVisible(key) {
            return clavier.clavierViewModel?.labelStrategy === ScopicFlow.LS_All || clavier.clavierViewModel?.labelStrategy === ScopicFlow.LS_C && key % 12 === 0;
        }
    }
    ClavierManipulator {
        id: clavierManipulator
        clavierViewModel: clavier.clavierViewModel
        viewSize: parent.height - clavier.topExpansion
        topExpansion: clavier.topExpansion
        bottomExpansion: clavier.bottomExpansion
    }

    // visual components
    Item {
        id: viewportContainer

        LayoutMirroring.childrenInherit: true
        LayoutMirroring.enabled: false
        anchors.left: parent.left
        anchors.right: parent.right
        height: 128 * d.keyHeight
        y: clavier.clavierViewModel ? Math.min(clavier.topExpansion, clavier.height - (128 - clavier.clavierViewModel.start) * clavier.clavierViewModel.pixelDensity - clavier.bottomExpansion) : clavier.topExpansion

        Repeater {
            id: keyRepeater

            model: 128

            Rectangle {
                id: clavierKey

                required property int index
                readonly property bool isBlackKey: d.isBlackKey(index)
                property bool isLeftLabelVisible: false
                readonly property bool isRightLabelVisible: d.isRightLabelVisible(index)
                readonly property string keyName: ClavierHelper.keyNameImpl(index, clavier.clavierViewModel?.accidentalType ?? 0)
                readonly property double textYOffset: (d.keyYFactor[index % 12] - index % 12 - 0.5) * d.keyHeight

                anchors.left: parent.left
                border.color: Theme.borderColor
                border.width: 1
                bottomRightRadius: isBlackKey ? 4 : 0
                color: mouseArea.pressed ? isBlackKey ? SFPalette.blackKeyPressedColor : SFPalette.whiteKeyPressedColor : mouseArea.containsMouse ? isBlackKey ? SFPalette.blackKeyHoveredColor : SFPalette.whiteKeyHoveredColor : isBlackKey ? SFPalette.blackKeyColor : SFPalette.whiteKeyColor
                height: d.keyHeight * d.keyHeightFactor[index % 12]
                topRightRadius: isBlackKey ? 4 : 0
                width: parent.width * (isBlackKey ? 0.75 : 1)
                y: d.calculateYFromKey(index)
                z: isBlackKey ? 1 : 0

                Behavior on color {
                    ColorAnimation {
                        duration: Theme.colorAnimationDuration
                        easing.type: Easing.OutCubic
                    }
                }

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 8
                    anchors.verticalCenter: parent.top
                    anchors.verticalCenterOffset: parent.textYOffset
                    color: SFPalette.suitableForegroundColor(isBlackKey ? SFPalette.blackKeyColor : SFPalette.whiteKeyColor)
                    text: parent.keyName
                    visible: parent.isLeftLabelVisible
                }
                Text {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.top
                    anchors.verticalCenterOffset: parent.textYOffset
                    color: SFPalette.suitableForegroundColor(isBlackKey ? SFPalette.blackKeyColor : SFPalette.whiteKeyColor)
                    text: parent.keyName
                    visible: parent.isRightLabelVisible
                }
                MouseArea {
                    acceptedButtons: Qt.RightButton
                    anchors.fill: parent
                    hoverEnabled: true
                }
                MouseArea {
                    id: mouseArea
                    acceptedButtons: Qt.LeftButton
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
        }
    }

    // scroll
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Vertical
        viewModel: clavier.scrollBehaviorViewModel

        onMoved: (_, y, isPhysicalWheel) => {
            clavierManipulator.moveViewBy(y, isPhysicalWheel);
        }
        onZoomed: (_, ratioY, _, y, isPhysicalWheel) => {
            clavierManipulator.zoomViewBy(ratioY, y, isPhysicalWheel);
        }
    }
    MiddleButtonMoveHandler {
        anchors.fill: parent
        direction: Qt.Vertical
        viewModel: clavier.scrollBehaviorViewModel

        onMoved: (_, y) => {
            clavierManipulator.moveViewBy(y);
        }
    }
}
