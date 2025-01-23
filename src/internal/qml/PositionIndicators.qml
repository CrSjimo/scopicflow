import QtQml
import QtQuick

Item {
    required property QtObject palette
    required property QtObject timeViewModel
    required property QtObject timeLayoutViewModel
    required property QtObject playbackViewModel

    Rectangle {
        id: secondaryIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: parent.palette.secondaryIndicatorColor
        x: parent.timeViewModel && parent.timeLayoutViewModel && parent.playbackViewModel ? (parent.playbackViewModel.secondaryPositon - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
    }
    Rectangle {
        id: primaryIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: parent.palette.primaryIndicatorColor
        x: parent.timeViewModel && parent.timeLayoutViewModel && parent.playbackViewModel ? (parent.playbackViewModel.primaryPosition - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
    }
    Rectangle {
        id: cursorIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: 1
        color: parent.palette.cursorIndicatorColor
        x: parent.timeViewModel && parent.timeLayoutViewModel ? (parent.timeLayoutViewModel.cursorPosition - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
    }

}