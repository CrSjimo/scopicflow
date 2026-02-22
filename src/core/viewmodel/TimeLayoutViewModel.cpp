#include "TimeLayoutViewModel.h"

namespace sflow {

    TimeLayoutViewModel::TimeLayoutViewModel(QObject *parent) : QObject(parent),
        m_pixelDensity(0.2), m_maximumPixelDensity(1.0), m_minimumPixelDensity(0.00390625),
        m_cursorPosition(-1), m_positionAlignment(480), m_displayPositionAlignment(480), m_isDisplayPositionExplicitlySet(false) {
    }

    TimeLayoutViewModel::~TimeLayoutViewModel() = default;

    double TimeLayoutViewModel::pixelDensity() const {
        return m_pixelDensity;
    }

    void TimeLayoutViewModel::setPixelDensity(double pixelDensity) {
        if (m_pixelDensity != pixelDensity) {
            m_pixelDensity = pixelDensity;
            Q_EMIT pixelDensityChanged();
        }
    }

    double TimeLayoutViewModel::maximumPixelDensity() const {
        return m_maximumPixelDensity;
    }

    void TimeLayoutViewModel::setMaximumPixelDensity(double maximumPixelDensity) {
        if (m_maximumPixelDensity != maximumPixelDensity) {
            m_maximumPixelDensity = maximumPixelDensity;
            Q_EMIT maximumPixelDensityChanged();
        }
    }

    double TimeLayoutViewModel::minimumPixelDensity() const {
        return m_minimumPixelDensity;
    }

    void TimeLayoutViewModel::setMinimumPixelDensity(double minimumPixelDensity) {
        if (m_minimumPixelDensity != minimumPixelDensity) {
            m_minimumPixelDensity = minimumPixelDensity;
            Q_EMIT minimumPixelDensityChanged();
        }
    }

    int TimeLayoutViewModel::cursorPosition() const {
        return m_cursorPosition;
    }

    void TimeLayoutViewModel::setCursorPosition(int cursorPosition) {
        if (m_cursorPosition != cursorPosition) {
            m_cursorPosition = cursorPosition;
            Q_EMIT cursorPositionChanged();
        }
    }

    int TimeLayoutViewModel::positionAlignment() const {
        return m_positionAlignment;
    }

    void TimeLayoutViewModel::setPositionAlignment(int positionAlignment) {
        if (m_positionAlignment != positionAlignment) {
            m_positionAlignment = positionAlignment;
            Q_EMIT positionAlignmentChanged();
            if (!m_isDisplayPositionExplicitlySet) {
                m_displayPositionAlignment = positionAlignment;
                Q_EMIT displayPositionAlignmentChanged();
            }
        }
    }

    int TimeLayoutViewModel::displayPositionAlignment() const {
        return m_displayPositionAlignment;
    }

    void TimeLayoutViewModel::setDisplayPositionAlignment(int displayPositionAlignment) {
        if (m_displayPositionAlignment != displayPositionAlignment) {
            m_displayPositionAlignment = displayPositionAlignment;
            m_isDisplayPositionExplicitlySet = true;
            Q_EMIT displayPositionAlignmentChanged();
        }
    }

    void TimeLayoutViewModel::resetDisplayPositionAlignment() {
        m_isDisplayPositionExplicitlySet = false;
        if (m_displayPositionAlignment != m_positionAlignment) {
            m_displayPositionAlignment = m_positionAlignment;
            Q_EMIT displayPositionAlignmentChanged();
        }
    }

}
