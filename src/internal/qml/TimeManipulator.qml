import QtQuick

import dev.sjimo.ScopicFlow.Internal

Item {
    id: timeManipulator
    visible: false

    SFTimeManipulator {
        id: d
        timeViewModel: timeManipulator.timeViewModel
        timeLayoutViewModel: timeManipulator.timeLayoutViewModel
        viewSize: timeManipulator.width
    }

    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject animationViewModel: null

    function moveViewBy(deltaX, animated = false) {
        d.moveViewBy(deltaX, animated)
    }
    function zoomOnWheel(ratio, centerX, animated = false) {
        d.zoomViewBy(ratio, centerX, animated)
    }

    function ensureVisible(position, length, leftPadding = 0, rightPadding = 0, animated = false) {
        d.ensureVisible(posiiton, length, leftPadding, rightPadding, animated)
    }

}