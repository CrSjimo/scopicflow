import QtQml
import QtQuick

Item {
    id: locator
    visible: false
    property QtObject trackListViewModel: null

    readonly property list<int> map: column.map
    readonly property int totalHeight: map.length ? map[map.length - 1] : 0
    readonly property int viewportHeight: totalHeight + (map.length > 1 ? map[map.length - 1] - map[map.length - 2] : 0)

    Connections {
        target: locator.trackListViewModel?.handle ?? null
        function onCountChanged() {
            column.map.length = locator.trackListViewModel.handle.count + 1
        }
    }

    Column {
        id: column
        property list<int> map: []
        Repeater {
            model: locator.trackListViewModel?.handle.items ?? null
            delegate: Item {
                required property int index
                required property QtObject modelData
                width: 1
                height: modelData.rowHeight
                property int pos: y + height
                onPosChanged: column.map[index + 1] = pos
            }
        }
    }

}