import QtQml
import QtQuick
import QtQuick.Shapes
import QtQuick.Layouts
import QtQuick.Controls.impl

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: clip

    property bool current: false
    property bool editScopeFocused: false
    property bool selected: false
    property url iconSource: ""
    property string name: ""
    property bool mute: false
    property bool overlapped: false
    property bool editing: false
    property double headerMargin: 0
    property Component thumbnailComponent: null
    property color color
    required property SelectionController selectionController

    property QtObject clipViewModel: SequenceSlicerLoader.viewModel

    clip: true

    Binding {
        clip.z: clip.current ? 2 : clip.clipViewModel?.selected ? 1 : 0
        clip.selected: clip.clipViewModel?.selected ?? false
        clip.name: clip.clipViewModel?.name ?? ""
        clip.iconSource: clip.clipViewModel?.iconSource ?? ""
        clip.overlapped: clip.clipViewModel?.overlapped ?? false
        clip.mute: clip.clipViewModel?.mute ?? false
        clip.editing: clip.clipViewModel?.editing ?? false
        when: clip.SequenceSlicerLoader.inRange
    }

    Binding {
        clip.editScopeFocused: clip.selectionController?.editScopeFocused ?? false
        when: clip.SequenceSlicerLoader.inRange
    }

    Rectangle {
        anchors.fill: parent
        anchors.margins: 0.5
        readonly property color thumbnailColor: SFPalette.clipThumbnailColorChange.apply(clip.mute ? SFPalette.clipMuteColor : clip.color)
        color: clip.selected ? SFPalette.clipSelectedColorChange.apply(thumbnailColor) : thumbnailColor
        border.width: 2
        border.color: clip.selected ? Theme.foregroundPrimaryColor : clip.overlapped ? Theme.warningColor : clip.color ?? Qt.rgba(0, 0, 0, 0)
        radius: 4
    }

    Rectangle {
        id: headerRect
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 2.5
        height: 18
        topLeftRadius: 2
        topRightRadius: 2
        readonly property color baseColor: clip.mute ? SFPalette.clipMuteColor : clip.color
        color: clip.selected ? SFPalette.clipSelectedColorChange.apply(baseColor) : baseColor
        clip: true
        RowLayout {
            spacing: 4
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: Math.max(2, Math.min(clip.headerMargin, parent.width - width - 2))
            width: Math.min(implicitWidth, clip.width - 10)
            IconImage {
                Layout.alignment: Qt.AlignVCenter
                source: clip.iconSource
                sourceSize.width: 16
                sourceSize.height: 16
                color: Theme.foregroundPrimaryColor
            }
            Text {
                Layout.alignment: Qt.AlignVCenter
                Layout.fillWidth: true
                text: clip.name
                font: Theme.font
                color: Theme.foregroundPrimaryColor
                elide: Text.ElideRight
            }
            Rectangle {
                Layout.alignment: Qt.AlignVCenter
                Layout.leftMargin: 4
                width: 8
                height: 8
                radius: 4
                visible: clip.current
                border.color: clip.editScopeFocused ? Theme.foregroundPrimaryColor : Theme.foregroundSecondaryColor
                color: clip.editScopeFocused ? Theme.accentColor : Theme.foregroundSecondaryColor
            }
            Shape {
                visible: clip.editing
                ShapePath {
                    strokeColor: Theme.foregroundPrimaryColor
                    strokeWidth: clip.editScopeFocused ? 1 : 0
                    fillColor: clip.editScopeFocused ? Theme.accentColor : Theme.foregroundSecondaryColor
                    fillRule: ShapePath.WindingFill
                    // ic_fluent_edit_12_filled
                    PathSvg { path: "M 7.736 1.56 C 8.48379 0.812483 9.69598 0.812706 10.4435 1.5605 C 11.191 2.30829 11.1908 3.52048 10.443 4.268 L 10.209 4.502 L 7.502 1.795 L 7.736 1.56 M 6.795 2.502 L 1.65 7.646 C 1.58055 7.71576 1.53324 7.80447 1.514 7.901 L 1.01 10.401 C 0.976604 10.5651 1.02759 10.735 1.1458 10.8537 C 1.26402 10.9723 1.43378 11.0238 1.598 10.991 L 4.102 10.491 C 4.19865 10.4715 4.28737 10.4238 4.357 10.354 L 9.502 5.209 L 6.795 2.502 " }
                }
            }
        }
    }

    Item {
        id: thumbnailContainer
        anchors.top: headerRect.bottom
        anchors.left: parent.left
        anchors.leftMargin: 2.5
        anchors.right: parent.right
        anchors.rightMargin: 2.5
        anchors.bottom: parent.bottom
        anchors.bottomMargin:4.5
        clip: true
        property alias thumbnailComponent: clip.thumbnailComponent
        property Item thumbnailItem: null
        function loadThumbnailItem() {
            if (thumbnailItem) {
                thumbnailItem.destroy()
            }
            if (!thumbnailComponent)
                return
            thumbnailItem = thumbnailComponent.createObject(thumbnailBoundingItem, {
                "clipViewModel": clip.clipViewModel,
                "visualVisible": clip.SequenceSlicerLoader.inRange
            })
            if (thumbnailItem) {
                thumbnailItem.clipViewModel = Qt.binding(() => clip.clipViewModel)
                thumbnailItem.visualVisible = Qt.binding(() => clip.SequenceSlicerLoader.inRange)
                thumbnailItem.anchors.fill = thumbnailBoundingItem
            }
        }
        Item {
            id: thumbnailBoundingItem
            anchors.fill: parent
            anchors.leftMargin: -2.5
            anchors.rightMargin: -2.5
            data: [thumbnailContainer.thumbnailItem]
        }
        Component.onCompleted: loadThumbnailItem()
        onThumbnailComponentChanged: loadThumbnailItem()
    }
}