pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: clipSequence

    property RangeSequenceViewModel clipSequenceViewModel: null
    property ListViewModel trackListViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null
    property TimeViewModel timeViewModel: null
    property SelectionController selectionController: null

    clip: true
    implicitHeight: 20

    TimeManipulator {
        id: timeManipulator

        target: clipSequence
        timeLayoutViewModel: clipSequence.timeLayoutViewModel
        timeViewModel: clipSequence.timeViewModel
    }
    Rectangle {
        id: background

        anchors.fill: parent
        color: Theme.backgroundColor(clipSequence.ThemedItem.backgroundLevel)
    }
    TimeViewportContainer {
        id: viewportContainer

        timeViewModel: clipSequence.timeViewModel
        timeLayoutViewModel: clipSequence.timeLayoutViewModel

        Item {
            anchors.fill: parent
            SequenceSlicer {
                id: slicer
                leftOutBound: 256
                viewModel: clipSequence.clipSequenceViewModel
                sliceWidth: clipSequence.width
                timeLayoutViewModel: clipSequence.timeLayoutViewModel
                timeViewModel: clipSequence.timeViewModel

                delegate: Rectangle {
                    id: clipSequenceDelegate
                    readonly property ClipViewModel clipViewModel: SequenceSlicerLoader.viewModel
                    property double headerMargin: 0
                    property string name: ""
                    Binding {
                        clipSequenceDelegate.x: (clipSequenceDelegate.clipViewModel?.position ?? 0) * (clipSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipSequenceDelegate.width: (clipSequenceDelegate.clipViewModel?.length ?? 0) * (clipSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipSequenceDelegate.headerMargin: ((clipSequence.timeViewModel?.start ?? 0) - (clipSequenceDelegate.clipViewModel?.position ?? 0)) * (clipSequence.timeLayoutViewModel?.pixelDensity ?? 0)
                        clipSequenceDelegate.color: clipSequenceDelegate.clipViewModel?.editing ? (clipSequence.trackListViewModel?.items[clipSequenceDelegate.clipViewModel?.trackIndex ?? 0]?.color ?? Qt.rgba(0, 0, 0, 0)) : Theme.backgroundTertiaryColor
                        clipSequenceDelegate.z: clipSequenceDelegate.clipViewModel?.editing ? 1 : 0
                        clipSequenceDelegate.name: clipSequenceDelegate.clipViewModel?.name ?? ""
                        when: clipSequenceDelegate.SequenceSlicerLoader.inRange
                    }
                    border.color: Theme.borderColor
                    height: clipSequence.height
                    RowLayout {
                        spacing: 4
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: Math.max(2, Math.min(clipSequenceDelegate.headerMargin, parent.width - width))
                        width: Math.min(implicitWidth, clipSequenceDelegate.width)
                        Text {
                            Layout.alignment: Qt.AlignVCenter
                            Layout.fillWidth: true
                            text: clipSequenceDelegate.name
                            font: Theme.font
                            color: Theme.foregroundPrimaryColor
                            elide: Text.ElideRight
                            leftPadding: 4
                            rightPadding: 4
                        }
                    }
                }
            }
        }
    }
    StandardScrollHandler {
        movableOrientation: Qt.Horizontal
        viewModel: clipSequence.scrollBehaviorViewModel

        onMoved: (x, _, isPhysicalWheel) => timeManipulator.moveViewBy(x, isPhysicalWheel)
        onZoomed: (ratioX, _, x, _, isPhysicalWheel) => timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel)
    }
}
