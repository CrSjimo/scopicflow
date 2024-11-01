#include "TimeViewModel.h"

#include <iostream>
#include <ostream>
#include <set>

namespace sflow {

    struct A {
        int a;
        int b;
        bool operator<(const A &o) const {
            if (a == o.a) {
                return b < o.b;
            }
            return a < o.a;
        }
    };

    TimeViewModel::TimeViewModel(QObject *parent) : QObject(parent),
    m_start(0.0),
    m_end(64.0),
    m_pixelDensity(0.2),
    m_maximumPixelDensity(2.0),
    m_minimumPixelDensity(1.0 / 256.0),
    m_timeline(nullptr),
    m_primaryPosition(0),
    m_secondaryPosition(0),
    m_cursorPosition(-1) {


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
    double TimeViewModel::pixelDensity() const {
        return m_pixelDensity;
    }
    void TimeViewModel::setPixelDensity(double pixelDensity) {
        if (!qFuzzyCompare(m_pixelDensity, pixelDensity)) {
            m_pixelDensity = pixelDensity;
            emit pixelDensityChanged(pixelDensity);
        }
    }
    double TimeViewModel::maximumPixelDensity() const {
        return m_maximumPixelDensity;
    }
    void TimeViewModel::setMaximumPixelDensity(double maximumPixelDensity) {
        if (!qFuzzyCompare(m_maximumPixelDensity, maximumPixelDensity)) {
            m_maximumPixelDensity = maximumPixelDensity;
            emit maximumPixelDensityChanged(maximumPixelDensity);
        }
    }
    double TimeViewModel::minimumPixelDensity() const {
        return m_minimumPixelDensity;
    }
    void TimeViewModel::setMinimumPixelDensity(double minimumPixelDensity) {
        if(!qFuzzyCompare(m_minimumPixelDensity, minimumPixelDensity)) {
            m_minimumPixelDensity = minimumPixelDensity;
            emit minimumPixelDensityChanged(minimumPixelDensity);
        }
    }
    int TimeViewModel::primaryPosition() const {
        return m_primaryPosition;
    }
    void TimeViewModel::setPrimaryPosition(int primaryPosition) {
        if (m_primaryPosition != primaryPosition) {
            m_primaryPosition = primaryPosition;
            emit primaryPositionChanged(primaryPosition);
        }
    }
    int TimeViewModel::secondaryPosition() const {
        return m_secondaryPosition;
    }
    void TimeViewModel::setSecondaryPosition(int secondaryPosition) {
        if (m_secondaryPosition != secondaryPosition) {
            m_secondaryPosition = secondaryPosition;
            emit secondaryPositionChanged(secondaryPosition);
        }
    }
    int TimeViewModel::cursorPosition() const {
        return m_cursorPosition;
    }
    void TimeViewModel::setCursorPosition(int cursorPosition) {
        if (m_cursorPosition != cursorPosition) {
            m_cursorPosition = cursorPosition;
            emit cursorPositionChanged(cursorPosition);
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