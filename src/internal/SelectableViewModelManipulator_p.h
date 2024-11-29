#ifndef SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATOR_P_H
#define SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATOR_P_H

#include <QObject>

namespace sflow {

    class SelectableViewModelManipulatorInterface : public QObject {
        Q_OBJECT
    public:
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
    };

    class SelectableViewModelManipulator : public QObject {
        Q_OBJECT
        Q_PROPERTY(QObject *viewModel READ viewModel WRITE setViewModel NOTIFY viewModelChanged)
    public:
        explicit SelectableViewModelManipulator(QObject *parent = nullptr);
        ~SelectableViewModelManipulator() override;

        QObject *viewModel() const;
        void setViewModel(QObject *viewModel);

        Q_INVOKABLE void select(QObject *item, int button, int modifiers) const;

        static void registerViewModelInterface(const QString &className, const QMetaObject *metaObject);

    signals:
        void viewModelChanged();

    private:
        SelectableViewModelManipulatorInterface *m_interface;
    };
} // sflow

#endif //SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATOR_P_H
