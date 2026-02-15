import QtQml
import QtQuick
import QtQuick.Controls

import SVSCraft
import SVSCraft.UIComponents

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

TimeViewModel {
    id: proxyTimeViewModel

    property ClipViewModel clipViewModel: null
    property TimeViewModel timeViewModel: null

    readonly property double proxyTimeStartPosition: (timeViewModel?.start ?? 0) - (clipViewModel?.position ?? 0) + (clipViewModel?.clipStart ?? 0)
    readonly property double proxyTimeEndPosition: (timeViewModel?.end ?? 0) - (clipViewModel?.position ?? 0) + (clipViewModel?.clipStart ?? 0)

    timeline: timeViewModel?.timeline ?? null

    onProxyTimeStartPositionChanged: () => {
        start = proxyTimeStartPosition
    }

    onProxyTimeEndPositionChanged: () => {
        end = proxyTimeEndPosition
    }

    onStartChanged: () => {
        if (proxyTimeStartPosition !== start) {
            timeViewModel.start = start + (clipViewModel?.position ?? 0) - (clipViewModel?.clipStart ?? 0)
        }
    }

    onEndChanged: () => {
        if (proxyTimeEndPosition !== end) {
            timeViewModel.end = end + (clipViewModel?.position ?? 0) - (clipViewModel?.clipStart ?? 0)
        }
    }
}