#ifndef SFLOW_POINTSEQUENCEVIEWMODEL_H
#define SFLOW_POINTSEQUENCEVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class PointSequenceViewModelPrivate;
    class PointSequenceViewModelQmlHandle;
    class PointSequenceViewModelManipulatorInterface;

    class SCOPIC_FLOW_EXPORT PointSequenceViewModel : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(PointSequenceViewModel)
        Q_PRIVATE_PROPERTY(d_func(), PointSequenceViewModelQmlHandle *handle MEMBER handle CONSTANT)
        Q_PRIVATE_SLOT(d_func(), void handleItemSelectedChanged())
        Q_PRIVATE_SLOT(d_func(), void handleItemPositionChanged())

    public:
        explicit PointSequenceViewModel(QObject *parent = nullptr, const QByteArray &positionProperty = "position", const QByteArray &selectedProperty = "selected");
        ~PointSequenceViewModel() override;

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
