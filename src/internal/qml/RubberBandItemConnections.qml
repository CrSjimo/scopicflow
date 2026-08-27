import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow

QtObject {
    required property Item target
    required property var viewModel
    required property RubberBandSelector selector
    property double x: target.x
    property double y: target.y
    property double width: target.width
    property double height: target.height
    property bool visible: target.visible
    function handleRubberBand() {
        if (visible)
            selector.insertItem(viewModel, Qt.rect(x, y, width, height));
        else
            selector.removeItem(viewModel);
    }
    Component.onDestruction: selector.removeItem(viewModel)
    onVisibleChanged: handleRubberBand()
    onXChanged: handleRubberBand()
    onYChanged: handleRubberBand()
    onWidthChanged: handleRubberBand()
    onHeightChanged: handleRubberBand()
}
