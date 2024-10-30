#include "TimeViewModel.h"

namespace sflow {

    TimeViewModel::TimeViewModel(QObject *parent) : QObject(parent), m_start(0.0), m_pixelDensity(0.2), m_timeline(nullptr) {
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
    double TimeViewModel::pixelDensity() const {
        return m_pixelDensity;
    }
    void TimeViewModel::setPixelDensity(double pixelDensity) {
        if (!qFuzzyCompare(m_pixelDensity, pixelDensity)) {
            m_pixelDensity = pixelDensity;
            emit pixelDensityChanged(pixelDensity);
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