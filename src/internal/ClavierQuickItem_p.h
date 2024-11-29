#ifndef SCOPIC_FLOW_CLAVIERQUICKITEM_P_H
#define SCOPIC_FLOW_CLAVIERQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>

namespace sflow {

    class ClavierQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT ClavierQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(ClavierQuickItem)
    public:
        explicit ClavierQuickItem(QQuickItem *parent = nullptr);
        ~ClavierQuickItem() override;

        Q_INVOKABLE QString keyNameImpl(int key, int accidentalType) const;

    private:
        QScopedPointer<ClavierQuickItemPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_CLAVIERQUICKITEM_P_H
