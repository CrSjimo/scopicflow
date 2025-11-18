#ifndef SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H
#define SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H

#include <map>
#include <algorithm>

#include <QHash>
#include <QSet>
#include <QVariant>

#include <ScopicFlowCore/PointSequenceViewModel.h>
#include <ScopicFlowCore/private/PointSequenceContainer_p.h>
#include <ScopicFlowCore/private/SliceableViewModelManipulatorInterface_p.h>
#include <ScopicFlowCore/private/SelectableViewModelManipulatorInterface_p.h>

namespace sflow {

    class PointSequenceViewModelSelectableManipulatorInterface : public SelectableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_INVOKABLE explicit PointSequenceViewModelSelectableManipulatorInterface(PointSequenceViewModelPrivate *d);
        ~PointSequenceViewModelSelectableManipulatorInterface() override = default;

        void setSelected(const QVariant &item, bool selected) override;
        bool isSelected(const QVariant &item) const override;
        QVariant nextItem(const QVariant &item) const override;
        QVariant previousItem(const QVariant &item) const override;
        QVariant firstItem() const override;
        QVariant lastItem() const override;
        QVariant currentItem() const override;
        void setCurrentItem(const QVariant &item) override;
        QVariantList selection() const override;
        int compareOrder(const QVariant &item1, const QVariant &item2) const override;
        bool isValidItem(const QVariant &item) const override;
        qsizetype getId(const QVariant &item) const override;
        QVariant fromId(qsizetype id) const override;
        QObject *viewModel() const override;

    private:
        PointSequenceViewModelPrivate *d;
    };

    class SCOPIC_FLOW_CORE_EXPORT PointSequenceViewModelSliceableManipulatorInterface : public SliceableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_DECLARE_PRIVATE(PointSequenceViewModel)
        explicit PointSequenceViewModelSliceableManipulatorInterface(PointSequenceViewModelPrivate *d);
        ~PointSequenceViewModelSliceableManipulatorInterface() override;

        QObjectList items() const override;
        Q_INVOKABLE QObjectList slice(int position, int length) const override;
        Q_INVOKABLE int itemPosition(QObject *item) const override;

    private:
        PointSequenceViewModelPrivate *d_ptr;
    };

    class PointSequenceViewModelPrivate {
        Q_DECLARE_PUBLIC(PointSequenceViewModel)
    public:
        PointSequenceViewModel *q_ptr;
        PointSequenceViewModelSliceableManipulatorInterface *iSliceable{};
        PointSequenceViewModelSelectableManipulatorInterface *iSelectable{};

        QByteArray selectedProperty;
        QByteArray positionProperty;

        PointSequenceContainer container;
        QSet<QObject *> selection;
        QObject *currentItem{};
        bool intermediate{};

        void insertItem(QObject *item);
        void removeItem(QObject *item);

        void handleItemSelectedChanged();
        void handleItemPositionChanged();
    };

}

#endif //SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H
