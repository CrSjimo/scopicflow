import QtQuick

Item {
    id: timeManipulator
    visible: false

    property var timeViewModel: null
    property var animationViewModel: null

    function moveViewBy(deltaX, animated = false) {
        if (!timeViewModel)
            return
        let newStart = Math.max(0.0, timeViewModel.start + deltaX / timeViewModel.pixelDensity)
        let newEnd = newStart + width / timeViewModel.pixelDensity
        if (newEnd > timeViewModel.end)
            timeViewModel.end = newEnd
        if (!animated) {
            timeViewModel.start = newStart
        } else {
            startBehavior.enabled = false
            d.start = timeViewModel.start
            startBehavior.enabled = true
            d.start = newStart
        }
    }
    function zoomOnWheel(ratio, centerX, animated = false) {
        if (!timeViewModel)
            return
        let newPixelDensity = Math.min(Math.max(timeViewModel.minimumPixelDensity, timeViewModel.pixelDensity * ratio), timeViewModel.maximumPixelDensity)
        let newStart = Math.max(0.0, timeViewModel.start + centerX / timeViewModel.pixelDensity - centerX / newPixelDensity)
        let newEnd = newStart + width / newPixelDensity
        if (newEnd > timeViewModel.end)
            timeViewModel.end = newEnd
        if (!animated) {
            timeViewModel.start = newStart
            timeViewModel.pixelDensity = newPixelDensity
        } else {
            d.currentAnimationFixStartToZero = ratio < 1 && timeViewModel.start === 0
            pixelDensityBehavior.enabled = false
            d.pixelDensity = timeViewModel.pixelDensity
            pixelDensityBehavior.enabled = true
            d.centerX = centerX
            d.pixelDensity = newPixelDensity
        }
    }

    QtObject {
        id: d

        property double start: 0
        property double pixelDensity: 0
        property bool currentAnimationFixStartToZero: false
        property double centerX: 0

        onStartChanged: {
            timeManipulator.timeViewModel.start = start
        }
        onPixelDensityChanged: {
            timeManipulator.timeViewModel.start = Math.abs(timeManipulator.timeViewModel.start) < Number.EPSILON && currentAnimationFixStartToZero ? 0.0 : Math.max(0.0, timeManipulator.timeViewModel.start + centerX / timeManipulator.timeViewModel.pixelDensity - centerX / pixelDensity)
            timeManipulator.timeViewModel.pixelDensity = pixelDensity

        }

        Behavior on start {
            id: startBehavior
            NumberAnimation {
                duration: (timeManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }
        Behavior on pixelDensity {
            id: pixelDensityBehavior
            NumberAnimation {
                duration: (timeManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }
    }
}