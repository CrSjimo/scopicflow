import QtQml
import QtQuick

import dev.sjimo.ScopicFlow

Item {
    required property QtObject playbackViewModel
    required property QtObject timeLayoutViewModel
    required property QtObject timeViewModel

    Rectangle {
        id: secondaryIndicator

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: SFPalette.playheadSecondaryColor
        width: 1
        x: parent.timeViewModel && parent.timeLayoutViewModel && parent.playbackViewModel ? (parent.playbackViewModel.secondaryPositon - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
    }
    Rectangle {
        id: primaryIndicator

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: SFPalette.playheadPrimaryColor
        width: 1
        x: parent.timeViewModel && parent.timeLayoutViewModel && parent.playbackViewModel ? (parent.playbackViewModel.primaryPosition - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
    }
    Rectangle {
        id: cursorIndicator

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: SFPalette.cursorIndicatorColor
        width: 1
        x: parent.timeViewModel && parent.timeLayoutViewModel ? (parent.timeLayoutViewModel.cursorPosition - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
    }
}
