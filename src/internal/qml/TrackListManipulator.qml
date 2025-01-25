import QtQml
import QtQuick

Item {
    id: trackListManipulator
    visible: false

    property QtObject trackListLayoutViewModel: null
    property QtObject animationViewModel: null
    property int viewportHeight: 0

    function moveViewBy(deltaY, animated = false) {
        if (!trackListLayoutViewModel)
            return
        let newViewportOffset = Math.max(0, trackListLayoutViewModel.viewportOffset + deltaY)
        if (newViewportOffset + height > viewportHeight) {
            newViewportOffset = viewportHeight - height
        }
        if (animated) {
            viewportOffsetBehavior.enabled = false
            d.viewportOffset = trackListLayoutViewModel.viewportOffset
            viewportOffsetBehavior.enabled = true
            d.viewportOffset = newViewportOffset
        } else {
            trackListLayoutViewModel.viewportOffset = newViewportOffset
        }
    }

    QtObject {
        id: d
        property double viewportOffset: 0
        onViewportOffsetChanged: {
            trackListManipulator.trackListLayoutViewModel.viewportOffset = viewportOffset
        }
        Behavior on viewportOffset {
            id: viewportOffsetBehavior
            NumberAnimation {
                duration: (trackListManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }
    }

}