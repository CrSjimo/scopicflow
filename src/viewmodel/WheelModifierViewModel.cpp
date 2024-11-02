#include "WheelModifierViewModel.h"

namespace sflow {
    WheelModifierViewModel::WheelModifierViewModel(QObject *parent) : QObject(parent) {
        m_modifiers.insert(AlternateAxis, Qt::AltModifier);
        m_modifiers.insert(Zoom, Qt::ControlModifier);
        m_modifiers.insert(Page, Qt::ShiftModifier);
    }
    WheelModifierViewModel::~WheelModifierViewModel() = default;

    Qt::KeyboardModifier WheelModifierViewModel::modifier(WheelAction action) {
        return m_modifiers.value(action);
    }
    void WheelModifierViewModel::setModifier(WheelAction action, Qt::KeyboardModifier modifier) {
        if (m_modifiers.value(action) != modifier) {
            m_modifiers.insert(action, modifier);
            emit modifierChanged(action, modifier);
        }
    }

}