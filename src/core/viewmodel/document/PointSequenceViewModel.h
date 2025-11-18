#ifndef SFLOW_POINTSEQUENCEVIEWMODEL_H
#define SFLOW_POINTSEQUENCEVIEWMODEL_H

#include <QObject>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class PointSequenceViewModelPrivate;
    class PointSequenceViewModelSliceableManipulatorInterface;
    class PointSequenceViewModelSelectableManipulatorInterface;

    class SCOPIC_FLOW_CORE_EXPORT PointSequenceViewModel : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(PointSequenceViewModel)
        Q_PRIVATE_PROPERTY(d_func(), PointSequenceViewModelSliceableManipulatorInterface *iSliceable MEMBER iSliceable CONSTANT)
        Q_PRIVATE_PROPERTY(d_func(), PointSequenceViewModelSelectableManipulatorInterface *iSelectable MEMBER iSelectable CONSTANT)
        Q_PRIVATE_SLOT(d_func(), void handleItemSelectedChanged())
        Q_PRIVATE_SLOT(d_func(), void handleItemPositionChanged())
        Q_PROPERTY(QObject *currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)
        Q_PROPERTY(QObjectList selection READ selection NOTIFY selectionChanged)

    public:
        explicit PointSequenceViewModel(QObject *parent = nullptr, const QByteArray &positionProperty = "position", const QByteArray &selectedProperty = "selected");
        ~PointSequenceViewModel() override;

        QObject *currentItem() const;
        void setCurrentItem(QObject *item);

        int count() const;
        QObjectList items() const;

        Q_INVOKABLE void insertItem(QObject *item);
        Q_INVOKABLE void removeItem(QObject *item);

        int selectionCount() const;
        QObjectList selection() const;

    Q_SIGNALS:
        void currentItemChanged(QObject *item);
        void countChanged(int count);
        void itemsChanged();
        void selectionCountChanged(int selectionCount);
        void selectionChanged();
        void itemUpdated(QObject *item);
        void itemAboutToInsert(QObject *item);
        void itemInserted(QObject *item);
        void itemAboutToRemove(QObject *item);
        void itemRemoved(QObject *item);
        void itemSelected(QObject *item);
        void itemDeselected(QObject *item);

    private:
        QScopedPointer<PointSequenceViewModelPrivate> d_ptr;

    };

}

#endif //SFLOW_POINTSEQUENCEVIEWMODEL_H
