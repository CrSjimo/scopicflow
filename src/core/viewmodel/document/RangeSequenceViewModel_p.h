#ifndef SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_P_H
#define SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_P_H

#include <QVariant>

#include <ScopicFlowCore/RangeSequenceViewModel.h>
#include <ScopicFlowCore/private/PointSequenceContainer_p.h>
#include <ScopicFlowCore/private/RangeSequenceContainer_p.h>
#include <ScopicFlowCore/private/SliceableViewModelManipulatorInterface_p.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT RangeSequenceViewModelSliceableManipulatorInterface : public SliceableViewModelManipulatorInterface {
        Q_OBJECT
    public:
        Q_DECLARE_PRIVATE(RangeSequenceViewModel)
        explicit RangeSequenceViewModelSliceableManipulatorInterface(RangeSequenceViewModelPrivate *d);
        ~RangeSequenceViewModelSliceableManipulatorInterface() override;

        QObjectList items() const override;
        QObjectList slice(int position, int length) const override;
        int itemPosition(QObject *item) const override;
        int itemLength(QObject *item) const override;
        QObjectList itemsBetween(QObject *item1, QObject *item2) const override;
        QObject *previousItem(QObject *item) const override;
        QObject *nextItem(QObject *item) const override;

    private:
        RangeSequenceViewModelPrivate *d_ptr;
    };

    class RangeSequenceViewModelPrivate {
        Q_DECLARE_PUBLIC(RangeSequenceViewModel)
    public:
        RangeSequenceViewModel *q_ptr;
        RangeSequenceViewModelSliceableManipulatorInterface *iSliceable{};

        QByteArray positionProperty;
        QByteArray lengthProperty;

        PointSequenceContainer pointContainer;
        RangeSequenceContainer rangeContainer;

        void insertItem(QObject *item);
        void removeItem(QObject *item);

        void handleItemPositionChanged();
        void handleItemLengthChanged();
    };

}

#endif //SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_P_H
