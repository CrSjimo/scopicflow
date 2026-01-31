import QtQml
import QtQuick

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
    property double headerMargin: 0
    property Component thumbnail: null
    required property SelectionController selectionController

    property QtObject clipViewModel: SequenceSlicerLoader.viewModel

    Rectangle {
        anchors.fill: parent
        readonly property color thumbnailColor: SFPalette.clipThumbnailColorChange.apply(clip.clipViewModel?.color ?? Qt.rgba(0, 0, 0, 0))
        color: clip.selected ? SFPalette.clipSelectedColorChange.apply(thumbnailColor) : thumbnailColor
        border.width: 2
        border.color: clip.selected ? Theme.foregroundPrimaryColor : clip.clipViewModel?.color ?? Qt.rgba(0, 0, 0, 0)
        radius: 4
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 20
        topLeftRadius: 4
        topRightRadius: 4
        readonly property color baseColor: clip.clipViewModel?.color ?? Qt.rgba(0, 0, 0, 0)
        color: clip.selected ? SFPalette.clipSelectedColorChange.apply(baseColor) : baseColor
    }
}