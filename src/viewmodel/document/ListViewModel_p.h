#ifndef SCOPIC_FLOW_LISTVIEWMODEL_P_H
#define SCOPIC_FLOW_LISTVIEWMODEL_P_H

#include <ScopicFlow/ListViewModel.h>

namespace sflow {

    class ListViewModelQmlHandle : public QObject {
        Q_OBJECT
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
        Q_PROPERTY(bool intermediate READ intermediate WRITE setIntermediate NOTIFY intermediateChanged)
        Q_PROPERTY(QObjectList items READ items NOTIFY itemsChanged)
    public:
        Q_DECLARE_PRIVATE(ListViewModel)
        explicit ListViewModelQmlHandle(ListViewModelPrivate *d);
        ~ListViewModelQmlHandle() override;

        int count() const;

        int currentIndex() const;
        void setCurrentIndex(int index);

        bool intermediate() const;
        void setIntermediate(bool intermediate);

        QObjectList items() const;

        Q_INVOKABLE void rotateItems(int index, int count, int middleIndex);
        Q_INVOKABLE QObject *itemAt(int index) const;

    signals:
        void countChanged();
        void currentIndexChanged();
        void intermediateChanged();
        void itemsChanged();

    private:
        ListViewModelPrivate *d_ptr;

    };

    class ListViewModelPrivate {
        Q_DECLARE_PUBLIC(ListViewModel)
    public:
        ListViewModel *q_ptr;
        ListViewModelQmlHandle *handle{};

        QByteArray selectedProperty;

        QObjectList items;
        int currentIndex{};
        bool intermediate{};

        void handleItemSelectedChanged();

    };
}

#endif //SCOPIC_FLOW_LISTVIEWMODEL_P_H
