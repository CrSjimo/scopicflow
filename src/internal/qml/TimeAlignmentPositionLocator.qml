import QtQml

QtObject {
    property QtObject timeAlignmentViewModel: null

    function mapToTick(x) {
        if (!timeAlignmentViewModel)
            return 0
        return Math.round(timeAlignmentViewModel.start + x / timeAlignmentViewModel.pixelDensity)
    }
    function mapToX(tick) {
        if (!timeAlignmentViewModel)
            return 0
        return (tick - timeAlignmentViewModel.start) * timeAlignmentViewModel.pixelDensity
    }
    function alignTick(tick) {
        if (!timeAlignmentViewModel)
            return tick
        let align = timeAlignmentViewModel.positionAlignment
        return Math.floor((tick + align / 2) / align) * align
    }
    function alignTickCeil(tick) {
        if (!timeAlignmentViewModel)
            return tick
        let align = timeAlignmentViewModel.positionAlignment
        return Math.floor((tick + align - 1) / align) * align
    }
    function alignTickFloor(tick) {
        if (!timeAlignmentViewModel)
            return tick
        let align = timeAlignmentViewModel.positionAlignment
        return Math.floor(tick / align) * align
    }
    function alignedX(x) {
        return mapToX(alignTick(mapToTick(x)))
    }

}