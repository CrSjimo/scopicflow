#include "TimeViewModel.h"

#include <SVSCraftCore/MusicTimeline.h>

namespace sflow {

    TimeViewModel::TimeViewModel(QObject *parent) : QObject(parent),
        m_start(0.0), m_end(48000.0), m_timeline(nullptr) {
    }

    TimeViewModel::~TimeViewModel() = default;

    double TimeViewModel::start() const {
        return m_start;
    }

    void TimeViewModel::setStart(double start) {
        if (m_start != start) {
            m_start = start;
            Q_EMIT startChanged();
        }
    }

    double TimeViewModel::end() const {
        return m_end;
    }

    void TimeViewModel::setEnd(double end) {
        if (m_end != end) {
            m_end = end;
            Q_EMIT endChanged();
        }
    }

    const SVS::MusicTimeline *TimeViewModel::timeline() const {
        return m_timeline;
    }

    void TimeViewModel::setTimeline(const SVS::MusicTimeline *timeline) {
        if (m_timeline != timeline) {
            m_timeline = timeline;
            Q_EMIT timelineChanged();
        }
    }

}

#include "moc_TimeViewModel.cpp"
