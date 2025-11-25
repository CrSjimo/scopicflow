import QtQml
import QtQuick

import dev.sjimo.ScopicFlow

Item {
    property QtObject playbackViewModel: null
    property QtObject timeLayoutViewModel: null
    property QtObject timeViewModel: null

    readonly property Rectangle primaryIndicator: primaryIndicator
    readonly property Rectangle secondaryIndicator: secondaryIndicator
    readonly property Rectangle cursorIndicator: cursorIndicator

    Rectangle {
        id: secondaryIndicator

        anchors.bottom: parent.bottom
        anchors.top: parent.top
        color: SFPalette.playheadSecondaryColor
        width: 1
        x: parent.timeViewModel && parent.timeLayoutViewModel && parent.playbackViewModel ? (parent.playbackViewModel.secondaryPosition - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
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
        visible: parent.timeLayoutViewModel && parent.timeLayoutViewModel.cursorPosition >= 0
        width: 1
        x: parent.timeViewModel && parent.timeLayoutViewModel ? (parent.timeLayoutViewModel.cursorPosition - parent.timeViewModel.start) * parent.timeLayoutViewModel.pixelDensity - 0.5 : 0
    }
}
