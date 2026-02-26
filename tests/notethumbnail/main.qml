import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Views
import dev.sjimo.ScopicFlow.Internal

ApplicationWindow {
    id: main
    visible: true
    width: 640
    height: 480

    required property RangeSequenceViewModel noteSequenceViewModel

    NoteThumbnail {
        id: noteThumbnail
        anchors.fill: parent
        color: "red"
        noteSequenceViewModel: main.noteSequenceViewModel
    }

}