#ifndef SCOPIC_FLOW_CLAVIERQUICKITEM_P_P_H
#define SCOPIC_FLOW_CLAVIERQUICKITEM_P_P_H

#include <ScopicFlow/private/ClavierQuickItem_p.h>

#include <QVariantAnimation>

namespace sflow {

    class ClavierQuickItemPrivate {
        Q_DECLARE_PUBLIC(ClavierQuickItem)
    public:
        ClavierQuickItem *q_ptr;

        ClavierPalette *palette;
        ClavierViewModel *clavierViewModel = nullptr;
        WheelModifierViewModel *wheelModifierViewModel = nullptr;
        ClavierQuickItem::LabelStrategy labelStrategy = ClavierQuickItem::C;

        QVariantAnimation *startAnimation;
        QVariantAnimation *pixelDensityAnimation;
        bool currentAnimationFixStartToZero = false;
        bool currentAnimationEnsureEnd = false;

        void handlePixelDensityAnimation(double centerY, double newPixelDensity) const;
    };
}

#endif //SCOPIC_FLOW_CLAVIERQUICKITEM_P_P_H
