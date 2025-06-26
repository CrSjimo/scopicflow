import QtQml
import QtQuick

Item {
    id: clavierManipulator

    property var animationViewModel: null
    property var clavierViewModel: null
    property double startOffset: 0

    function moveViewBy(deltaY, animated = false) {
        if (!clavierViewModel)
            return;
        let newStart = Math.max(0, clavierViewModel.start - deltaY / clavierViewModel.pixelDensity);
        newStart = Math.min(newStart, 128 - (height + startOffset) / clavierViewModel.pixelDensity);
        if (!animated) {
            clavierViewModel.start = newStart;
        } else {
            startBehavior.enabled = false;
            d.start = clavierViewModel.start;
            startBehavior.enabled = true;
            d.start = newStart;
        }
    }
    function zoomOnWheel(ratio, centerY, animated = false) {
        if (!clavierViewModel)
            return;
        let newPixelDensity = Math.min(Math.max(clavierViewModel.minimumPixelDensity, clavierViewModel.pixelDensity * ratio), clavierViewModel.maximumPixelDensity);
        let newStart = Math.max(0, clavierViewModel.start + (height - centerY + startOffset) / clavierViewModel.pixelDensity - (height - centerY + startOffset) / newPixelDensity);
        newStart = Math.min(newStart, 128 - (height + startOffset) / newPixelDensity);
        if (!animated) {
            clavierViewModel.start = newStart;
            clavierViewModel.pixelDensity = newPixelDensity;
        } else {
            d.currentAnimationFixStartToZero = ratio < 1 && Math.abs(clavierViewModel.start) < Number.EPSILON;
            d.currentAnimationEnsureEnd = ratio < 1 && Math.abs(clavierViewModel.start - (128 - (height + startOffset) / clavierViewModel.pixelDensity)) < Number.EPSILON;
            pixelDensityBehavior.enabled = false;
            d.pixelDensity = clavierViewModel.pixelDensity;
            pixelDensityBehavior.enabled = true;
            d.centerY = centerY;
            d.pixelDensity = newPixelDensity;
        }
    }

    visible: false

    QtObject {
        id: d

        property double centerY: 0
        property bool currentAnimationEnsureEnd: false
        property bool currentAnimationFixStartToZero: false
        property double pixelDensity: 0
        property double start: 0

        Behavior on pixelDensity {
            id: pixelDensityBehavior

            NumberAnimation {
                duration: (clavierManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }
        Behavior on start {
            id: startBehavior

            NumberAnimation {
                duration: (clavierManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }

        onPixelDensityChanged: {
            let newStart = currentAnimationFixStartToZero ? 0 : currentAnimationEnsureEnd ? 128 - (clavierManipulator.height + clavierManipulator.startOffset) / pixelDensity : Math.max(0.0, clavierManipulator.clavierViewModel.start + (clavierManipulator.height - centerY + clavierManipulator.startOffset) / clavierManipulator.clavierViewModel.pixelDensity - (clavierManipulator.height - centerY + clavierManipulator.startOffset) / pixelDensity);
            newStart = Math.min(newStart, 128 - (clavierManipulator.height + clavierManipulator.startOffset) / pixelDensity);
            clavierManipulator.clavierViewModel.start = newStart;
            clavierManipulator.clavierViewModel.pixelDensity = pixelDensity;
        }
        onStartChanged: {
            clavierManipulator.clavierViewModel.start = start;
        }
    }
}
