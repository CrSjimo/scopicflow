#include "LabelSequenceLayoutViewModel.h"

namespace sflow {
    LabelSequenceLayoutViewModel::LabelSequenceLayoutViewModel(QObject *parent) : QObject(parent), m_editing(false) {
    }
    LabelSequenceLayoutViewModel::~LabelSequenceLayoutViewModel() = default;
    bool LabelSequenceLayoutViewModel::editing() const {
        return m_editing;
    }
    void LabelSequenceLayoutViewModel::setEditing(bool editing) {
        if (m_editing != editing) {
            m_editing = editing;
            emit editingChanged(editing);
        }
    }
}