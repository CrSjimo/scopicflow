#include "ScrollBehaviorViewModel.h"

namespace sflow {
    ScrollBehaviorViewModel::ScrollBehaviorViewModel(QObject *parent) : QObject(parent), m_alternateAxisModifier(Qt::AltModifier), m_zoomModifier(Qt::ControlModifier), m_pageModifier(Qt::ShiftModifier) {
    }
    ScrollBehaviorViewModel::~ScrollBehaviorViewModel() = default;

    Qt::KeyboardModifier ScrollBehaviorViewModel::alternateAxisModifier() const {
        return m_alternateAxisModifier;
    }
    void ScrollBehaviorViewModel::setAlternateAxisModifier(Qt::KeyboardModifier modifier) {
        if (m_alternateAxisModifier != modifier) {
            m_alternateAxisModifier = modifier;
            emit alternateAxisModifierChanged(modifier);
        }
    }
    Qt::KeyboardModifier ScrollBehaviorViewModel::zoomModifier() const {
        return m_zoomModifier;
    }
    void ScrollBehaviorViewModel::setZoomModifier(Qt::KeyboardModifier modifier) {
        if (m_zoomModifier != modifier) {
            m_zoomModifier = modifier;
            emit zoomModifierChanged(modifier);
        }
    }
    Qt::KeyboardModifier ScrollBehaviorViewModel::pageModifier() const {
        return m_pageModifier;
    }
    void ScrollBehaviorViewModel::setPageModifier(Qt::KeyboardModifier modifier) {
        if (m_pageModifier != modifier) {
            m_pageModifier = modifier;
            emit pageModifierChanged(modifier);
        }
    }


}