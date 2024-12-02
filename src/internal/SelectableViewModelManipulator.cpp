#include "SelectableViewModelManipulator_p.h"

#include <QHash>
#include <QQmlEngine>

namespace sflow {

    namespace {
        auto _ = qmlRegisterType<SelectableViewModelManipulator>("dev.sjimo.ScopicFlow.Private.Internal", 1, 0, "SelectableViewModelManipulator");
    }

    static QHash<QString, const QMetaObject *> m_registry;

    SelectableViewModelManipulatorInterface::SelectableViewModelManipulatorInterface(QObject *parent) : QObject(parent) {
    }
    SelectableViewModelManipulator::SelectableViewModelManipulator(QObject *parent) : QObject(parent), m_interface(nullptr) {
    }
    SelectableViewModelManipulator::~SelectableViewModelManipulator() = default;
    QObject *SelectableViewModelManipulator::viewModel() const {
        if (m_interface)
            return m_interface->viewModel();
        return nullptr;
    }
    void SelectableViewModelManipulator::setViewModel(QObject *viewModel) {
        delete m_interface;
        m_interface = nullptr;
        if (!viewModel) {
            return;
        }
        QString className = viewModel->metaObject()->className();
        auto metaObject = m_registry.value(className);
        if (!metaObject) {
            if (auto engine = qjsEngine(this))
                engine->throwError(QString("Unknown view model class"));
            return;
        }
        auto obj = metaObject->newInstance(viewModel, static_cast<QObject *>(this));
        m_interface = qobject_cast<SelectableViewModelManipulatorInterface *>(obj);
        if (!m_interface) {
            if (auto engine = qjsEngine(this))
                engine->throwError(QString("Invalid meta object of interface"));
            return;
        }
    }
    void SelectableViewModelManipulator::select(QObject *item, int button, int modifiers) const {
        if (!m_interface) {
            if (auto engine = qjsEngine(this))
                engine->throwError(QString("View model is not set"));
            return;
        }
        if (!item) {
            if (!(modifiers & Qt::ControlModifier) && !(modifiers & Qt::ShiftModifier)) {
                for (auto o : m_interface->selection())
                    m_interface->setSelected(o, false);
            }
            return;
        }
        if (!(modifiers & Qt::ControlModifier) && !(button == Qt::RightButton && m_interface->isSelected(item))) {
            for (auto o : m_interface->selection())
                m_interface->setSelected(o, false);
        }
        if (modifiers & Qt::ShiftModifier) {
            if (!m_interface->currentItem()) {
                m_interface->setCurrentItem(item);
            }
            int order = m_interface->compareOrder(m_interface->currentItem(), item);
            if (order < 0) {
                for (auto o = m_interface->currentItem(); o; o = m_interface->nextItem(o)) {
                    m_interface->setSelected(o, true);
                    if (o == item)
                        break;
                }
            } else {
                for (auto o = m_interface->currentItem(); o; o = m_interface->previousItem(o)) {
                    m_interface->setSelected(o, true);
                    if (o == item)
                        break;
                }
            }
            return;
        }
        m_interface->setCurrentItem(item);
        if (modifiers & Qt::ControlModifier) {
            if (button == Qt::RightButton) {
                m_interface->setSelected(item, true);
            } else {
                m_interface->setSelected(item, !m_interface->isSelected(item));
            }
            return;
        }
        m_interface->setSelected(item, true);
    }
    void SelectableViewModelManipulator::registerViewModelInterface(const QString &className, const QMetaObject *metaObject) {
        m_registry.insert(className, metaObject);
    }
}