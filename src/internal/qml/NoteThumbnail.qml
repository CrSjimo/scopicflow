import QtQml
import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Item {
    id: noteThumbnail

    required property color color
    required property int length
    required property QtObject noteSequenceViewModel
    required property int position

    clip: true

    Item {
        id: container

        readonly property double centerKey: (minKey + maxKey) / 2
        property list<int> keyCount: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
        readonly property int maxKey: findMaxKey(keyCount)
        readonly property double maxNoteHeight: Math.min(4, noteThumbnail.height / 30)
        readonly property int minKey: findMinKey(keyCount)
        readonly property double noteHeight: Math.max(0, noteThumbnail.height / (maxKey - minKey + 1))

        function findMaxKey(m) {
            for (let i = 127; i >= 0; i--) {
                if (m[i] !== 0)
                    return i;
            }
            return 0;
        }
        function findMinKey(m) {
            for (let i = 0; i < 128; i++) {
                if (m[i] !== 0)
                    return i;
            }
            return 127;
        }

        y: 0.5 * noteThumbnail.height - (127 - container.centerKey)

        transform: Scale {
            origin.x: 0
            origin.y: 127 - container.centerKey
            xScale: noteThumbnail.width / noteThumbnail.length
            yScale: Math.min(container.maxNoteHeight, container.noteHeight)
        }

        SequenceSlicerLoader {
            handle: noteThumbnail.noteSequenceViewModel?.handle ?? null
            range: [noteThumbnail.position, noteThumbnail.position + noteThumbnail.length - 1]

            delegate: Rectangle {
                id: noteRect

                property int currentKey: -1
                required property QtObject model

                color: noteThumbnail.color
                height: 1
                width: model.length
                x: model.position - noteThumbnail.position
                y: 127 - model.key

                Component.onCompleted: () => {
                    currentKey = model.key;
                    container.keyCount[currentKey]++;
                }
                Component.onDestruction: () => {
                    if (visible)
                        container.keyCount[currentKey]--;
                }
                onVisibleChanged: () => {
                    if (!container)
                        return;
                    if (!typeof (container.keyCount)) // magic. do not touch this
                        return;
                    if (visible)
                        container.keyCount[currentKey]++;
                    else
                        container.keyCount[currentKey]--;
                }

                Connections {
                    function onKeyChanged() {
                        container.keyCount[noteRect.currentKey]--;
                        noteRect.currentKey = noteRect.model.key;
                        container.keyCount[noteRect.currentKey]++;
                    }

                    target: model
                }
            }
        }
    }
}
