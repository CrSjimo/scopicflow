#include "ScrollBehaviorViewModel.h"

namespace sflow {
    ScrollBehaviorViewModel::ScrollBehaviorViewModel(QObject *parent) : QObject(parent),
    m_alternateAxisModifier(Qt::AltModifier), m_zoomModifier(Qt::ControlModifier), m_pageModifier(Qt::ShiftModifier),
    m_usePageModifierAsAlternateAxisZoom(false), m_affectVelocity(false) {
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
    bool ScrollBehaviorViewModel::usePageModifierAsAlternateAxisZoom() const {
        return m_usePageModifierAsAlternateAxisZoom;
    }
    void ScrollBehaviorViewModel::setUsePageModifierAsAlternateAxisZoom(bool usePageModifierAsAlternateAxisZoom) {
        if(m_usePageModifierAsAlternateAxisZoom != usePageModifierAsAlternateAxisZoom) {
            m_usePageModifierAsAlternateAxisZoom = usePageModifierAsAlternateAxisZoom;
            emit usePageModifierAsAlternateAxisZoomChanged(usePageModifierAsAlternateAxisZoom);
        }
    }
    bool ScrollBehaviorViewModel::affectVelocity() const {
        return m_affectVelocity;
    }
    void ScrollBehaviorViewModel::setAffectVelocity(bool affectVelocity) {
        if (m_affectVelocity != affectVelocity) {
            m_affectVelocity = affectVelocity;
            emit affectVelocityChanged(affectVelocity);
        }
    }


}