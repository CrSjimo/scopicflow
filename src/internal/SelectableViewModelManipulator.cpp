#include "SelectableViewModelManipulator_p.h"

#include <QQmlEngine>

#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>

namespace sflow {

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
        auto metaObject = SelectableViewModelManipulatorInterface::getInterface(className);
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
    void SelectableViewModelManipulator::select(const QVariant &item, int button, int modifiers) const {
        if (!m_interface) {
            if (auto engine = qjsEngine(this))
                engine->throwError(QString("View model is not set"));
            return;
        }
        if (!m_interface->isValidItem(item)) {
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
            if (!m_interface->isValidItem(m_interface->currentItem())) {
                m_interface->setCurrentItem(item);
            }
            int order = m_interface->compareOrder(m_interface->currentItem(), item);
            if (order < 0) {
                for (auto o = m_interface->currentItem(); m_interface->isValidItem(o); o = m_interface->nextItem(o)) {
                    m_interface->setSelected(o, true);
                    if (o == item)
                        break;
                }
            } else {
                for (auto o = m_interface->currentItem(); m_interface->isValidItem(o); o = m_interface->previousItem(o)) {
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
}