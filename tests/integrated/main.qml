import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Item {
    id: main
    required property QtObject timeViewModel
    required property QtObject arrangementTimeViewModel
    required property QtObject timeLayoutViewModel
    required property QtObject arrangementTimeLayoutViewModel
    required property QtObject clavierViewModel
    required property QtObject trackListViewModel
    required property QtObject trackListLayoutViewModel
    required property QtObject labelSequenceViewModel
    required property QtObject labelSequenceBehaviorViewModel
    required property QtObject arrangementLabelSequenceBehaviorViewModel
    required property QtObject playbackViewModel
    required property QtObject scrollBehaviorViewModel
    required property QtObject animationViewModel
    property QtObject noteSequenceViewModel: null
    required property QtObject pianoRollNoteAreaBehaviorViewModel
    required property QtObject backNoteSequenceViewModel
    required property QtObject backPianoRollNoteAreaBehaviorViewModel
    required property QtObject clipSequenceViewModel
    required property QtObject clipPaneBehaviorViewModel

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
                    height: clipPane.y - arrangementTimeline.y
                }
                TrackList {
                    id: trackList
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    trackListViewModel: main.trackListViewModel
                    trackListLayoutViewModel: main.trackListLayoutViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel
                    animationViewModel: main.animationViewModel
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
                    Layout.fillWidth: true

                    timeViewModel: main.arrangementTimeViewModel
                    timeLayoutViewModel: main.arrangementTimeLayoutViewModel
                    playbackViewModel: main.playbackViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel
                    animationViewModel: main.animationViewModel
                }
                LabelSequence {
                    id: arrangementLabelSequence
                    Layout.fillWidth: true

                    timeViewModel: main.arrangementTimeViewModel
                    timeLayoutViewModel: main.arrangementTimeLayoutViewModel
                    playbackViewModel: main.playbackViewModel
                    labelSequenceViewModel: main.labelSequenceViewModel
                    labelSequenceBehaviorViewModel: main.arrangementLabelSequenceBehaviorViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel
                    animationViewModel: main.animationViewModel
                }
                ClipPane {
                    id: clipPane
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    timeViewModel: main.arrangementTimeViewModel
                    timeLayoutViewModel: main.arrangementTimeLayoutViewModel
                    playbackViewModel: main.playbackViewModel
                    scrollBehaviorViewModel: main.scrollBehaviorViewModel
                    animationViewModel: main.animationViewModel
                    trackListViewModel: main.trackListViewModel
                    trackListLayoutViewModel: main.trackListLayoutViewModel
                    clipSequenceViewModel: main.clipSequenceViewModel
                    clipPaneBehaviorViewModel: main.clipPaneBehaviorViewModel

                    clipGraph: ScopicFlowInternal.NoteThumbnail {
                        anchors.fill: parent
                        required property QtObject model
                        noteSequenceViewModel: model?.noteSequenceViewModel ?? null
                        position: model.clipStart
                        length: model.length
                    }

                    property QtObject currentClip: null
                    Connections {
                        target: clipPane.currentClip
                        function onPositionChanged() {
                            main.pianoRollNoteAreaBehaviorViewModel.offset = target.position - target.clipStart
                        }
                        function onClipStartChanged() {
                            main.pianoRollNoteAreaBehaviorViewModel.offset = target.position - target.clipStart
                        }
                    }

                    Connections {
                        target: main.trackListViewModel?.handle.items[clipPane.currentClip?.trackNumber ?? 0] ?? null
                        function onColorChanged() {
                            main.pianoRollNoteAreaBehaviorViewModel.color = target.color
                        }
                    }

                    onClipDoubleClicked: (model) => {
                        currentClip = model
                        main.noteSequenceViewModel = model.noteSequenceViewModel
                        main.pianoRollNoteAreaBehaviorViewModel.offset = model.position - model.clipStart
                        main.pianoRollNoteAreaBehaviorViewModel.color = main.trackListViewModel.handle.items[model.trackNumber].color
                        main.timeViewModel.start = model.position
                    }

                }
            }
        }

        GridLayout {
            columnSpacing: 0
            rowSpacing: 0

            Clavier {
                id: clavier
                Layout.row: 0
                Layout.rowSpan: 3
                Layout.column: 0
                Layout.minimumWidth: 96
                Layout.fillHeight: true

                clavierViewModel: main.clavierViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                animationViewModel: main.animationViewModel
                                topMargin: pianoRollSplitView.y - timeline.y
                bottomMargin: pianoRoll.bottomMargin
            }

            Timeline {
                id: timeline
                Layout.row: 0
                Layout.column: 1
                Layout.fillWidth: true

                timeViewModel: main.timeViewModel
                timeLayoutViewModel: main.timeLayoutViewModel
                playbackViewModel: main.playbackViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                animationViewModel: main.animationViewModel
            }

            LabelSequence {
                id: labelSequence
                Layout.row: 1
                Layout.column: 1
                Layout.fillWidth: true

                timeViewModel: main.timeViewModel
                timeLayoutViewModel: main.timeLayoutViewModel
                playbackViewModel: main.playbackViewModel
                labelSequenceViewModel: main.labelSequenceViewModel
                labelSequenceBehaviorViewModel: main.labelSequenceBehaviorViewModel
                scrollBehaviorViewModel: main.scrollBehaviorViewModel
                animationViewModel: main.animationViewModel
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

                        timeViewModel: main.timeViewModel
                        timeLayoutViewModel: main.timeLayoutViewModel
                        playbackViewModel: main.playbackViewModel
                        clavierViewModel: main.clavierViewModel
                        scrollBehaviorViewModel: main.scrollBehaviorViewModel
                        animationViewModel: main.animationViewModel

                        viewItems: [
                            Component {
                                PianoRollNoteArea {
                                    anchors.fill: parent
                                    z: 2
                                    timeViewModel: main.timeViewModel
                                    timeLayoutViewModel: main.timeLayoutViewModel
                                    clavierViewModel: main.clavierViewModel
                                    animationViewModel: main.animationViewModel
                                    noteSequenceViewModel: main.noteSequenceViewModel
                                    pianoRollNoteAreaBehaviorViewModel: main.pianoRollNoteAreaBehaviorViewModel
                                }
                            },
                            Component {
                                PianoRollNoteArea {
                                    anchors.fill: parent
                                    opacity: 0.5
                                    z: 1
                                    timeViewModel: main.timeViewModel
                                    timeLayoutViewModel: main.timeLayoutViewModel
                                    clavierViewModel: main.clavierViewModel
                                    animationViewModel: main.animationViewModel
                                    noteSequenceViewModel: main.backNoteSequenceViewModel
                                    pianoRollNoteAreaBehaviorViewModel: main.backPianoRollNoteAreaBehaviorViewModel
                                }
                            }
                        ]

                        bottomMargin: height - parent.height
                    }
                }
                Pane {
                    SplitView.preferredHeight: 128
                    focusPolicy: Qt.StrongFocus
                    opacity: 0.5
                    padding: 0
                }
            }
        }
    }

}