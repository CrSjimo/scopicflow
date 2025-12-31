#include "TrackListLayoutViewModel.h"

namespace sflow {

    TrackListLayoutViewModel::TrackListLayoutViewModel(QObject *parent) : QObject(parent),
        m_viewportOffset(0.0) {
    }

    TrackListLayoutViewModel::~TrackListLayoutViewModel() = default;

    double TrackListLayoutViewModel::viewportOffset() const {
        return m_viewportOffset;
    }

    void TrackListLayoutViewModel::setViewportOffset(double viewportOffset) {
        if (m_viewportOffset != viewportOffset) {
            m_viewportOffset = viewportOffset;
            Q_EMIT viewportOffsetChanged();
        }
    }

}

#include "moc_TrackListLayoutViewModel.cpp"
