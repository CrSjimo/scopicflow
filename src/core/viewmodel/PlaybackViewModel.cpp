#include "PlaybackViewModel.h"

namespace sflow {
    PlaybackViewModel::PlaybackViewModel(QObject *parent) : QObject(parent),
    m_primaryPosition(0), m_secondaryPosition(0), m_loopStart(0), m_loopLength(-1) {
    }
    PlaybackViewModel::~PlaybackViewModel() = default;

    int PlaybackViewModel::primaryPosition() const {
        return m_primaryPosition;
    }
    void PlaybackViewModel::setPrimaryPosition(int primaryPosition) {
        if (m_primaryPosition != primaryPosition) {
            m_primaryPosition = primaryPosition;
            Q_EMIT primaryPositionChanged();
        }
    }
    int PlaybackViewModel::secondaryPosition() const {
        return m_secondaryPosition;
    }
    void PlaybackViewModel::setSecondaryPosition(int secondaryPosition) {
        if (m_secondaryPosition != secondaryPosition) {
            m_secondaryPosition = secondaryPosition;
            Q_EMIT secondaryPositionChanged();
        }
    }
    int PlaybackViewModel::loopStart() const {
        return m_loopStart;
    }
    void PlaybackViewModel::setLoopStart(int loopStart) {
        if (m_loopStart != loopStart) {
            m_loopStart = loopStart;
            Q_EMIT loopStartChanged();
        }
    }
    int PlaybackViewModel::loopLength() const {
        return m_loopLength;
    }
    void PlaybackViewModel::setLoopLength(int loopLength) {
        if (m_loopLength != loopLength) {
            m_loopLength = loopLength;
            Q_EMIT loopLengthChanged();
        }
    }

}
