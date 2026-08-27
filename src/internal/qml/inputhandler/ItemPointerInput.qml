import QtQml
import QtQuick

import dev.sjimo.ScopicFlow.Internal

PointerInputArea {
    id: itemInput

    required property ScenePointerInput sceneInput
    property Item item: parent
    property QtObject viewModel: null
    property int hoverRegion: 0
    property var payloadResolver: localPoint => undefined
    property var hoverRegionResolver: null
    property var itemHandlerResolver: (event, hit) => sceneInput.resolveHandler(event, hit)
    property var itemCursorResolver: null
    property var itemPressCursorResolver: itemCursorResolver

    anchors.fill: parent
    router: sceneInput.router
    coordinateSpace: sceneInput.coordinateSpace
    handlerResolver: itemHandlerResolver
    cursorResolver: itemCursorResolver
    pressCursorResolver: itemPressCursorResolver
    hitResolver: (surfacePoint, localPoint) => {
        const topLeft = itemInput.item.mapToItem(itemInput.coordinateSpace, 0, 0)
        const payload = itemInput.payloadResolver(localPoint)
        return {
            valid: true,
            target: itemInput.viewModel,
            targetRect: Qt.rect(topLeft.x, topLeft.y,
                                itemInput.item.width, itemInput.item.height),
            hoverRegion: itemInput.hoverRegionResolver
                ? itemInput.hoverRegionResolver(localPoint, payload)
                : itemInput.hoverRegion,
            payload: payload,
        }
    }
}
