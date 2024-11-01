#include "TimeAlignmentViewModel.h"

namespace sflow {
    TimeAlignmentViewModel::TimeAlignmentViewModel(QObject *parent) : TimeViewModel(parent), m_positionAlignment(480), m_lengthQuantization(480), m_lengthHint(480) {
    }
    TimeAlignmentViewModel::~TimeAlignmentViewModel() = default;
    int TimeAlignmentViewModel::positionAlignment() const {
        return m_positionAlignment;
    }
    void TimeAlignmentViewModel::setPositionAlignment(int positionAlignment) {
        if (m_positionAlignment != positionAlignment) {
            m_positionAlignment = positionAlignment;
            emit positionAlignmentChanged(positionAlignment);
        }
    }
    int TimeAlignmentViewModel::lengthQuantization() const {
        return m_lengthQuantization;
    }
    void TimeAlignmentViewModel::setLengthQuantization(int lengthQuantization) {
        if (m_lengthQuantization != lengthQuantization) {
            m_lengthQuantization = lengthQuantization;
            emit lengthQuantizationChanged(lengthQuantization);
        }
    }
    int TimeAlignmentViewModel::lengthHint() const {
        return m_lengthHint;
    }
    void TimeAlignmentViewModel::setLengthHint(int lengthHint) {
        if (m_lengthHint != lengthHint) {
            m_lengthHint = lengthHint;
            emit lengthHintChanged(lengthHint);
        }
    }
}