#ifndef SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H
#define SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H

#include <ScopicFlow/private/TimelineQuickItem_p.h>

namespace sflow {

    class TimelineQuickItemPrivate {
        Q_DECLARE_PUBLIC(TimelineQuickItem)
    public:
        ~TimelineQuickItemPrivate();
        TimelineQuickItem *q_ptr;

        TimelinePalette *palette = nullptr;
        TimeAlignmentViewModel *timeAlignmentViewModel = nullptr;

        QTextLayout *createTextLayoutForBarNumber(int bar);
        QTextLayout *createTextLayoutForTimeSignature(int numerator, int denominator);

        QHash<int, QTextLayout *> barNumberTextLayouts;
        QHash<qint64, QTextLayout *> timeSignatureTextLayouts;

        double tickToX(int tick) const;
        int xToTick(double x) const;
        int alignTick(int tick) const;
    };
}

#endif //SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H
