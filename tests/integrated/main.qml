import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import dev.sjimo.ScopicFlow

Item {
    id: main
    required property QtObject timeAlignmentViewModel
    required property QtObject arrangementTimeAlignmentViewModel
    required property QtObject clavierViewModel
    required property QtObject trackListViewModel
    required property QtObject trackListLayoutViewModel
    required property QtObject labelSequenceViewModel
    required property QtObject playbackViewModel
    required property QtObject scrollBehaviorViewModel
    required property QtObject animationViewModel
    required property QtObject paletteViewModel

    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        SplitView {
            SplitView.preferredHeight: 300
            orientation: Qt.Horizontal
            ColumnLayout {
                SplitView.preferredWidth: 360
                SplitView.minimumWidth: 256
                spacing: 0
                Item {
                    Layout.fillWidth: true
                    height: placeholder.y - arrangementTimeline.y
                }
                TrackList {
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    trackListViewModel: main.trackListViewModel
                    trackListLayoutViewModel: main.trackListLayoutViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel
                    animationViewModel: main.animationViewModel
                    paletteViewModel: main.paletteViewModel
                    trackExtraDelegate: Rectangle {
                        required property QtObject trackViewModel
                        anchors.fill: parent
                        clip: true
                        color: "red"
                        Text {
                            anchors.centerIn: parent
                            text: "Custom Track Extra Delegate\n" +
                                `name = ${parent.trackViewModel.name}\n` +
                                "msr = " +
                                (parent.trackViewModel.mute ? "[M]" : "") +
                                (parent.trackViewModel.solo ? "[S]" : "") +
                                (parent.trackViewModel.record ? "[R]" : "") + "\n" +
                                `gain = ${parent.trackViewModel.gain}\n` +
                                `pan = ${parent.trackViewModel.pan}\n` +
                                `selected = ${parent.trackViewModel.selected}\n` +
                                `intermediate = ${parent.trackViewModel.intermediate}`
                        }
                    }
                }
            }
            ColumnLayout {
                spacing: 0
                Timeline {
                    id: arrangementTimeline
                    Layout.minimumHeight: 24
                    Layout.fillWidth: true

                    timeAlignmentViewModel: main.arrangementTimeAlignmentViewModel
                    playbackViewModel: main.playbackViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel
                    animationViewModel: main.animationViewModel
                    paletteViewModel: main.paletteViewModel
                }
                LabelSequence {

                    Layout.minimumHeight: 16
                    Layout.fillWidth: true

                    timeAlignmentViewModel: main.arrangementTimeAlignmentViewModel
                    playbackViewModel: main.playbackViewModel
                    labelSequenceViewModel: main.labelSequenceViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel
                    animationViewModel: main.animationViewModel
                    paletteViewModel: main.paletteViewModel
                }
                Item {
                    id: placeholder
                    Layout.fillHeight: true
                }
            }
        }

        GridLayout {
            columnSpacing: 0
            rowSpacing: 0

            Clavier {
                Layout.row: 0
                Layout.rowSpan: 3
                Layout.column: 0
                Layout.minimumWidth: 96
                Layout.fillHeight: true

                clavierViewModel: main.clavierViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                animationViewModel: main.animationViewModel
                paletteViewModel: main.paletteViewModel
                topMargin: pianoRollSplitView.y - timeline.y
                bottomMargin: pianoRoll.bottomMargin
            }

            Timeline {
                id: timeline
                Layout.row: 0
                Layout.column: 1
                Layout.minimumHeight: 24
                Layout.fillWidth: true

                timeAlignmentViewModel: main.timeAlignmentViewModel
                playbackViewModel: main.playbackViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                animationViewModel: main.animationViewModel
                paletteViewModel: main.paletteViewModel
            }

            LabelSequence {
                Layout.row: 1
                Layout.column: 1
                Layout.minimumHeight: 16
                Layout.fillWidth: true

                timeAlignmentViewModel: main.timeAlignmentViewModel
                playbackViewModel: main.playbackViewModel
                labelSequenceViewModel: main.labelSequenceViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                animationViewModel: main.animationViewModel
                paletteViewModel: main.paletteViewModel
            }

            SplitView {
                id: pianoRollSplitView
                orientation: Qt.Vertical
                Layout.row: 2
                Layout.column: 1
                Layout.fillWidth: true
                Layout.fillHeight: true
                Item {
                    SplitView.fillHeight: true
                    PianoRoll {
                        id: pianoRoll
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: pianoRollSplitView.height
                        timeAlignmentViewModel: main.timeAlignmentViewModel
                        playbackViewModel: main.playbackViewModel
                        clavierViewModel: main.clavierViewModel
                        scrollBehaviorViewModel: main.scrollBehaviorViewModel
                        animationViewModel: main.animationViewModel
                        paletteViewModel: main.paletteViewModel
                        bottomMargin: height - parent.height
                    }
                }
                Rectangle {
                    SplitView.preferredHeight: 128
                    color: "red"
                    opacity: 0.5
                }
            }
        }
    }

}