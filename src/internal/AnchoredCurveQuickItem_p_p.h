#ifndef SCOPIC_FLOW_ANCHOREDCURVEQUICKITEM_P_P_H
#define SCOPIC_FLOW_ANCHOREDCURVEQUICKITEM_P_P_H

#include <ScopicFlowInternal/private/AnchoredCurveQuickItem_p.h>

#include <SVSCraftCore/AnchoredCurve.h>

namespace sflow {
    class AnchoredCurveQuickItemPrivate {
        Q_DECLARE_PUBLIC(AnchoredCurveQuickItem)
    public:
        AnchoredCurveQuickItem *q_ptr;

        TimeViewModel *timeViewModel{};
        TimeLayoutViewModel *timeLayoutViewModel{};
        PointSequenceViewModel *anchoredCurveViewModel{};
        int topValue{};
        int bottomValue{};
        QList<QColor> strokeColors;
        QColor fillColor;

        SVS::AnchoredCurve curve;
        QHash<QObject *, int> itemPositions;
        QHash<int, QObject *> positionItems;

        double viewPosition = 0;
        double viewLength = 0;
        bool curveDirtyFlag = false;

        void handleItemInserted(QObject *item);
        void handleItemRemoved(QObject *item);
        void handleItemUpdated(QObject *item);
        void handleItemUpdatedSlot();
    };
}

#endif //SCOPIC_FLOW_ANCHOREDCURVEQUICKITEM_P_P_H
