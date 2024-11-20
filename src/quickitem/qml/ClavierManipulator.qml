import QtQuick

Item {
    id: clavierManipulator
    property var clavierViewModel: null
    property var animationViewModel: null

    visible: false

    property double _start: 0
    Behavior on _start {
        id: startBehavior
        NumberAnimation {
            duration: (clavierManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }
    on_StartChanged: {
        clavierViewModel.start = _start
    }
    property double _pixelDensity: 0
    Behavior on _pixelDensity {
        id: pixelDensityBehavior
        NumberAnimation {
            duration: (clavierManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
            easing.type: Easing.OutCubic
        }
    }
    property bool _currentAnimationFixStartToZero: false
    property bool _currentAnimationEnsureEnd: false
    property double _centerY: 0
    on_PixelDensityChanged: {
        let newStart = _currentAnimationFixStartToZero ? 0.0 : _currentAnimationEnsureEnd ? 128 - height / _pixelDensity : Math.max(0.0, clavierViewModel.start + (height - _centerY) / clavierViewModel.pixelDensity - (height - _centerY) / _pixelDensity)
        newStart = Math.min(newStart, 128 - height / _pixelDensity)
        clavierViewModel.start = newStart
        clavierViewModel.pixelDensity = _pixelDensity

    }

    function moveViewBy(deltaY, animated = false) {
        if (!clavierViewModel)
            return
        let newStart = Math.max(0.0, clavierViewModel.start - deltaY / clavierViewModel.pixelDensity)
        newStart = Math.min(newStart, 128 - height / clavierViewModel.pixelDensity)
        if (!animated) {
            clavierViewModel.start = newStart
        } else {
            startBehavior.enabled = false
            _start = clavierViewModel.start
            startBehavior.enabled = true
            _start = newStart
        }
    }

    function zoomOnWheel(ratio, centerY, animated = false) {
        if (!clavierViewModel)
            return
        let newPixelDensity = Math.min(Math.max(clavierViewModel.minimumPixelDensity, clavierViewModel.pixelDensity * ratio), clavierViewModel.maximumPixelDensity)
        let newStart = Math.max(0.0, clavierViewModel.start + (height - centerY) / clavierViewModel.pixelDensity - (height - centerY) / newPixelDensity)
        newStart = Math.min(newStart, 128 - height / newPixelDensity)
        if (!animated) {
            clavierViewModel.start = newStart
            clavierViewModel.pixelDensity = newPixelDensity
        } else {
            _currentAnimationFixStartToZero = ratio < 1 && Math.abs(clavierViewModel.start) < Number.EPSILON
            _currentAnimationEnsureEnd = ratio < 1 && Math.abs(clavierViewModel.start - (128 - height / clavierViewModel.pixelDensity)) < Number.EPSILON
            pixelDensityBehavior.enabled = false
            _pixelDensity = clavierViewModel.pixelDensity
            pixelDensityBehavior.enabled = true
            _centerY = centerY
            _pixelDensity = newPixelDensity
        }
    }
}