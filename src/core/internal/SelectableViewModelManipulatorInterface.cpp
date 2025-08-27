#include "SelectableViewModelManipulatorInterface_p.h"

#include <QHash>

namespace sflow {

    static QHash<QString, const QMetaObject *> m_registry;

    SelectableViewModelManipulatorInterface::SelectableViewModelManipulatorInterface(QObject *parent) : QObject(parent) {
    }

    void SelectableViewModelManipulatorInterface::registerViewModelInterface(const QString &className, const QMetaObject *metaObject) {
        m_registry.insert(className, metaObject);
    }
    const QMetaObject *SelectableViewModelManipulatorInterface::getInterface(const QString &className) {
        return m_registry.value(className);
    }
}