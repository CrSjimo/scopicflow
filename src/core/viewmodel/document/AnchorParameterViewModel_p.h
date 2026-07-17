#ifndef SCOPIC_FLOW_ANCHORPARAMETERVIEWMODEL_P_H
#define SCOPIC_FLOW_ANCHORPARAMETERVIEWMODEL_P_H

#include <map>
#include <unordered_map>

#include <ScopicFlowCore/AnchorParameterViewModel.h>
#include <ScopicFlowCore/private/SliceableViewModelManipulatorInterface_p.h>

namespace sflow {

    class AnchorParameterViewModelPrivate;

    class SCOPIC_FLOW_CORE_EXPORT AnchorParameterViewModelSliceableManipulatorInterface : public SliceableViewModelManipulatorInterface {
        Q_OBJECT

    public:
        explicit AnchorParameterViewModelSliceableManipulatorInterface(AnchorParameterViewModelPrivate *d);
        ~AnchorParameterViewModelSliceableManipulatorInterface() override;

        QObjectList items() const override;
        QObjectList slice(int position, int length) const override;
        int itemPosition(QObject *item) const override;
        QObjectList itemsBetween(QObject *item1, QObject *item2) const override;
        QObject *previousItem(QObject *item) const override;
        QObject *nextItem(QObject *item) const override;

    private:
        AnchorParameterViewModelPrivate *d_ptr;
    };

    class AnchorParameterViewModelPrivate {
        Q_DECLARE_PUBLIC(AnchorParameterViewModel)

    public:
        using Container = std::multimap<int, ParameterAnchorViewModel *>;
        using Iterator = Container::iterator;

        AnchorParameterViewModel *q_ptr = nullptr;
        AnchorParameterViewModelSliceableManipulatorInterface *iSliceable = nullptr;

        Container items;
        std::unordered_map<ParameterAnchorViewModel *, Iterator> itemIterators;

        struct InteractiveMoveState {
            int position = 0;
            double value = 0.0;
        };

        ParameterAnchorViewModel *movingItem = nullptr;
        std::unordered_map<ParameterAnchorViewModel *, InteractiveMoveState> movingItems;

        bool isMovingItem(const ParameterAnchorViewModel *item) const;
        ParameterAnchorViewModel *effectiveItemAtPosition(int position) const;
        ParameterAnchorViewModel *effectiveItemBefore(int position) const;
        ParameterAnchorViewModel *effectiveItemAfter(int position) const;
        bool relocateItem(ParameterAnchorViewModel *item, int position);
        bool removeItem(ParameterAnchorViewModel *item, bool itemIsBeingDestroyed);
        void attachItem(ParameterAnchorViewModel *item);
    };

}

#endif // SCOPIC_FLOW_ANCHORPARAMETERVIEWMODEL_P_H
