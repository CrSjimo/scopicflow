#ifndef SCOPIC_FLOW_LISTVIEWMODEL_H
#define SCOPIC_FLOW_LISTVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class ListViewModelPrivate;
    class ListViewModelQmlHandle;
    class ListViewModelManipulatorInterface;

    class SCOPIC_FLOW_EXPORT ListViewModel : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(ListViewModel)
        Q_PRIVATE_PROPERTY(ListViewModel::d_func(), ListViewModelQmlHandle *handle MEMBER handle CONSTANT)
        Q_PRIVATE_SLOT(d_func(), void handleItemSelectedChanged())
    public:
        explicit ListViewModel(QObject *parent = nullptr, const QByteArray &selectedProperty = "selected");
        ~ListViewModel() override;

        int currentIndex() const;
        void setCurrentIndex(int index);

        bool intermediate() const;
        void setIntermediate(bool intermediate);

        QObjectList items() const;
        void setItems(const QObjectList &items);

        QObjectList selection() const;

    signals:
        void currentIndexChanged(int index);
        void intermediateChanged(bool intermediate);
        void itemsRotated(int index, int count, int middleIndex);
        void itemSelected(QObject *item);
        void itemDeselected(QObject *item);

    private:
        friend class ListViewModelManipulatorInterface;
        QScopedPointer<ListViewModelPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_LISTVIEWMODEL_H
