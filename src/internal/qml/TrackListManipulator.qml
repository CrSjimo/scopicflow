import QtQml
import QtQuick

Item {
    id: trackListManipulator

    property QtObject animationViewModel: null
    property QtObject trackListLayoutViewModel: null
    property int viewportHeight: 0

    function moveViewBy(deltaY, animated = false) {
        if (!trackListLayoutViewModel)
            return;
        let newViewportOffset = Math.max(0, Math.min(viewportHeight - height, trackListLayoutViewModel.viewportOffset + deltaY));
        if (animated) {
            viewportOffsetBehavior.enabled = false;
            d.viewportOffset = trackListLayoutViewModel.viewportOffset;
            viewportOffsetBehavior.enabled = true;
            d.viewportOffset = newViewportOffset;
        } else {
            trackListLayoutViewModel.viewportOffset = newViewportOffset;
        }
    }

    visible: false

    onHeightChanged: {
        if (!trackListLayoutViewModel)
            return;
        let newViewportOffset = Math.max(0, Math.min(viewportHeight - height, trackListLayoutViewModel.viewportOffset));
        trackListManipulator.trackListLayoutViewModel.viewportOffset = newViewportOffset;
    }

    QtObject {
        id: d

        property double viewportOffset: 0

        Behavior on viewportOffset {
            id: viewportOffsetBehavior

            NumberAnimation {
                duration: (trackListManipulator.animationViewModel?.scrollAnimationRatio ?? 1.0) * 250
                easing.type: Easing.OutCubic
            }
        }

        onViewportOffsetChanged: {
            trackListManipulator.trackListLayoutViewModel.viewportOffset = viewportOffset;
        }
    }
}
