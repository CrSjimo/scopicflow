import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

Item {
    anchors.fill: parent

    readonly property bool dragged: _dragged
    readonly property point startPoint: _startPoint
    readonly property int modifiers: _modifiers
    property bool _dragged: false
    property point _startPoint: Qt.point(0, 0)
    property int _modifiers: 0

    signal dragStarted(x: double, y: double, modifiers: int)
    signal dragMoved(x: double, y: double)
    signal dragFinished()
    signal dragCanceled()
}