#ifndef SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H
#define SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H

#include <QVariantAnimation>

#include <ScopicFlow/private/TimelineQuickItem_p.h>

class QSGTextNode;

namespace sflow {

    class TimelineQuickItemPrivate {
        Q_DECLARE_PUBLIC(TimelineQuickItem)
    public:
        ~TimelineQuickItemPrivate();
        TimelineQuickItem *q_ptr;

        TimelinePalette *palette = nullptr;
        TimeAlignmentViewModel *timeAlignmentViewModel = nullptr;
        PlaybackViewModel *playbackViewModel = nullptr;
        WheelModifierViewModel *wheelModifierViewModel = nullptr;
        AnimationViewModel *animationViewModel = nullptr;

        QTextLayout *createTextLayoutForBarNumber(int bar);
        QSGTextNode *createTextNodeForBarNumber(int bar, const QColor &color);
        QTextLayout *createTextLayoutForTimeSignature(int numerator, int denominator);
        QSGTextNode *createTextNodeForTimeSignature(int numerator, int denominator, const QColor &color);

        QHash<int, QTextLayout *> barNumberTextLayouts;
        QHash<int, QSGTextNode *> barNumberTextNodes;
        QHash<qint64, QTextLayout *> timeSignatureTextLayouts;
        QHash<qint64, QSGTextNode *> timeSignatureTextNodes;

        double tickToX(int tick) const;
        int xToTick(double x) const;
        int alignTick(int tick) const;
        int alignTickCeil(int tick) const;
        int alignTickFloor(int tick) const;

        QVariantAnimation *startAnimation;
        QVariantAnimation *pixelDensityAnimation;
        bool currentAnimationFixStartToZero = false;

        void handlePixelDensityAnimation(double centerX, double newPixelDensity) const;
    };
}

#endif //SCOPIC_FLOW_TIMELINEQUICKITEM_P_P_H
