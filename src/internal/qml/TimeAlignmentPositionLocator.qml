import QtQml

QtObject {
    property QtObject timeViewModel: null
    property QtObject timeLayoutViewModel: null

    function mapToTick(x) {
        if (!timeViewModel || !timeLayoutViewModel)
            return 0
        return Math.round(timeViewModel.start + x / timeLayoutViewModel.pixelDensity)
    }
    function mapToX(tick) {
        if (!timeViewModel || !timeLayoutViewModel)
            return 0
        return (tick - timeViewModel.start) * timeLayoutViewModel.pixelDensity
    }
    function alignTick(tick) {
        if (!timeViewModel || !timeLayoutViewModel)
            return tick
        let align = timeLayoutViewModel.positionAlignment
        return Math.floor((tick + align / 2) / align) * align
    }
    function alignTickCeil(tick) {
        if (!timeViewModel || !timeLayoutViewModel)
            return tick
        let align = timeLayoutViewModel.positionAlignment
        return Math.floor((tick + align - 1) / align) * align
    }
    function alignTickFloor(tick) {
        if (!timeViewModel || !timeLayoutViewModel)
            return tick
        let align = timeLayoutViewModel.positionAlignment
        return Math.floor(tick / align) * align
    }
    function alignedX(x) {
        return mapToX(alignTick(mapToTick(x)))
    }

}