import "qrc:/ScopicFlow/private"
import QtQml
import QtQuick

Item {
    property QtObject timeAlignmentViewModel: null
    property QtObject playbackViewModel: null
    property QtObject scrollBehaviorViewModel: null
    property QtObject animationViewModel: null
    property QtObject paletteViewModel: null
    function mapToTick(x) {

    }
    function mapToX(tick) {

    }
    signal positionIndicatorDoubleClicked()
    signal contextMenuRequestedForTimeline(tick: int)
    signal contextMenuRequestedForPositionIndicator()

    Timeline {
        anchors.fill: parent
    }
}