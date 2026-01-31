#ifndef SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_H
#define SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class RangeSequenceViewModelPrivate;
    class RangeSequenceViewModelSliceableManipulatorInterface;

    class SCOPIC_FLOW_CORE_EXPORT RangeSequenceViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(RangeSequenceViewModel)
        Q_PRIVATE_PROPERTY(d_func(), RangeSequenceViewModelSliceableManipulatorInterface *iSliceable MEMBER iSliceable CONSTANT)
        Q_PRIVATE_SLOT(d_func(), void handleItemPositionChanged())
        Q_PRIVATE_SLOT(d_func(), void handleItemLengthChanged())
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)

    public:
        explicit RangeSequenceViewModel(QObject *parent = nullptr, const QByteArray &positionProperty = "position", const QByteArray &lengthProperty = "length");
        ~RangeSequenceViewModel() override;

        int count() const;
        QObjectList items() const;

        Q_INVOKABLE void insertItem(QObject *item);
        Q_INVOKABLE void removeItem(QObject *item);

    Q_SIGNALS:
        void countChanged(int count);
        void itemsChanged();
        void itemUpdated(QObject *item);
        void itemAboutToInsert(QObject *item);
        void itemInserted(QObject *item);
        void itemAboutToRemove(QObject *item);
        void itemRemoved(QObject *item);

    private:
        QScopedPointer<RangeSequenceViewModelPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_H
