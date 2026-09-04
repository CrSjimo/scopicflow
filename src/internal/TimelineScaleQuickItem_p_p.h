#ifndef SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_P_H
#define SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_P_H

#include <ScopicFlowInternal/private/TimelineScaleQuickItem_p.h>

#include <QVariantAnimation>
#include <QSGTransformNode>

#include <SVSCraftCore/MusicTimeline.h>

class QSGTextNode;
class QTextLayout;

namespace sflow {

    class ScaleSGNode : public QSGTransformNode {
    public:
        explicit ScaleSGNode(TimelineScaleQuickItemPrivate *d) : d(d) {}
        ~ScaleSGNode() override;
        TimelineScaleQuickItemPrivate *d;

        QTextLayout *createTextLayoutForBarNumber(int bar);
        QSGTextNode *createTextNodeForBarNumber(int bar);
        QTextLayout *createTextLayoutForTimeSignature(int numerator, int denominator);
        QSGTextNode *createTextNodeForTimeSignature(int bar, int numerator, int denominator);

        QHash<int, QSharedPointer<QTextLayout>> barNumberTextLayouts;
        QHash<int, QSharedPointer<QSGTextNode>> barNumberTextNodes;
        QHash<qint64, QSharedPointer<QTextLayout>> timeSignatureTextLayouts;
        QHash<QString, QSharedPointer<QSGTextNode>> timeSignatureTextNodes;
        QQuickWindow *window{};
    };

    class TimelineScaleQuickItemPrivate {
        Q_DECLARE_PUBLIC(TimelineScaleQuickItem)
    public:
        TimelineScaleQuickItem *q_ptr;

        QPointer<TimeViewModel> timeViewModel;
        QPointer<TimeLayoutViewModel> timeLayoutViewModel;
        QPointer<const SVS::MusicTimeline> timeline;

        QColor color;
        QFont font;

        void updateTimeline();
    };
}

#endif //SCOPIC_FLOW_TIMELINESCALEQUICKITEM_P_P_H
