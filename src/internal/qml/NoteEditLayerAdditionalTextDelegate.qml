import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: note
    property bool additionalTextEditing: false
    property color color: Theme.accentColor
    property string additionalText: ""
    property bool additionalTextHighlighted: false
    property QtObject noteViewModel: SequenceSlicerLoader.viewModel

    opacity: additionalTextEditing ? 0 : 1

    implicitHeight: additionalTextItem.implicitHeight

    Binding {
        note.additionalText: note.noteViewModel?.additionalText ?? ""
        note.additionalTextHighlighted: note.noteViewModel?.additionalTextHighlighted ?? false
        when: note.SequenceSlicerLoader.inRange
    }

    Text {
        id: additionalTextItem
        leftPadding: 6.5
        rightPadding: 6.5
        topPadding: 4
        bottomPadding: 4
        anchors.fill: parent
        color: note.additionalTextHighlighted ? note.color : Theme.foregroundSecondaryColor
        font: Theme.font
        text: note.additionalText
        visible: width >= implicitWidth
    }
}