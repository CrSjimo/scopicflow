#ifndef SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATORINTERFACE_P_H
#define SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATORINTERFACE_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT SelectableViewModelManipulatorInterface : public QObject {
        Q_OBJECT
    public:
        explicit SelectableViewModelManipulatorInterface(QObject *parent = nullptr);
        virtual void setSelected(QObject *item, bool selected) = 0;
        virtual bool isSelected(QObject *item) const = 0;
        virtual QObject *nextItem(QObject *item) const = 0;
        virtual QObject *previousItem(QObject *item) const = 0;
        virtual QObject *firstItem() const = 0;
        virtual QObject *lastItem() const = 0;
        virtual QObject *currentItem() const = 0;
        virtual void setCurrentItem(QObject *item) = 0;
        virtual QObjectList selection() const = 0;
        virtual int compareOrder(QObject *item1, QObject *item2) const = 0;
        virtual QObject *viewModel() const = 0;

        static void registerViewModelInterface(const QString &className, const QMetaObject *metaObject);
        static const QMetaObject *getInterface(const QString &className);
    };

}

#endif //SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATORINTERFACE_P_H
