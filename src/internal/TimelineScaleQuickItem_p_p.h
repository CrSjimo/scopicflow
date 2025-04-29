#ifndef SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_P_H
#define SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_P_H

#include <QVariantAnimation>
#include <QSGNode>

#include <SVSCraftCore/musictimeline.h>

#include <ScopicFlowInternal/private/TimelineScaleQuickItem_p.h>

class QSGTextNode;
class QTextLayout;

namespace sflow {

    class ScaleSGNode : public QSGNode {
    public:
        explicit ScaleSGNode(TimelineScaleQuickItemPrivate *d) : d(d) {}
        ~ScaleSGNode() override;
        TimelineScaleQuickItemPrivate *d;

        QTextLayout *createTextLayoutForBarNumber(int bar);
        QSGTextNode *createTextNodeForBarNumber(int bar, const QColor &color);
        QTextLayout *createTextLayoutForTimeSignature(int numerator, int denominator);
        QSGTextNode *createTextNodeForTimeSignature(int numerator, int denominator, const QColor &color);

        QHash<int, QTextLayout *> barNumberTextLayouts;
        QHash<int, QSGTextNode *> barNumberTextNodes;
        QHash<qint64, QTextLayout *> timeSignatureTextLayouts;
        QHash<qint64, QSGTextNode *> timeSignatureTextNodes;
    };

    class TimelineScaleQuickItemPrivate {
        Q_DECLARE_PUBLIC(TimelineScaleQuickItem)
    public:
        TimelineScaleQuickItem *q_ptr;

        QPointer<TimeViewModel> timeViewModel;
        QPointer<TimeLayoutViewModel> timeLayoutViewModel;
        QPointer<SVS::MusicTimeline> timeline;

        QColor color;

        void updateTimeline();
    };
}

#endif //SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_P_H
