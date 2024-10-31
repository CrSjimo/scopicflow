#ifndef SCOPIC_FLOW_TIMELINE_P_H
#define SCOPIC_FLOW_TIMELINE_P_H

#include <ScopicFlow/Timeline.h>

namespace sflow {

    class TimelinePrivate {
        Q_DECLARE_PUBLIC(Timeline)
    public:
        ~TimelinePrivate();
        Timeline *q_ptr;

        TimelinePalette *palette = nullptr;
        TimeViewModel *timeViewModel = nullptr;

        QTextLayout *createTextLayoutForBarNumber(int bar);
        QTextLayout *createTextLayoutForTimeSignature(int numerator, int denominator);

        QHash<int, QTextLayout *> barNumberTextLayouts;
        QHash<qint64, QTextLayout *> timeSignatureTextLayouts;

        double tickToX(int tick) const;
        int xToTick(double x) const;
    };
}

#endif //SCOPIC_FLOW_TIMELINE_P_H
