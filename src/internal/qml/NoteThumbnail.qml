import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

Item {
    id: noteThumbnail
    required property QtObject noteSequenceViewModel
    required property int position
    required property int length
    required property color color

    clip: true

    Item {
        id: container
        function findMaxKey(m) {
            for (let i = 127; i >= 0; i--) {
                if (m[i] !== 0)
                    return i
            }
            return 0
        }
        function findMinKey(m) {
            for (let i = 0; i < 128; i++) {
                if (m[i] !== 0)
                    return i
            }
            return 127
        }
        readonly property int maxKey: findMaxKey(keyCount)
        readonly property int minKey: findMinKey(keyCount)
        readonly property double centerKey: (minKey + maxKey) / 2
        readonly property double maxNoteHeight: Math.min(4, noteThumbnail.height / 30)
        readonly property double noteHeight: Math.max(0, noteThumbnail.height / (maxKey - minKey + 1))
        property list<int> keyCount: [0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0]
        SequenceSlicerLoader {
            handle: noteThumbnail.noteSequenceViewModel?.handle ?? null
            range: [noteThumbnail.position, noteThumbnail.position + noteThumbnail.length - 1]
            delegate: Rectangle {
                id: noteRect
                required property QtObject model
                x: model.position - noteThumbnail.position
                y: 127 - model.key
                width: model.length
                height: 1
                color: noteThumbnail.color
                property int currentKey: -1
                Component.onCompleted: () => {
                    currentKey = model.key
                    container.keyCount[currentKey]++
                }
                Component.onDestruction: () => {
                    if (visible)
                        container.keyCount[currentKey]--
                }
                onVisibleChanged: () => {
                    if (!container)
                        return
                    if (!typeof(container.keyCount)) // magic. do not touch this
                        return
                    if (visible)
                        container.keyCount[currentKey]++
                    else
                        container.keyCount[currentKey]--
                }
                Connections {
                    target: model
                    function onKeyChanged() {
                        container.keyCount[noteRect.currentKey]--
                        noteRect.currentKey = noteRect.model.key
                        container.keyCount[noteRect.currentKey]++
                    }
                }
            }
        }
        transform: Scale {
            origin.x: 0
            origin.y: 127 - container.centerKey
            xScale: noteThumbnail.width / noteThumbnail.length
            yScale: Math.min(container.maxNoteHeight, container.noteHeight)
        }
        y: 0.5 * noteThumbnail.height - (127 - container.centerKey)
    }
}