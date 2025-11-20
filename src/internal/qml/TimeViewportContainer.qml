import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: viewportContainer

    property TimeViewModel timeViewModel: null
    property TimeLayoutViewModel timeLayoutViewModel: null

    x: -(timeViewModel?.start ?? 0) * (timeLayoutViewModel?.pixelDensity ?? 0)
    y: 0
    width: (timeViewModel?.end ?? 0) * (timeLayoutViewModel?.pixelDensity ?? 0)
    height: parent.height
}