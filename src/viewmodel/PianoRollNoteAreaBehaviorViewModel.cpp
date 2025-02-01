#include "PianoRollNoteAreaBehaviorViewModel.h"

namespace sflow {
    PianoRollNoteAreaBehaviorViewModel::PianoRollNoteAreaBehaviorViewModel(QObject *parent) : QObject(parent),
    m_editing(false),
    m_unitedExtend(false),
    m_compactDisplay(false),
    m_lengthHint(0),
    m_mouseBehavior(Pointer) {
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
    QColor PianoRollNoteAreaBehaviorViewModel::color() const {
        return m_color;
    }
    void PianoRollNoteAreaBehaviorViewModel::setColor(const QColor &color) {
        if (m_color != color) {
            m_color = color;
            emit colorChanged(color);
        }
    }
    bool PianoRollNoteAreaBehaviorViewModel::compactDisplay() const {
        return m_compactDisplay;
    }
    void PianoRollNoteAreaBehaviorViewModel::setCompactDisplay(bool compactDisplay) {
        if (m_compactDisplay != compactDisplay) {
            m_compactDisplay = compactDisplay;
            emit compactDisplayChanged(compactDisplay);
        }
    }
    int PianoRollNoteAreaBehaviorViewModel::lengthHint() const {
        return m_lengthHint;
    }
    void PianoRollNoteAreaBehaviorViewModel::setLengthHint(int lengthHint) {
        if (m_lengthHint != lengthHint) {
            m_lengthHint = lengthHint;
            emit lengthHintChanged(lengthHint);
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