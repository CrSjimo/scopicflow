#ifndef SCOPIC_FLOW_LISTVIEWMODEL_H
#define SCOPIC_FLOW_LISTVIEWMODEL_H

#include <QObject>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class ListViewModelPrivate;
    class ListViewModelQmlHandle;
    class ListViewModelManipulatorInterface;

    class SCOPIC_FLOW_CORE_EXPORT ListViewModel : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(ListViewModel)
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)
    public:
        explicit ListViewModel(QObject *parent = nullptr);
        ~ListViewModel() override;

        int count() const;
        QObjectList items() const;

        Q_INVOKABLE void insertItem(int index, QObject *item);
        Q_INVOKABLE void removeItem(int index);
        Q_INVOKABLE void rotate(int leftIndex, int middleIndex, int rightIndex);

    signals:
        void countChanged(int count);
        void itemsChanged();
        void itemAboutToInsert(int index, QObject *item);
        void itemInserted(int index, QObject *item);
        void itemAboutToRemove(int index, QObject *item);
        void itemRemoved(int index, QObject *item);
        void rotated(int leftIndex, int middleIndex, int rightIndex);

    private:
        friend class ListViewModelManipulatorInterface;
        QScopedPointer<ListViewModelPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_LISTVIEWMODEL_H
