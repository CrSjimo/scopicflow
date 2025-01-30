#include "PianoRollNoteAreaBehaviorViewModel.h"

namespace sflow {
    PianoRollNoteAreaBehaviorViewModel::PianoRollNoteAreaBehaviorViewModel(QObject *parent) : QObject(parent), m_editing(false), m_unitedExtend(false), m_mouseBehavior(Pointer) {
    }
    PianoRollNoteAreaBehaviorViewModel::~PianoRollNoteAreaBehaviorViewModel() = default;
    bool PianoRollNoteAreaBehaviorViewModel::editing() const {
        return m_editing;
    }
    void PianoRollNoteAreaBehaviorViewModel::setEditing(bool editing) {
        if (m_editing != editing) {
            m_editing = editing;
            emit editingChanged(editing);
        }
    }
    bool PianoRollNoteAreaBehaviorViewModel::unitedExtend() const {
        return m_unitedExtend;
    }
    void PianoRollNoteAreaBehaviorViewModel::setUnitedExtend(bool unitedExtend) {
        if (m_unitedExtend != unitedExtend) {
            m_unitedExtend = unitedExtend;
            emit unitedExtendChanged(unitedExtend);
        }
    }
    PianoRollNoteAreaBehaviorViewModel::MouseBehavior PianoRollNoteAreaBehaviorViewModel::mouseBehavior() const {
        return m_mouseBehavior;
    }
    void PianoRollNoteAreaBehaviorViewModel::setMouseBehavior(MouseBehavior mouseBehavior) {
        if (m_mouseBehavior != mouseBehavior) {
            m_mouseBehavior = mouseBehavior;
            emit mouseBehaviorChanged(mouseBehavior);
        }
    }
}