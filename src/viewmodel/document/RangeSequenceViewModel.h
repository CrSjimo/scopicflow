#ifndef SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_H
#define SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_H

#include <QObject>
#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class RangeSequenceViewModelPrivate;
    class RangeSequenceViewModelQmlHandle;

    class SCOPIC_FLOW_EXPORT RangeSequenceViewModel : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(RangeSequenceViewModel)
        Q_PRIVATE_PROPERTY(d_func(), RangeSequenceViewModelQmlHandle *handle MEMBER handle CONSTANT)
        Q_PRIVATE_SLOT(d_func(), void handleItemSelectedChanged())
        Q_PRIVATE_SLOT(d_func(), void handleItemPositionChanged())
        Q_PRIVATE_SLOT(d_func(), void handleItemLengthChanged())

    public:
        explicit RangeSequenceViewModel(QObject *parent = nullptr, const QByteArray &positionProperty = "position", const QByteArray &lengthProperty = "length", const QByteArray &selectedProperty = "selected", const QByteArray &overlappedProperty = "overlapped");
        ~RangeSequenceViewModel() override;

        QObject *currentItem() const;
        void setCurrentItem(QObject *item);

        bool intermediate() const;
        void setIntermediate(bool intermediate);

        int count() const;
        QObjectList items() const;

        void insertItem(QObject *item);
        void removeItem(QObject *item);

        QObjectList selection() const;

    signals:
        void currentItemChanged(QObject *item);
        void intermediateChanged(bool intermediate);
        void itemInserted(QObject *item);
        void itemRemoved(QObject *item);
        void itemSelected(QObject *item);
        void itemDeselected(QObject *item);

    private:
        QScopedPointer<RangeSequenceViewModelPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_H
