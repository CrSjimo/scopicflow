#include "LabelSequenceBehaviorViewModel.h"

namespace sflow {
    LabelSequenceBehaviorViewModel::LabelSequenceBehaviorViewModel(QObject *parent) : QObject(parent), m_editing(false) {
    }
    LabelSequenceBehaviorViewModel::~LabelSequenceBehaviorViewModel() = default;
    bool LabelSequenceBehaviorViewModel::editing() const {
        return m_editing;
    }
    void LabelSequenceBehaviorViewModel::setEditing(bool editing) {
        if (m_editing != editing) {
            m_editing = editing;
            emit editingChanged(editing);
        }
    }
    QColor LabelSequenceBehaviorViewModel::color() const {
        return m_color;
    }
    void LabelSequenceBehaviorViewModel::setColor(const QColor &color) {
        if (m_color != color) {
            m_color = color;
            emit colorChanged(color);
        }
    }
}