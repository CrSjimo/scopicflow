import QtQml
import QtQuick

Item {
    property QtObject clavierViewModel: null

    function mapToKey(y) {
        if (!clavierViewModel)
            return 0
        return Math.floor(clavierViewModel.start + (height - y) / clavierViewModel.pixelDensity)
    }

    function mapToKeyRound(y) {
        if (!clavierViewModel)
            return 0
        return Math.round(clavierViewModel.start + (height - y) / clavierViewModel.pixelDensity)
    }
}