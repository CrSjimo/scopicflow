import QtQml
import QtQuick

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal
import dev.sjimo.ScopicFlow

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
    property QtObject interactionControllerNotifier: null
    readonly property double keyHeight: clavierViewModel?.pixelDensity ?? 24
    readonly property list<double> keyHeightFactor: [5 / 3, 1, 5 / 3, 1, 5 / 3, 7 / 4, 1, 7 / 4, 1, 7 / 4, 1, 7 / 4]
    readonly property list<double> keyYFactor: [5 / 3, 2, 10 / 3, 4, 15 / 3, 27 / 4, 7, 34 / 4, 9, 41 / 4, 11, 12]
    property int lastNoteIndex: -1
    property QtObject scrollBehaviorViewModel: null
    property double topMargin: 0
    readonly property double viewportY: clavierViewModel ? height - (128 - clavierViewModel.start) * clavierViewModel.pixelDensity : 0

    function calculateYFromKey(key) {
        let indexInGroup = key % 12;
        let group = (key - indexInGroup) / 12;

        return (128 - group * 12 - keyYFactor[indexInGroup]) * clavier.keyHeight;
    }
    function isBlackKey(key) {
        let indexInGroup = key % 12;
        return indexInGroup === 1 || indexInGroup === 3 || indexInGroup === 6 || indexInGroup === 8 || indexInGroup === 10;
    }
    function isRightLabelVisible(key) {
        return clavierViewModel?.labelStrategy === ScopicFlow.LS_All || clavierViewModel?.labelStrategy === ScopicFlow.LS_C && key % 12 === 0;
    }
    function mapToKey(y) {
        return 127 - Math.floor((y - viewportY) / keyHeight);
    }

    clip: true

    onCursorNoteIndexChanged: () => {
        if (lastNoteIndex >= 0)
            keyRepeater.itemAt(lastNoteIndex).isLeftLabelVisible = false;
        if (cursorNoteIndex >= 0)
            keyRepeater.itemAt(cursorNoteIndex).isLeftLabelVisible = true;
        lastNoteIndex = cursorNoteIndex;
    }
    onHeightChanged: () => {
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
        color: Theme.borderColor
    }
    Item {
        id: clavierViewport

        LayoutMirroring.childrenInherit: true
        LayoutMirroring.enabled: false
        anchors.left: parent.left
        anchors.right: parent.right
        height: 128 * clavier.keyHeight
        y: clavier.clavierViewModel ? Math.min(clavier.topMargin, clavier.height - (128 - clavier.clavierViewModel.start) * clavier.clavierViewModel.pixelDensity - clavier.bottomMargin) : clavier.topMargin

        Repeater {
            id: keyRepeater

            model: 128

            Rectangle {
                id: clavierKey

                required property int index
                readonly property bool isBlackKey: clavier.isBlackKey(index)
                property bool isLeftLabelVisible: false
                readonly property bool isRightLabelVisible: clavier.isRightLabelVisible(index)
                readonly property string keyName: ClavierHelper.keyNameImpl(index, clavier.clavierViewModel?.accidentalType ?? 0)
                readonly property double textYOffset: (clavier.keyYFactor[index % 12] - index % 12 - 0.5) * clavier.keyHeight

                anchors.left: parent.left
                border.color: Theme.borderColor
                border.width: 1
                bottomRightRadius: isBlackKey ? 4 : 0
                color: mouseArea.pressed ? isBlackKey ? SFPalette.blackKeyPressedColor : SFPalette.whiteKeyPressedColor : mouseArea.containsMouse ? isBlackKey ? SFPalette.blackKeyHoveredColor : SFPalette.whiteKeyHoveredColor : isBlackKey ? SFPalette.blackKeyColor : SFPalette.whiteKeyColor
                height: clavier.keyHeight * clavier.keyHeightFactor[index % 12]
                topRightRadius: isBlackKey ? 4 : 0
                width: parent.width * (isBlackKey ? 0.75 : 1)
                y: clavier.calculateYFromKey(index)
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
                    function sendInteractionNotification(interactionType) {
                        if (clavier.interactionControllerNotifier?.handleSceneInteraction(interactionType, clavier.clavierViewModel, null, 0, parent.index))
                            return false;
                        clavier.interactionControllerNotifier?.sceneInteracted(interactionType, clavier.clavierViewModel, null, 0, parent.index);
                        return true;
                    }

                    acceptedButtons: Qt.RightButton
                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: sendInteractionNotification(ScopicFlow.II_ContextMenu)
                }
                MouseArea {
                    id: mouseArea

                    function sendInteractionNotification(interactionType) {
                        if (clavier.interactionControllerNotifier?.handleSceneInteraction(interactionType, clavier.clavierViewModel, null, 0, parent.index))
                            return false;
                        clavier.interactionControllerNotifier?.sceneInteracted(interactionType, clavier.clavierViewModel, null, 0, parent.index);
                        return true;
                    }

                    acceptedButtons: Qt.LeftButton
                    anchors.fill: parent
                    hoverEnabled: true

                    onCanceled: sendInteractionNotification(ScopicFlow.II_Released)
                    onClicked: sendInteractionNotification(ScopicFlow.II_Clicked)
                    onDoubleClicked: sendInteractionNotification(ScopicFlow.II_DoubleClicked)
                    onEntered: sendInteractionNotification(ScopicFlow.II_HoverEntered)
                    onExited: sendInteractionNotification(ScopicFlow.II_HoverExited)
                    onPressed: mouse => {
                        if (!sendInteractionNotification(ScopicFlow.II_Pressed))
                            mouse.accepted = false;
                    }
                    onReleased: sendInteractionNotification(ScopicFlow.II_Released)
                }
            }
        }
    }
    StandardScrollHandler {
        anchors.fill: parent
        movableOrientation: Qt.Vertical
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
        direction: Qt.Vertical
        viewModel: clavier.scrollBehaviorViewModel

        onMoved: (_, y) => {
            clavierManipulator.moveViewBy(y);
        }
    }
}
