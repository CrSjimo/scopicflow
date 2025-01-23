#include "TimeViewModel.h"

#include <iostream>
#include <ostream>
#include <set>

namespace sflow {

    TimeViewModel::TimeViewModel(QObject *parent) : QObject(parent),
    m_start(0.0),
    m_end(48000),
    m_timeline(nullptr){


    }

    TimeViewModel::~TimeViewModel() = default;

    double TimeViewModel::start() const {
        return m_start;
    }
    void TimeViewModel::setStart(double start) {
        if (!qFuzzyCompare(m_start, start)) {
            m_start = start;
            emit startChanged(start);
        }
    }
    double TimeViewModel::end() const {
        return m_end;
    }
    void TimeViewModel::setEnd(double end) {
        if (!qFuzzyCompare(m_end, end)) {
            m_end = end;
            emit endChanged(end);
        }
    }
    SVS::MusicTimeline *TimeViewModel::timeline() const {
        return m_timeline;
    }
    void TimeViewModel::setTimeline(SVS::MusicTimeline *timeline) {
        if (m_timeline != timeline) {
            m_timeline = timeline;
            emit timelineChanged();
        }
    }
}