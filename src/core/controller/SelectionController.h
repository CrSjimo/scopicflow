#ifndef SCOPIC_FLOW_SELECTIONCONTROLLER_H
#define SCOPIC_FLOW_SELECTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT SelectionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        QML_UNCREATABLE("")
        Q_PROPERTY(QObject *currentItem READ currentItem NOTIFY currentItemChanged)
    public:
        explicit SelectionController(QObject *parent = nullptr);
        ~SelectionController() override;

        enum SelectionCommandFlag {
            Select = 0x1,
            Deselect = 0x2,
            Toggle = Select | Deselect,
            ClearPreviousSelection = 0x4,
            SetCurrentItem = 0x8,
        };
        Q_ENUM(SelectionCommandFlag)
        Q_DECLARE_FLAGS(SelectionCommand, SelectionCommandFlag)

        Q_INVOKABLE virtual QObjectList getSelectedItems() const = 0;
        Q_INVOKABLE virtual QObjectList getItemsBetween(QObject *startItem, QObject *endItem) const = 0;
        Q_INVOKABLE virtual void select(QObject *item, SelectionCommand command) = 0;
        Q_INVOKABLE virtual QObject *currentItem() const = 0;

        Q_INVOKABLE void selectByMouse(QObject *item, Qt::MouseButton button, Qt::KeyboardModifiers modifiers, const QString &selectedProperty = "selected");

    Q_SIGNALS:
        void currentItemChanged();

    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(SelectionController::SelectionCommand)

}

#endif //SCOPIC_FLOW_SELECTIONCONTROLLER_H
