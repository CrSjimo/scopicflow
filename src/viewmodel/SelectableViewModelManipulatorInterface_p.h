#ifndef SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATORINTERFACE_P_H
#define SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATORINTERFACE_H

#  include <QObject>
#  include <QVariant>

#  include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT SelectableViewModelManipulatorInterface : public QObject {
        Q_OBJECT
    public:
        explicit SelectableViewModelManipulatorInterface(QObject *parent = nullptr);

        virtual void setSelected(const QVariant &item, bool selected) = 0;
        virtual bool isSelected(const QVariant &item) const = 0;

        virtual QVariant nextItem(const QVariant &item) const = 0;
        virtual QVariant previousItem(const QVariant &item) const = 0;
        virtual QVariant firstItem() const = 0;
        virtual QVariant lastItem() const = 0;

        virtual QVariant currentItem() const = 0;
        virtual void setCurrentItem(const QVariant &item) = 0;

        virtual QVariantList selection() const = 0;

        virtual int compareOrder(const QVariant &item1, const QVariant &item2) const = 0;

        virtual bool isValidItem(const QVariant &item) const = 0;

        virtual qsizetype getId(const QVariant &item) const = 0;
        virtual QVariant fromId(qsizetype id) const = 0;

        virtual QObject *viewModel() const = 0;

        static void registerViewModelInterface(const QString &className, const QMetaObject *metaObject);
        static const QMetaObject *getInterface(const QString &className);
    };

}

#endif //SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATORINTERFACE_P_H
