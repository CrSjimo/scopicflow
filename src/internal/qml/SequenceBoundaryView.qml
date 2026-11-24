import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: view
    property var sequenceViewModel: null
    property var timeViewModel: null
    property var timeLayoutViewModel: null
    property var clipViewModel: null
    clip: true

    readonly property real pixelDensity: timeLayoutViewModel?.pixelDensity ?? 0

    ClipMappedProxyTimeViewModel {
        id: proxyTimeViewModel
        clipViewModel: view.clipViewModel
        timeViewModel: view.timeViewModel
    }

    TimeViewportContainer {
        timeLayoutViewModel: view.timeLayoutViewModel
        timeViewModel: proxyTimeViewModel

        Item {
            anchors.fill: parent

            SequenceSlicer {
                sliceWidth: view.width
                timeLayoutViewModel: view.timeLayoutViewModel
                timeViewModel: proxyTimeViewModel
                viewModel: view.sequenceViewModel

                delegate: Item {
                    id: boundary

                    readonly property var sequenceItem: SequenceSlicerLoader.viewModel
                    readonly property real startX:
                        (sequenceItem?.position ?? 0) * view.pixelDensity
                    readonly property real endX:
                        startX + (sequenceItem?.length ?? 0) * view.pixelDensity

                    height: parent ? parent.height : 0
                    visible: SequenceSlicerLoader.inRange

                    Rectangle {
                        x: boundary.startX - 0.5
                        width: 1
                        height: parent.height
                        color: Theme.foregroundSecondaryColor
                    }
                    Rectangle {
                        visible: boundary.endX > boundary.startX
                        x: boundary.endX - 0.5
                        width: 1
                        height: parent.height
                        color: Theme.foregroundSecondaryColor
                    }
                }
            }
        }
    }
}
