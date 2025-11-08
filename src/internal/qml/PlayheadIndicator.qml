import QtQml
import QtQuick
import QtQuick.Shapes

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: indicator
    LayoutMirroring.enabled: false
    LayoutMirroring.childrenInherit: true
    implicitHeight: 14.333333333333334 // 43 / 3
    implicitWidth: 18.475208614068027 // 32 / sqrt(3)
    property color color

    Shape {
        id: shape
        width: parent.width
        height: parent.height
        anchors.horizontalCenter: parent.left
        ShapePath {
            id: indicatorPath

            fillColor: indicator.color
            strokeWidth: 0

            PathLine {
                x: shape.width * 0.25
                y: 0
            }
            PathLine {
                x: shape.width * 0.75
                y: 0
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: shape.width * 0.875
                y: 4
            }
            PathLine {
                x: shape.width * 0.625
                y: 12
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: shape.width * 0.375
                y: 12
            }
            PathLine {
                x: shape.width * 0.125
                y: 4
            }
            PathArc {
                radiusX: 4 / 3
                radiusY: 4 / 3
                x: shape.width * 0.25
                y: 0
            }
        }
    }

}