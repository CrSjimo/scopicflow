#ifndef SCOPIC_FLOW_DYNAMICMIXINGVIEWMODEL_P_H
#define SCOPIC_FLOW_DYNAMICMIXINGVIEWMODEL_P_H

#include <map>
#include <unordered_map>

#include <ScopicFlowCore/DynamicMixingViewModel.h>
#include <ScopicFlowCore/private/SliceableViewModelManipulatorInterface_p.h>

namespace sflow {

    class DynamicMixingViewModelPrivate;

    class SCOPIC_FLOW_CORE_EXPORT DynamicMixingViewModelSliceableManipulatorInterface : public SliceableViewModelManipulatorInterface {
        Q_OBJECT

    public:
        explicit DynamicMixingViewModelSliceableManipulatorInterface(DynamicMixingViewModelPrivate *d);
        ~DynamicMixingViewModelSliceableManipulatorInterface() override;

        QObjectList items() const override;
        QObjectList slice(int position, int length) const override;
        int itemPosition(QObject *item) const override;
        QObjectList itemsBetween(QObject *item1, QObject *item2) const override;
        QObject *previousItem(QObject *item) const override;
        QObject *nextItem(QObject *item) const override;

    private:
        DynamicMixingViewModelPrivate *d_ptr;
    };

    class DynamicMixingViewModelPrivate {
        Q_DECLARE_PUBLIC(DynamicMixingViewModel)

    public:
        using Container = std::multimap<int, DynamicMixingAnchorViewModel *>;
        using Iterator = Container::iterator;

        DynamicMixingViewModel *q_ptr = nullptr;
        DynamicMixingViewModelSliceableManipulatorInterface *iSliceable = nullptr;
        int voiceCount = 1;

        Container items;
        std::unordered_map<DynamicMixingAnchorViewModel *, Iterator> itemIterators;

        struct InteractiveMoveState {
            int position = 0;
            QList<double> ratio;
        };

        DynamicMixingAnchorViewModel *movingItem = nullptr;
        std::unordered_map<DynamicMixingAnchorViewModel *, InteractiveMoveState> movingItems;

        bool isMovingItem(const DynamicMixingAnchorViewModel *item) const;
        DynamicMixingAnchorViewModel *effectiveItemAtPosition(int position) const;
        DynamicMixingAnchorViewModel *effectiveItemBefore(int position) const;
        DynamicMixingAnchorViewModel *effectiveItemAfter(int position) const;
        bool relocateItem(DynamicMixingAnchorViewModel *item, int position);
        bool removeItem(DynamicMixingAnchorViewModel *item,
                        bool itemIsBeingDestroyed,
                        bool notifyCollection = true);
        void attachItem(DynamicMixingAnchorViewModel *item);
    };

}

#endif // SCOPIC_FLOW_DYNAMICMIXINGVIEWMODEL_P_H
