#include "ScaleHighlightViewModel.h"

namespace sflow {

    ScaleHighlightViewModel::ScaleHighlightViewModel(QObject *parent) : QObject(parent),
        m_position(0), m_length(0), m_cMask(0) {
    }

    ScaleHighlightViewModel::~ScaleHighlightViewModel() = default;

    int ScaleHighlightViewModel::position() const {
        return m_position;
    }

    void ScaleHighlightViewModel::setPosition(int position) {
        if (m_position != position) {
            m_position = position;
            Q_EMIT positionChanged();
        }
    }

    int ScaleHighlightViewModel::length() const {
        return m_length;
    }

    void ScaleHighlightViewModel::setLength(int length) {
        if (m_length != length) {
            m_length = length;
            Q_EMIT lengthChanged();
        }
    }

    int ScaleHighlightViewModel::cMask() const {
        return m_cMask;
    }

    void ScaleHighlightViewModel::setCMask(int cMask) {
        if (m_cMask != cMask) {
            m_cMask = cMask;
            Q_EMIT cMaskChanged();
        }
    }

}
