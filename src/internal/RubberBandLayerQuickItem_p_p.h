#ifndef SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_P_H
#define SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_P_H

#include <ScopicFlowInternal/private/RubberBandLayerQuickItem_p.h>

namespace sflow {
    class RubberBandLayerQuickItemPrivate {
        Q_DECLARE_PUBLIC(RubberBandLayerQuickItem)
    public:
        RubberBandLayerQuickItem *q_ptr;

        SelectableViewModelManipulator *selectionManipulator = nullptr;
        QQmlComponent *rubberBandComponent = nullptr;
        QQuickItem *rubberBandItem = nullptr;

        QPointF startPos;
        QPointF endPos;

        bool started = false;

        QHash<qsizetype, QRectF> itemRects;
        QSet<qsizetype> taggedItems;
    };
}

#endif //SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_P_H
