import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal as Internal

Item {
    id: main
    required property QtObject timeAlignmentViewModel
    required property QtObject playbackViewModel
    required property QtObject scrollBehaviorViewModel
    required property QtObject animationViewModel
    required property QtObject paletteViewModel

    required property QtObject pointSequenceViewModel

    ColumnLayout {
        anchors.fill: parent
        Timeline {
            id: timeline
            Layout.fillWidth: true
            Layout.minimumHeight: 24

            timeAlignmentViewModel: main.timeAlignmentViewModel
            playbackViewModel: main.playbackViewModel
            scrollBehaviorViewModel: main.scrollBehaviorViewModel
            animationViewModel: main.animationViewModel
            paletteViewModel: main.paletteViewModel
        }

        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 16
            Internal.SequenceSlicer {
                model: main.pointSequenceViewModel
                timeViewModel: main.timeAlignmentViewModel
                sliceWidth: timeline.width
                sectionLength: 1920
                delegate: Text {
                    required property QtObject model
                    text: model.text
                    Component.onCompleted: {
                        console.log(text)
                    }
                }
            }
        }

    }

}