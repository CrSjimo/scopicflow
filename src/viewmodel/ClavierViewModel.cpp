#include "ClavierViewModel.h"

namespace sflow {
    ClavierViewModel::ClavierViewModel(QObject *parent) : QObject(parent), m_start(0), m_pixelDensity(24), m_minimumPixelDensity(16), m_maximumPixelDensity(64), m_cursorPosition(-1) {
    }
    ClavierViewModel::~ClavierViewModel() = default;
    double ClavierViewModel::start() const {
        return m_start;
    }
    void ClavierViewModel::setStart(double start) {
        if (m_start != start) {
            m_start = start;
            emit startChanged(start);
        }
    }
    double ClavierViewModel::pixelDensity() const {
        return m_pixelDensity;
    }
    void ClavierViewModel::setPixelDensity(double pixelDensity) {
        if (m_pixelDensity != pixelDensity) {
            m_pixelDensity = pixelDensity;
            emit pixelDensityChanged(pixelDensity);
        }
    }
    double ClavierViewModel::maximumPixelDensity() const {
        return m_maximumPixelDensity;
    }
    void ClavierViewModel::setMaximumPixelDensity(double maximumPixelDensity) {
        if (m_maximumPixelDensity != maximumPixelDensity) {
            m_maximumPixelDensity = maximumPixelDensity;
            emit maximumPixelDensityChanged(maximumPixelDensity);
        }
    }
    double ClavierViewModel::minimumPixelDensity() const {
        return m_minimumPixelDensity;
    }
    void ClavierViewModel::setMinimumPixelDensity(double minimumPixelDensity) {
        if (m_minimumPixelDensity != minimumPixelDensity) {
            m_minimumPixelDensity = minimumPixelDensity;
            emit minimumPixelDensityChanged(minimumPixelDensity);
        }
    }
    int ClavierViewModel::cursorPosition() const {
        return m_cursorPosition;
    }
    void ClavierViewModel::setCursorPosition(int cursorPosition) {
        if (m_cursorPosition != cursorPosition) {
            m_cursorPosition = cursorPosition;
            emit cursorPositionChanged(cursorPosition);
        }
    }
}