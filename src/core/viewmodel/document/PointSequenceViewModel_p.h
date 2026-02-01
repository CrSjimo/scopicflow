#ifndef SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H
#define SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H

#include <QVariant>

#include <ScopicFlowCore/PointSequenceViewModel.h>
#include <ScopicFlowCore/private/PointSequenceContainer_p.h>
#include <ScopicFlowCore/private/SliceableViewModelManipulatorInterface_p.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT PointSequenceViewModelSliceableManipulatorInterface : public SliceableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_DECLARE_PRIVATE(PointSequenceViewModel)
        explicit PointSequenceViewModelSliceableManipulatorInterface(PointSequenceViewModelPrivate *d);
        ~PointSequenceViewModelSliceableManipulatorInterface() override;

        QObjectList items() const override;
        QObjectList slice(int position, int length) const override;
        int itemPosition(QObject *item) const override;
        QObjectList itemsBetween(QObject *item1, QObject *item2) const override;
        QObject *previousItem(QObject *item) const override;
        QObject *nextItem(QObject *item) const override;

    private:
        PointSequenceViewModelPrivate *d_ptr;
    };

    class PointSequenceViewModelPrivate {
        Q_DECLARE_PUBLIC(PointSequenceViewModel)
    public:
        PointSequenceViewModel *q_ptr;
        PointSequenceViewModelSliceableManipulatorInterface *iSliceable{};

        QByteArray positionProperty;

        PointSequenceContainer container;

        void insertItem(QObject *item);
        void removeItem(QObject *item);

        void handleItemPositionChanged();
    };

}

#endif //SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H
