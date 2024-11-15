#ifndef SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H
#define SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H

#include <QVariantAnimation>
#include <qsgnode.h>

#include <ScopicFlow/private/TimelineQuickItem_p.h>

class QSGTextNode;

namespace sflow {

    class ScaleSGNode : public QSGNode {
    public:
        explicit ScaleSGNode(TimelineQuickItemPrivate *d) : d(d) {}
        ~ScaleSGNode() override;
        TimelineQuickItemPrivate *d;

        QTextLayout *createTextLayoutForBarNumber(int bar);
        QSGTextNode *createTextNodeForBarNumber(int bar, const QColor &color);
        QTextLayout *createTextLayoutForTimeSignature(int numerator, int denominator);
        QSGTextNode *createTextNodeForTimeSignature(int numerator, int denominator, const QColor &color);

        QHash<int, QTextLayout *> barNumberTextLayouts;
        QHash<int, QSGTextNode *> barNumberTextNodes;
        QHash<qint64, QTextLayout *> timeSignatureTextLayouts;
        QHash<qint64, QSGTextNode *> timeSignatureTextNodes;
    };

    class TimelineQuickItemPrivate {
        Q_DECLARE_PUBLIC(TimelineQuickItem)
    public:
        TimelineQuickItem *q_ptr;

        TimeAlignmentViewModel *timeAlignmentViewModel = nullptr;
        SVS::MusicTimeline *timeline = nullptr;
        PlaybackViewModel *playbackViewModel = nullptr;
        ScrollBehaviorViewModel *scrollBehaviorViewModel = nullptr;
        AnimationViewModel *animationViewModel = nullptr;
        PaletteViewModel *paletteViewModel = nullptr;

        QColor backgroundColor;
        QColor foregroundColor;

        double tickToX(int tick) const;
        int xToTick(double x) const;
        int alignTick(int tick) const;
        int alignTickCeil(int tick) const;
        int alignTickFloor(int tick) const;
    };
}

#endif //SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H
