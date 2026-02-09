import QtQml
import QtQuick
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
        when: clip.SequenceSlicerLoader.inRange
    }

    Binding {
        clip.editScopeFocused: clip.selectionController?.editScopeFocused ?? false
        when: clip.SequenceSlicerLoader.inRange && clip.current
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
        Row {
            spacing: 8
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: Math.max(2, Math.min(clip.headerMargin, parent.width - width - 2))
            IconLabel {
                spacing: 4
                anchors.verticalCenter: parent.verticalCenter
                icon.source: clip.iconSource
                icon.width: 16
                icon.height: 16
                icon.color: Theme.foregroundPrimaryColor
                text: clip.name
                font: Theme.font
                color: Theme.foregroundPrimaryColor
            }
            Rectangle {
                anchors.verticalCenter: parent.verticalCenter
                width: 8
                height: 8
                radius: 4
                visible: clip.current
                border.color: clip.editScopeFocused ? Theme.foregroundPrimaryColor : Theme.foregroundSecondaryColor
                color: clip.editScopeFocused ? Theme.accentColor : Theme.foregroundSecondaryColor
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