#include "PlaybackViewModel.h"

namespace sflow {
    PlaybackViewModel::PlaybackViewModel(QObject *parent) : QObject(parent), m_primaryPosition(0), m_secondaryPosition(0) {
    }
    PlaybackViewModel::~PlaybackViewModel() = default;

    int PlaybackViewModel::primaryPosition() const {
        return m_primaryPosition;
    }
    void PlaybackViewModel::setPrimaryPosition(int primaryPosition) {
        if (m_primaryPosition != primaryPosition) {
            m_primaryPosition = primaryPosition;
            emit primaryPositionChanged(primaryPosition);
        }
    }
    int PlaybackViewModel::secondaryPosition() const {
        return m_secondaryPosition;
    }
    void PlaybackViewModel::setSecondaryPosition(int secondaryPosition) {
        if (m_secondaryPosition != secondaryPosition) {
            m_secondaryPosition = secondaryPosition;
            emit secondaryPositionChanged(secondaryPosition);
        }
    }
}