pragma ComponentBehavior: Bound

import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: clipPaneBackground

    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property ListViewModel trackListViewModel: null
    property TrackListLayoutViewModel trackListLayoutViewModel: null

    clip: true

    TrackListManipulator {
        id: trackListManipulator

        target: clipPaneBackground
        trackListLayoutViewModel: clipPaneBackground.trackListLayoutViewModel
        trackListViewModel: clipPaneBackground.trackListViewModel
    }
    Rectangle {
        id: background

        anchors.fill: parent
        color: SFPalette.editAreaPrimaryColor
    }
    PianoRollScale {
        anchors.fill: parent
        barScaleColor: SFPalette.scalePrimaryColor
        beatScaleColor: SFPalette.scaleSecondaryColor
        segmentScaleColor: SFPalette.scaleTertiaryColor
        timeLayoutViewModel: clipPaneBackground.timeLayoutViewModel
        timeViewModel: clipPaneBackground.timeViewModel
    }
    Item {
        id: trackSplitters

        anchors.left: parent.left
        anchors.right: parent.right
        y: -(clipPaneBackground.trackListLayoutViewModel?.viewportOffset ?? 0)

        Repeater {
            id: trackSpaceRepeater

            model: (clipPaneBackground.trackListViewModel?.count ?? 0) + 1

            Item {
                required property int index
                readonly property QtObject trackViewModel: index !== (clipPaneBackground.trackListViewModel?.count ?? 0) ? clipPaneBackground.trackListViewModel.items[index] : null

                width: parent.width
                height: trackViewModel?.rowHeight ?? 0
                y: trackListManipulator.map[index] ?? 0
                Rectangle {
                    width: parent.width
                    height: 2
                    anchors.verticalCenter: parent.top
                    color: SFPalette.scaleTertiaryColor
                }
            }
        }
    }
}
