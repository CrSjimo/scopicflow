#include "WheelModifierViewModel.h"

namespace sflow {
    WheelModifierViewModel::WheelModifierViewModel(QObject *parent) : QObject(parent), m_alternateAxisModifier(Qt::AltModifier), m_zoomModifier(Qt::ControlModifier), m_pageModifier(Qt::ShiftModifier) {
    }
    WheelModifierViewModel::~WheelModifierViewModel() = default;

    Qt::KeyboardModifier WheelModifierViewModel::alternateAxisModifier() const {
        return m_alternateAxisModifier;
    }
    void WheelModifierViewModel::setAlternateAxisModifier(Qt::KeyboardModifier modifier) {
        if (m_alternateAxisModifier != modifier) {
            m_alternateAxisModifier = modifier;
            emit alternateAxisModifierChanged(modifier);
        }
    }
    Qt::KeyboardModifier WheelModifierViewModel::zoomModifier() const {
        return m_zoomModifier;
    }
    void WheelModifierViewModel::setZoomModifier(Qt::KeyboardModifier modifier) {
        if (m_zoomModifier != modifier) {
            m_zoomModifier = modifier;
            emit zoomModifierChanged(modifier);
        }
    }
    Qt::KeyboardModifier WheelModifierViewModel::pageModifier() const {
        return m_pageModifier;
    }
    void WheelModifierViewModel::setPageModifier(Qt::KeyboardModifier modifier) {
        if (m_pageModifier != modifier) {
            m_pageModifier = modifier;
            emit pageModifierChanged(modifier);
        }
    }


}