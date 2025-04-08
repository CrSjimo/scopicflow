import QtQml
import QtQuick

Item {
    id: locator
    visible: false
    property QtObject trackListViewModel: null
    property double overriddenRowHeight: 0

    readonly property list<int> map: column.map
    readonly property int totalHeight: map.length ? map[map.length - 1] : 0
    readonly property int viewportHeight: totalHeight + (map.length > 1 ? map[map.length - 1] - map[map.length - 2] : 0)

    function mapToIndex(y) {
        if (!trackListViewModel)
            return 0
        let trackCount = trackListViewModel.handle.items.length
        let targetIndex = trackCount - 1
        for (let i = 0; i < trackCount; i++) {
            if (y >= map[i]) {
                targetIndex = i
            }
        }
        return targetIndex
    }

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
                height: locator.overriddenRowHeight !== 0 ? locator.overriddenRowHeight : modelData.rowHeight
                property int pos: y + height
                onPosChanged: column.map[index + 1] = pos
            }
        }
    }

}