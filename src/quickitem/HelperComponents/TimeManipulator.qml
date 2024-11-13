import QtQuick

Item {
    id: timeManipulator
    property var timeViewModel: null
    property var animationViewModel: null

    visible: false

    property double _start: 0
    Behavior on _start {
        id: startBehavior
        NumberAnimation {
            duration: (timeManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }
    on_StartChanged: {
        timeViewModel.start = _start
    }
    property double _pixelDensity: 0
    Behavior on _pixelDensity {
        id: pixelDensityBehavior
        NumberAnimation {
            duration: (timeManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }
    property bool _currentAnimationFixStartToZero: false
    property double _centerX: 0
    on_PixelDensityChanged: {
        timeViewModel.start = Math.abs(timeViewModel.start) < Number.EPSILON && _currentAnimationFixStartToZero ? 0.0 : Math.max(0.0, timeViewModel.start + _centerX / timeViewModel.pixelDensity - _centerX / _pixelDensity)
        timeViewModel.pixelDensity = _pixelDensity

    }

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
            _start = timeViewModel.start
            startBehavior.enabled = true
            _start = newStart
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
            _currentAnimationFixStartToZero = ratio < 1 && timeViewModel.start === 0
            pixelDensityBehavior.enabled = false
            _pixelDensity = timeViewModel.pixelDensity
            pixelDensityBehavior.enabled = true
            _centerX = centerX
            _pixelDensity = newPixelDensity
        }
    }
}