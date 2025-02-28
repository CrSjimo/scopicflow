import QtQml
import QtQuick

QtObject {
    required property bool active

    property color border: active ? "#3498db" : "#222222"
    property color background: "#343a40"

    property color trackSplitter: "#7f000000"
    property color barScale: "#000000"
    property color beatScale: "#bf000000"
    property color segmentScale: "#7f000000"
}