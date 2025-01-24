import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

SequenceSlicerLoader {
    id: slicer
    required property QtObject model
    required property QtObject timeViewModel
    required property QtObject timeLayoutViewModel
    required property double sliceWidth
    property double leftOutBound: 0
    property double rightOutBound: 0
    property int sectionLength: 32768

    handle: model.handle
    range: [
        timeViewModel ? Math.floor((timeViewModel.start - leftOutBound / timeLayoutViewModel.pixelDensity) / sectionLength) * sectionLength : 0,
        timeViewModel ? Math.ceil((timeViewModel.start + sliceWidth / timeLayoutViewModel.pixelDensity + rightOutBound / timeLayoutViewModel.pixelDensity) / sectionLength) * sectionLength : 0
    ]


}
