import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: pianoRollScrollLayer

    property ClavierViewModel clavierViewModel: null
    property ScrollBehaviorViewModel scrollBehaviorViewModel: null
    property TimeViewModel timeViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null

    property double bottomExpansion: 0

    ClavierManipulator {
        id: clavierManipulator
        clavierViewModel: pianoRollScrollLayer.clavierViewModel
        target: pianoRollScrollLayer
        viewSize: pianoRollScrollLayer.height - pianoRollScrollLayer.bottomExpansion
    }

    TimeManipulator {
        id: timeManipulator
        target: pianoRollScrollLayer
        timeLayoutViewModel: pianoRollScrollLayer.timeLayoutViewModel
        timeViewModel: pianoRollScrollLayer.timeViewModel
    }

    ClavierViewportContainer {
        id: clavierViewportContainer
        clavierViewModel: pianoRollScrollLayer.clavierViewModel
    }

    TimeViewportContainer {
        id: timeViewportContainer
        timeViewModel: pianoRollScrollLayer.timeViewModel
        timeLayoutViewModel: pianoRollScrollLayer.timeLayoutViewModel
    }

    StandardScrollHandler {
        viewModel: pianoRollScrollLayer.scrollBehaviorViewModel
        horizontalScrollBarEnabled: true
        horizontalScrollX: -timeViewportContainer.x
        horizontalScrollWidth: timeViewportContainer.width
        verticalScrollBarEnabled: true
        verticalScrollY: -clavierViewportContainer.y
        verticalScrollHeight: clavierViewportContainer.height

        onMoved: (deltaX, deltaY, isPhysicalWheel) => {
            timeManipulator.moveViewBy(deltaX, isPhysicalWheel);
            clavierManipulator.moveViewBy(deltaY, isPhysicalWheel);
        }

        onZoomed: (ratioX, ratioY, x, y, isPhysicalWheel) => {
            timeManipulator.zoomViewBy(ratioX, x, isPhysicalWheel);
            clavierManipulator.zoomViewBy(ratioY, y, isPhysicalWheel);
        }
    }
}