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

        double viewPosition = 0;
        double viewLength = 0;
        int dirtyPosition = 0;
        int dirtyLength = -1;

        void handleItemInserted(QObject *item, bool updateDirty = true);
        void handleItemRemoved(QObject *item);
        void calculateDirty(QObject *item);
    };
}

#endif //SCOPIC_FLOW_ANCHOREDCURVEQUICKITEM_P_P_H
