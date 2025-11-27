#ifndef SCOPIC_FLOW_SELECTIONCONTROLLER_H
#define SCOPIC_FLOW_SELECTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT SelectionController : public QObject {
        Q_OBJECT
        QML_ELEMENT

    public:
        explicit SelectionController(QObject *parent = nullptr);
        ~SelectionController() override;

        Q_INVOKABLE virtual QObjectList getSelectedItems() = 0;
        Q_INVOKABLE virtual QObjectList getSlicedSelectedItems(QObject *startItem, QObject *endItem) = 0;
        Q_INVOKABLE virtual bool isSameType(QObject *item1, QObject *item2) = 0;

        Q_INVOKABLE virtual void select(QObject *item, bool shouldToggle, bool shouldClearPreviousSelection) = 0;

    };

}

#endif //SCOPIC_FLOW_SELECTIONCONTROLLER_H
