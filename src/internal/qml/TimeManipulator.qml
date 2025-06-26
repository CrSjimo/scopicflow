import QtQuick

import dev.sjimo.ScopicFlow.Internal

Item {
    id: timeManipulator

    property QtObject animationViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null

    function ensureVisible(position, length, leftPadding = 0, rightPadding = 0, animated = false) {
        d.ensureVisible(position, length, leftPadding, rightPadding, animated);
    }
    function moveViewBy(deltaX, animated = false) {
        d.moveViewBy(deltaX, animated);
    }
    function zoomOnWheel(ratio, centerX, animated = false) {
        d.zoomViewBy(ratio, centerX, animated);
    }

    visible: false

    SFTimeManipulator {
        id: d

        timeLayoutViewModel: timeManipulator.timeLayoutViewModel
        timeViewModel: timeManipulator.timeViewModel
        viewSize: timeManipulator.width
    }
}
