import QtQml
import QtQuick

Item {
    id: locator

    readonly property list<int> map: column.map
    property double overriddenRowHeight: 0
    readonly property int totalHeight: map.length ? map[map.length - 1] : 0
    property QtObject trackListViewModel: null
    readonly property int viewportHeight: totalHeight + (map.length > 1 ? map[map.length - 1] - map[map.length - 2] : 0)

    function mapToIndex(y) {
        if (!trackListViewModel)
            return 0;
        let trackCount = trackListViewModel.handle.items.length;
        let targetIndex = trackCount - 1;
        for (let i = 0; i < trackCount; i++) {
            if (y >= map[i]) {
                targetIndex = i;
            }
        }
        return targetIndex;
    }

    visible: false

    Connections {
        function onCountChanged() {
            column.map.length = locator.trackListViewModel.handle.count + 1;
        }

        target: locator.trackListViewModel?.handle ?? null
    }
    Column {
        id: column

        property list<int> map: []

        Repeater {
            model: locator.trackListViewModel?.handle.items ?? null

            delegate: Item {
                required property int index
                required property QtObject modelData
                property int pos: y + height

                height: locator.overriddenRowHeight !== 0 ? locator.overriddenRowHeight : modelData.rowHeight
                width: 1

                onPosChanged: column.map[index + 1] = pos
            }
        }
    }
}
