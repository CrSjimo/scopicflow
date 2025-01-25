#include "TrackListLayoutViewModel.h"

namespace sflow {
    TrackListLayoutViewModel::TrackListLayoutViewModel(QObject *parent) : QObject(parent), m_viewportOffset(0), m_trackHandleEnabled(true) {
    }
    TrackListLayoutViewModel::~TrackListLayoutViewModel() = default;
    double TrackListLayoutViewModel::viewportOffset() const {
        return m_viewportOffset;
    }
    void TrackListLayoutViewModel::setViewportOffset(double value) {
        if (m_viewportOffset != value) {
            m_viewportOffset = value;
            emit viewportOffsetChanged(value);
        }
    }
    bool TrackListLayoutViewModel::trackHandleEnabled() const {
        return m_trackHandleEnabled;
    }
    void TrackListLayoutViewModel::setTrackHandleEnabled(bool value) {
        if (m_trackHandleEnabled != value) {
            m_trackHandleEnabled = value;
            emit trackHandleEnabledChanged(value);
        }
    }
}