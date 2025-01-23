//
// Created by Crs_1 on 2025/1/23.
//

#include "TimeLayoutViewModel.h"

namespace sflow {
    TimeLayoutViewModel::TimeLayoutViewModel(QObject *parent) : QObject(parent),
    m_pixelDensity(0.2),
    m_maximumPixelDensity(2.0),
    m_minimumPixelDensity(1.0 / 256.0),
    m_cursorPosition(-1),
    m_positionAlignment(480) {
    }
    TimeLayoutViewModel::~TimeLayoutViewModel() = default;

    double TimeLayoutViewModel::pixelDensity() const {
        return m_pixelDensity;
    }
    void TimeLayoutViewModel::setPixelDensity(double pixelDensity) {
        if (!qFuzzyCompare(m_pixelDensity, pixelDensity)) {
            m_pixelDensity = pixelDensity;
            emit pixelDensityChanged(pixelDensity);
        }
    }
    double TimeLayoutViewModel::maximumPixelDensity() const {
        return m_maximumPixelDensity;
    }
    void TimeLayoutViewModel::setMaximumPixelDensity(double maximumPixelDensity) {
        if (!qFuzzyCompare(m_maximumPixelDensity, maximumPixelDensity)) {
            m_maximumPixelDensity = maximumPixelDensity;
            emit maximumPixelDensityChanged(maximumPixelDensity);
        }
    }
    double TimeLayoutViewModel::minimumPixelDensity() const {
        return m_minimumPixelDensity;
    }
    void TimeLayoutViewModel::setMinimumPixelDensity(double minimumPixelDensity) {
        if(!qFuzzyCompare(m_minimumPixelDensity, minimumPixelDensity)) {
            m_minimumPixelDensity = minimumPixelDensity;
            emit minimumPixelDensityChanged(minimumPixelDensity);
        }
    }
    int TimeLayoutViewModel::cursorPosition() const {
        return m_cursorPosition;
    }
    void TimeLayoutViewModel::setCursorPosition(int cursorPosition) {
        if (m_cursorPosition != cursorPosition) {
            m_cursorPosition = cursorPosition;
            emit cursorPositionChanged(cursorPosition);
        }
    }
    int TimeLayoutViewModel::positionAlignment() const {
        return m_positionAlignment;
    }
    void TimeLayoutViewModel::setPositionAlignment(int positionAlignment) {
        if (m_positionAlignment != positionAlignment) {
            m_positionAlignment = positionAlignment;
            emit positionAlignmentChanged(positionAlignment);
        }
    }
}