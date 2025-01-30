import QtQml
import QtQuick

Item {
    property bool running: false
    property double distanceX: 0
    property double distanceY: 0

    signal moved(deltaX: double, deltaY: double)

    function determine(x, width, y, height, callback) {
        let xTriggered = true
        if (x < 0)
            distanceX = x
        else if (x > width)
            distanceX = x - width
        else {
            distanceX = 0
            xTriggered = false
        }
        let yTriggered = true
        if (y < 0)
            distanceY = y
        else if (y > height)
            distanceY = y - height
        else {
            distanceY = 0
            yTriggered = false
        }
        running = xTriggered || yTriggered
        if (callback)
            callback(xTriggered, yTriggered)
    }

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