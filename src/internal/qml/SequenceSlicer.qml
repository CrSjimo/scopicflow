import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

SequenceSlicerLoader {
    id: slicer

    property double leftOutBound: 0
    required property QtObject model
    property double rightOutBound: 0
    property int sectionLength: 32768
    required property double sliceWidth
    required property QtObject timeLayoutViewModel
    required property QtObject timeViewModel

    handle: model?.handle ?? null
    range: [timeViewModel ? Math.floor((timeViewModel.start - leftOutBound / timeLayoutViewModel.pixelDensity) / sectionLength) * sectionLength : 0, timeViewModel ? Math.ceil((timeViewModel.start + sliceWidth / timeLayoutViewModel.pixelDensity + rightOutBound / timeLayoutViewModel.pixelDensity) / sectionLength) * sectionLength : 0]
}
