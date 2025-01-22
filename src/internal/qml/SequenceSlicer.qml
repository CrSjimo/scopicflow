import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

SequenceSlicerLoader {
    id: slicer
    property QtObject model: null
    property QtObject timeViewModel: null
    property double leftOutBound: 0
    property double rightOutBound: 0
    property int sectionLength: 32768

    handle: model.handle
    range: [
        timeViewModel ? Math.floor((timeViewModel.start - leftOutBound / timeViewModel.pixelDensity) / sectionLength) * sectionLength : 0,
        timeViewModel ? Math.ceil((timeViewModel.end + rightOutBound / timeViewModel.pixelDensity) / sectionLength) * sectionLength : 0
    ]


}
