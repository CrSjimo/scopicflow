#ifndef SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATOR_P_H
#define SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATOR_P_H

#include <QObject>
#include <qqmlintegration.h>

namespace sflow {

    class SelectableViewModelManipulatorInterface;

    class SelectableViewModelManipulator : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(QObject *viewModel READ viewModel WRITE setViewModel NOTIFY viewModelChanged)
    public:
        explicit SelectableViewModelManipulator(QObject *parent = nullptr);
        ~SelectableViewModelManipulator() override;

        QObject *viewModel() const;
        void setViewModel(QObject *viewModel);

        inline SelectableViewModelManipulatorInterface *interface() const {
            return m_interface;
        }

        Q_INVOKABLE void select(const QVariant &item, int button, int modifiers) const;

    signals:
        void viewModelChanged();

    private:
        SelectableViewModelManipulatorInterface *m_interface;
    };
} // sflow

#endif //SCOPIC_FLOW_SELECTABLEVIEWMODELMANIPULATOR_P_H
