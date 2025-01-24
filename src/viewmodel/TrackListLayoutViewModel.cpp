#include "TrackListLayoutViewModel.h"

namespace sflow {
    TrackListLayoutViewModel::TrackListLayoutViewModel(QObject *parent) : QObject(parent), m_viewportOffset(0) {
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
}