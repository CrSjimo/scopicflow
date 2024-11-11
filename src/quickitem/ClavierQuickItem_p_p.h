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
        ScrollBehaviorViewModel *scrollBehaviorViewModel = nullptr;
        AnimationViewModel *animationViewModel = nullptr;
        ClavierQuickItem::LabelStrategy labelStrategy = ClavierQuickItem::C;
    };
}

#endif //SCOPIC_FLOW_CLAVIERQUICKITEM_P_P_H
