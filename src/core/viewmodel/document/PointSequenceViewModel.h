#ifndef SFLOW_POINTSEQUENCEVIEWMODEL_H
#define SFLOW_POINTSEQUENCEVIEWMODEL_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class PointSequenceViewModelPrivate;
    class PointSequenceViewModelSliceableManipulatorInterface;
    class PointSequenceViewModelSelectableManipulatorInterface;

    class SCOPIC_FLOW_CORE_EXPORT PointSequenceViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(PointSequenceViewModel)
        Q_PRIVATE_PROPERTY(d_func(), PointSequenceViewModelSliceableManipulatorInterface *iSliceable MEMBER iSliceable CONSTANT)
        Q_PRIVATE_SLOT(d_func(), void handleItemPositionChanged())
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)

    public:
        explicit PointSequenceViewModel(QObject *parent = nullptr, const QByteArray &positionProperty = "position");
        ~PointSequenceViewModel() override;

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
        QScopedPointer<PointSequenceViewModelPrivate> d_ptr;

    };

}

#endif //SFLOW_POINTSEQUENCEVIEWMODEL_H
