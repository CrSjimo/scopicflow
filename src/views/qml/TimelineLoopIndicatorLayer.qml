pragma ComponentBehavior: Bound

import QtQml
import QtQuick
import QtQuick.Shapes

import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal as ScopicFlowInternal

Item {
    id: loopIndicatorLayer

    required property TimeViewModel timeViewModel
    required property TimeLayoutViewModel timeLayoutViewModel
    required property PlaybackViewModel playbackViewModel
    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property TimelineInteractionController timelineInteractionController

    TimeManipulator {
        id: loopTimeManipulator
        target: loopIndicatorLayer
        timeViewModel: loopIndicatorLayer.timeViewModel
        timeLayoutViewModel: loopIndicatorLayer.timeLayoutViewModel
    }

    component LoopMouseArea: MouseArea {
        id: loopMouseArea
        acceptedButtons: ((loopIndicatorLayer.scrollBehaviorViewModel?.scrollTypes ?? 0) & ScrollBehaviorViewModel.LeftButton)
                         ? Qt.NoButton : Qt.LeftButton
        anchors.fill: parent
        visible: Boolean((loopIndicatorLayer.timelineInteractionController?.interaction ?? 0) & TimelineInteractionController.AdjustLoopRange)
        hoverEnabled: true
        cursorShape: Qt.SizeHorCursor
        CursorBinding.cursorShape: cursorShape

        property bool dragged: false
        property double pressedDeltaX: 0
        property double pressedDeltaXBias: 0
        required property int adjustmentOperation

        signal positionUpdated(position: int)

        function moveToX(x) {
            positionUpdated(loopTimeManipulator.alignPosition(loopTimeManipulator.mapToPosition(x)))
        }

        onPressed: (mouse) => {
            CursorBinding.enabled = true
            dragged = false
            pressedDeltaX = mouse.x + pressedDeltaXBias;
        }

        onPositionChanged: (mouse) => {
            if (!pressed)
                return
            if (!dragged) {
                dragged = true
                loopIndicatorLayer.timelineInteractionController?.loopRangeAdjustingStarted(loopIndicatorLayer, adjustmentOperation)
            }
            const layerX = mapToItem(loopIndicatorLayer, mouse.x, 0).x
            loopDragScroller.determine(layerX, loopIndicatorLayer.width, 0, 0, triggered => {
                if (triggered)
                    return;
                moveToX(layerX - pressedDeltaX);
            });
        }

        onCanceled: () => {
            CursorBinding.enabled = false
            loopDragScroller.running = false
            loopIndicatorLayer.timelineInteractionController?.loopRangeAdjustingFinished(loopIndicatorLayer, adjustmentOperation)
        }

        onReleased: canceled()

        onEntered: () => {
            loopIndicatorLayer.timelineInteractionController?.loopHoverEntered(loopIndicatorLayer, adjustmentOperation)
        }

        onExited: () => {
            loopIndicatorLayer.timelineInteractionController?.loopHoverExited(loopIndicatorLayer, adjustmentOperation)
        }

        ScopicFlowInternal.DragScroller {
            id: loopDragScroller
            onMoved: deltaX => {
                loopTimeManipulator.moveViewBy(deltaX);
                loopMouseArea.moveToX((deltaX < 0 ? 0 : loopIndicatorLayer.width) - loopMouseArea.pressedDeltaX);
            }
        }

    }

    ScopicFlowInternal.TimeViewportContainer {
        timeViewModel: loopIndicatorLayer.timeViewModel
        timeLayoutViewModel: loopIndicatorLayer.timeLayoutViewModel

        Rectangle {
            id: loopRangeRect
            anchors.top: parent.top
            visible: (loopIndicatorLayer.playbackViewModel?.loopLength ?? -1) >= 0
            x: (loopIndicatorLayer.playbackViewModel?.loopStart ?? 0) * (loopIndicatorLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            width: (loopIndicatorLayer.playbackViewModel?.loopLength ?? 0) * (loopIndicatorLayer.timeLayoutViewModel?.pixelDensity ?? 0)
            height: 4
            color: SFPalette.loopColor

            LoopMouseArea {
                adjustmentOperation: TimelineInteractionController.AdjustRange
                onPositionUpdated: (position) => {
                    loopIndicatorLayer.playbackViewModel.loopStart = position;
                }
            }
        }

        Item {
            id: loopStartIndicator
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: (loopIndicatorLayer.playbackViewModel?.loopLength ?? -1) >= 0
            x: (loopIndicatorLayer.playbackViewModel?.loopStart ?? 0) * (loopIndicatorLayer.timeLayoutViewModel?.pixelDensity ?? 0) - 2
            width: 4
            Rectangle {
                x: 1
                width: 1
                height: 12
                color: SFPalette.loopColor
            }
            Shape {
                anchors.left: parent.horizontalCenter
                anchors.leftMargin: -0.5
                width: 12
                height: 12
                ShapePath {
                    strokeWidth: 1
                    strokeColor: SFPalette.loopColor
                    fillColor: Qt.rgba(SFPalette.loopColor.r, SFPalette.loopColor.g, SFPalette.loopColor.b, 0.5 * SFPalette.loopColor.a)
                    PathLine {
                        x: 12
                        y: 0
                    }
                    PathLine {
                        x: 0
                        y: 12
                    }
                }
            }
            LoopMouseArea {
                adjustmentOperation: TimelineInteractionController.AdjustStart
                anchors.bottomMargin: {
                    if (!loopIndicatorLayer.playbackViewModel)
                        return 0
                    let a = loopIndicatorLayer.playbackViewModel.loopStart
                    let b = loopIndicatorLayer.playbackViewModel.primaryPosition
                    let c = loopIndicatorLayer.playbackViewModel.secondaryPosition
                    return Math.min(Math.abs(a - b), Math.abs(a - c)) <= 8 / (loopIndicatorLayer.timeLayoutViewModel?.pixelDensity ?? 1) ? 18.333333333333334 : 0
                }
                onPositionUpdated: (position) => {
                    let delta = position - loopIndicatorLayer.playbackViewModel.loopStart
                    if (loopIndicatorLayer.playbackViewModel.loopLength - delta <= 0) {
                        return
                    }
                    loopIndicatorLayer.playbackViewModel.loopStart += delta
                    loopIndicatorLayer.playbackViewModel.loopLength -= delta
                }
            }
        }

        Item {
            id: loopEndIndicator
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            visible: (loopIndicatorLayer.playbackViewModel?.loopLength ?? -1) >= 0
            x: ((loopIndicatorLayer.playbackViewModel?.loopStart ?? 0) + (loopIndicatorLayer.playbackViewModel?.loopLength ?? 0)) * (loopIndicatorLayer.timeLayoutViewModel?.pixelDensity ?? 0) - 2
            width: 4
            Rectangle {
                x: 1
                width: 1
                height: 12
                color: SFPalette.loopColor
            }
            Shape {
                anchors.right: parent.horizontalCenter
                anchors.rightMargin: 0.5
                width: 12
                height: 12
                ShapePath {
                    strokeWidth: 1
                    strokeColor: SFPalette.loopColor
                    fillColor: Qt.rgba(SFPalette.loopColor.r, SFPalette.loopColor.g, SFPalette.loopColor.b, 0.5 * SFPalette.loopColor.a)
                    startX: 12
                    startY: 0
                    PathLine {
                        x: 0
                        y: 0
                    }
                    PathLine {
                        x: 12
                        y: 12
                    }
                }
            }
            LoopMouseArea {
                adjustmentOperation: TimelineInteractionController.AdjustEnd
                anchors.bottomMargin: {
                    if (!loopIndicatorLayer.playbackViewModel)
                        return 0
                    let a = loopIndicatorLayer.playbackViewModel.loopStart + loopIndicatorLayer.playbackViewModel.loopLength
                    let b = loopIndicatorLayer.playbackViewModel.primaryPosition
                    let c = loopIndicatorLayer.playbackViewModel.secondaryPosition
                    return Math.min(Math.abs(a - b), Math.abs(a - c)) <= 8 / (loopIndicatorLayer.timeLayoutViewModel?.pixelDensity ?? 1) ? 18.333333333333334 : 0
                }
                onPositionUpdated: (position) => {
                    let delta = position - (loopIndicatorLayer.playbackViewModel.loopStart + loopIndicatorLayer.playbackViewModel.loopLength)
                    if (loopIndicatorLayer.playbackViewModel.loopLength + delta <= 0) {
                        return
                    }
                    loopIndicatorLayer.playbackViewModel.loopLength += delta
                }
            }
        }
    }
}
