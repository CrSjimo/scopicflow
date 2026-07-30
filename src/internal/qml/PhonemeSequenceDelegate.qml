pragma ComponentBehavior: Bound

import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: phoneme

    property string content: ""
    property bool edited: false
    property bool editing: false
    property double textAreaWidth: 0
    property int textHorizontalAlignment: Text.AlignLeft

    readonly property PhonemeViewModel phonemeViewModel: SequenceSlicerLoader.viewModel

    visible: SequenceSlicerLoader.inRange
    implicitWidth: textMetrics.advanceWidth + 8
    implicitHeight: 20
    width: 2
    z: 1

    Rectangle {
        anchors.fill: parent
        color: phoneme.edited ? Theme.accentColor : Theme.foregroundPrimaryColor
    }

    Item {
        id: textArea

        clip: true
        x: phoneme.width
        width: phoneme.textAreaWidth
        height: phoneme.height

        TextMetrics {
            id: textMetrics

            elide: Text.ElideRight
            elideWidth: Math.max(0, textArea.width - 4)
            font: Theme.font
            text: phoneme.content
        }

        Text {
            anchors.fill: parent
            leftPadding: 2
            rightPadding: 2
            color: Theme.foregroundPrimaryColor
            font: Theme.font
            horizontalAlignment: phoneme.textHorizontalAlignment
            text: textMetrics.elidedText
            verticalAlignment: Text.AlignVCenter
            visible: !phoneme.editing && text.length > 0
        }
    }
}
