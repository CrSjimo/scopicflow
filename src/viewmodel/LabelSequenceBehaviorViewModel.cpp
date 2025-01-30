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
}