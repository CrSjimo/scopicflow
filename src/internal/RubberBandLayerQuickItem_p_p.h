#ifndef SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_P_H
#define SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_P_H

#include <QPointer>

#include <ScopicFlowInternal/private/RubberBandLayerQuickItem_p.h>

namespace sflow {
    class RubberBandLayerQuickItemPrivate {
        Q_DECLARE_PUBLIC(RubberBandLayerQuickItem)
    public:
        RubberBandLayerQuickItem *q_ptr;

        QPointer<QQmlComponent> rubberBandComponent;
        QPointer<QQuickItem> rubberBandItem;

        QPointF startPos;
        QPointF endPos;
        QRectF selectionRect;

        bool started = false;
    };
}

#endif //SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_P_H
