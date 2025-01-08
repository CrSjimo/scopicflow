import QtQml
import QtQuick

Item {
    id: clavierManipulator
    visible: false

    property var clavierViewModel: null
    property var animationViewModel: null
    property double startOffset: 0

    function moveViewBy(deltaY, animated = false) {
        if (!clavierViewModel)
            return
        let newStart = Math.max(0, clavierViewModel.start - deltaY / clavierViewModel.pixelDensity)
        newStart = Math.min(newStart, 128 - (height + startOffset) / clavierViewModel.pixelDensity)
        if (!animated) {
            clavierViewModel.start = newStart
        } else {
            startBehavior.enabled = false
            d.start = clavierViewModel.start
            startBehavior.enabled = true
            d.start = newStart
        }
    }

    function zoomOnWheel(ratio, centerY, animated = false) {
        if (!clavierViewModel)
            return
        let newPixelDensity = Math.min(Math.max(clavierViewModel.minimumPixelDensity, clavierViewModel.pixelDensity * ratio), clavierViewModel.maximumPixelDensity)
        let newStart = Math.max(0, clavierViewModel.start + (height - centerY) / clavierViewModel.pixelDensity - (height - centerY) / newPixelDensity)
        newStart = Math.min(newStart, 128 - height / newPixelDensity)
        if (!animated) {
            clavierViewModel.start = newStart
            clavierViewModel.pixelDensity = newPixelDensity
        } else {
            d.currentAnimationFixStartToZero = ratio < 1 && Math.abs(clavierViewModel.start) < Number.EPSILON
            d.currentAnimationEnsureEnd = ratio < 1 && Math.abs(clavierViewModel.start - (128 - height / clavierViewModel.pixelDensity)) < Number.EPSILON
            pixelDensityBehavior.enabled = false
            d.pixelDensity = clavierViewModel.pixelDensity
            pixelDensityBehavior.enabled = true
            d.centerY = centerY
            d.pixelDensity = newPixelDensity
        }
    }

    QtObject {
        id: d

        property double start: 0
        property double pixelDensity: 0
        property bool currentAnimationFixStartToZero: false
        property bool currentAnimationEnsureEnd: false
        property double centerY: 0

        onStartChanged: {
            clavierManipulator.clavierViewModel.start = start
        }

        onPixelDensityChanged: {
            let newStart = currentAnimationFixStartToZero ? 0 : currentAnimationEnsureEnd ? 128 - clavierManipulator.height / pixelDensity : Math.max(0.0, clavierManipulator.clavierViewModel.start + (clavierManipulator.height - centerY) / clavierManipulator.clavierViewModel.pixelDensity - (clavierManipulator.height - centerY) / pixelDensity)
            newStart = Math.min(newStart, 128 - clavierManipulator.height / pixelDensity)
            clavierManipulator.clavierViewModel.start = newStart
            clavierManipulator.clavierViewModel.pixelDensity = pixelDensity

        }

        Behavior on start {
            id: startBehavior
            NumberAnimation {
                duration: (clavierManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }
        Behavior on pixelDensity {
            id: pixelDensityBehavior
            NumberAnimation {
                duration: (clavierManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }
    }
}