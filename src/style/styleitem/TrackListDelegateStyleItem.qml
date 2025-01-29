import QtQml
import QtQuick
import dev.sjimo.ScopicFlow.Style

QtObject {
    required property QtObject trackViewModel
    required property bool current

    property color background: trackViewModel?.selected ? "#223344" : "#222222"
    property color foreground: Colors.autoForeground(background)
    property color border: "#535c68"

    property color colorIndicator: trackViewModel?.color ?? "transparent"
    property color selectionIndicator: current ? "#3498db" : "transparent"
    property color trackNumber: current ? "#3498db" : foreground

    property color buttonBackground: "#495057"
    property color buttonForeground: Colors.autoForeground(buttonBackground)
    property color buttonBorder: "#636e72"
    property color buttonBackgroundHovered: "#adb5bd"
    property color buttonBackgroundPressed: "#343a40"

    property color mute: "#e67700"
    property color solo: "#2b8a3e"
    property color record: "#c92a2a"
    property color route: "#5f3dc4"

    property color sliderBackground: "#495057"
    property color sliderTrack: "#3498db"
    property color sliderHandle: "#f8f9fa"
    property color sliderHandleHovered: "#f8f9fa"
    property color sliderHandlePressed: "#343a40"

    property color dialBackground: "#495057"
    property color dialForeground: "#ffffff"
    property color dialTrack: "#3498db"
    property color dialBackgroundHovered: "#868e96"
    property color dialBackgroundPressed: "#343a40"

    property color levelBackground: "#111111"
    property color levelLow: "#00c853"
    property color levelMiddle: "#ffab00"
    property color levelHigh: "#ff2c00"

}