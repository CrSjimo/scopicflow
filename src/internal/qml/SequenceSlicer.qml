import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

SequenceSlicerLoader {
    id: slicer

    property double leftOutBound: 0
    property double rightOutBound: 0
    property int sectionLength: 12000
    required property double sliceWidth
    required property QtObject timeLayoutViewModel
    required property QtObject timeViewModel

    range: [timeViewModel ? Math.floor((timeViewModel.start - leftOutBound / timeLayoutViewModel.pixelDensity) / sectionLength) * sectionLength : 0, timeViewModel ? Math.ceil((timeViewModel.start + sliceWidth / timeLayoutViewModel.pixelDensity + rightOutBound / timeLayoutViewModel.pixelDensity) / sectionLength) * sectionLength : 0]

    function temporarilyLoadForRubberBand(x, width) {
        temporarilyLoad([(x - leftOutBound) / timeLayoutViewModel.pixelDensity, width / timeLayoutViewModel.pixelDensity])
    }
}
