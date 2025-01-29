import QtQml
import QtQuick

QtObject {
    required property bool active

    property color background: "#222222"
    property color border: active ? "#3498db" : "transparent"
    property color trackHandle: "#3498db"
}