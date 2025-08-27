#include "SelectableViewModelManipulator_p.h"

#include <QQmlEngine>

#include <ScopicFlowCore/private/SelectableViewModelManipulatorInterface_p.h>
#include <ScopicFlowInternal/private/ScopedIntermediate_p.h>

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
        do {
            if (!viewModel) {
                break;
            }
            auto metaObject = SelectableViewModelManipulatorInterface::getInterface(viewModel);
            if (!metaObject) {
                if (auto engine = qjsEngine(this))
                    engine->throwError(QString("Unknown view model class"));
                break;
            }
            auto obj = metaObject->newInstance(viewModel, static_cast<QObject *>(this));
            m_interface = qobject_cast<SelectableViewModelManipulatorInterface *>(obj);
            if (!m_interface) {
                if (auto engine = qjsEngine(this))
                    engine->throwError(QString("Invalid meta object of interface"));
                break;
            }
        } while (false);
        emit viewModelChanged();
    }
    void SelectableViewModelManipulator::select(const QVariant &item, int button, int modifiers) const {
        if (!m_interface) {
            if (auto engine = qjsEngine(this))
                engine->throwError(QString("View model is not set"));
            return;
        }
        ScopedIntermediate scopedIntermediate(m_interface->viewModel());
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
            } else if (order > 0) {
                for (auto o = m_interface->currentItem(); m_interface->isValidItem(o); o = m_interface->previousItem(o)) {
                    m_interface->setSelected(o, true);
                    if (o == item)
                        break;
                }
            } else {
                m_interface->setSelected(m_interface->currentItem(), true);
                m_interface->setSelected(item, true);
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