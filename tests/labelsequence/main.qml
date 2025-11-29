import QtQml
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Views

ApplicationWindow {
    id: main
    visible: true
    width: 1024
    height: 64

    required property TimeLayoutViewModel timeLayoutViewModel
    required property TimeViewModel timeViewModel
    required property ScrollBehaviorViewModel scrollBehaviorViewModel
    required property PointSequenceViewModel labelSequenceViewModel
    required property LabelSequenceInteractionController labelSequenceInteractionController
    required property SelectionController selectionController

    Connections {
        target: main.labelSequenceInteractionController
        function onItemDoubleClicked(view, item) {
            view.editInPlace(item)
        }
    }

    LabelSequence {
        id: labelSequence
        width: parent.width
        timeLayoutViewModel: main.timeLayoutViewModel
        timeViewModel: main.timeViewModel
        scrollBehaviorViewModel: main.scrollBehaviorViewModel
        labelSequenceViewModel: main.labelSequenceViewModel
        labelSequenceInteractionController: main.labelSequenceInteractionController
        selectionController: main.selectionController
    }

}