import QtQml
import QtQuick

Item {

    property bool running: false
    property double distanceX: 0
    property double distanceY: 0

    signal moved(deltaX: double, deltaY: double)

    Timer {
        id: tickingTimer
        interval: 10
        repeat: true
        running: parent.running
        onTriggered: {
            parent.moved(Math.max(-1, Math.min(distanceX / 256, 1)) * interval, Math.max(-1, Math.min(distanceY / 256, 1)) * interval)
        }
    }
}