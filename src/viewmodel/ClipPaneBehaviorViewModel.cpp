#include "ClipPaneBehaviorViewModel.h"

namespace sflow {
    ClipPaneBehaviorViewModel::ClipPaneBehaviorViewModel(QObject *parent) : QObject(parent), m_lengthHint(0), m_mouseBehavior(Pointer) {
    }
    ClipPaneBehaviorViewModel::~ClipPaneBehaviorViewModel() = default;
    int ClipPaneBehaviorViewModel::lengthHint() const {
        return m_lengthHint;
    }
    void ClipPaneBehaviorViewModel::setLengthHint(int lengthHint) {
        if (m_lengthHint != lengthHint) {
            m_lengthHint = lengthHint;
            emit lengthHintChanged(lengthHint);
        }
    }
    ClipPaneBehaviorViewModel::MouseBehavior ClipPaneBehaviorViewModel::mouseBehavior() const {
        return m_mouseBehavior;
    }
    void ClipPaneBehaviorViewModel::setMouseBehavior(MouseBehavior mouseBehavior) {
        if (m_mouseBehavior != mouseBehavior) {
            m_mouseBehavior = mouseBehavior;
            emit mouseBehaviorChanged(mouseBehavior);
        }
    }
}