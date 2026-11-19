import QtQml
import QtQuick

import SVSCraft as SVS
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal
import dev.sjimo.ScopicFlow

ScopicFlowInternal.PositionIndicators {
    id: playheadLayer

    enum TimecodeFormat {
        MusicTime,
        AbsoluteTime,
        MusicAndAbsoluteTime
    }

    required timeViewModel
    required timeLayoutViewModel
    required playbackViewModel
    property int timecodeFormat: TimelinePlayheadLayer.MusicTime

    primaryIndicator.visible: false
    secondaryIndicator.visible: false
    cursorIndicator.anchors.topMargin: 16

    ScopicFlowInternal.PlayheadIndicator {
        anchors.bottom: parent.bottom
        x: playheadLayer.primaryIndicator.x
        color: SFPalette.playheadPrimaryColor
    }

    ScopicFlowInternal.PlayheadIndicator {
        anchors.bottom: parent.bottom
        x: playheadLayer.secondaryIndicator.x
        color: SFPalette.playheadSecondaryColor
    }

    Text {
        id: cursorLabel

        readonly property SVS.MusicTimeline musicTimeline: playheadLayer.timeViewModel?.timeline ?? null
        readonly property int cursorPosition: playheadLayer.timeLayoutViewModel?.cursorPosition ?? -1
        property int timelineRevision: 0

        function formatAbsoluteTime(milliseconds: int): string {
            return new SVS.longTime(milliseconds).toString()
        }

        anchors.top: parent.top
        anchors.topMargin: 2
        x: playheadLayer.cursorIndicator.x - implicitWidth / 2
        text: {
            if (!musicTimeline || cursorPosition < 0)
                return ""

            timelineRevision
            const cursorTime = musicTimeline.create(0, 0, cursorPosition)
            if (playheadLayer.timecodeFormat === TimelinePlayheadLayer.MusicTime)
                return cursorTime.toString(1, 1, 3)

            const absoluteTimeText = formatAbsoluteTime(Math.trunc(cursorTime.millisecond))
            if (playheadLayer.timecodeFormat === TimelinePlayheadLayer.AbsoluteTime)
                return absoluteTimeText

            return cursorTime.toString(1, 1, 3) + " " + absoluteTimeText
        }
        font.family: Theme.font.family
        font.pixelSize: 12
        color: SFPalette.cursorIndicatorColor

        Connections {
            target: cursorLabel.musicTimeline
            function onChanged() {
                cursorLabel.timelineRevision += 1
            }
        }
    }
}
