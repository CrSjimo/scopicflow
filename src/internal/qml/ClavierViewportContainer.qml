import QtQml
import QtQuick

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

FocusScope {
    id: viewportContainer

    property ClavierViewModel clavierViewModel: null

    x: 0
    y: ((clavierViewModel?.start ?? 0) - 128) * (clavierViewModel?.pixelDensity ?? 0)
    width: parent.width
    height: 128 * (clavierViewModel?.pixelDensity ?? 0)
}