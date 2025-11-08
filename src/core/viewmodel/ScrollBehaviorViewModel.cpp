#include "ScrollBehaviorViewModel.h"

namespace sflow {
    ScrollBehaviorViewModel::ScrollBehaviorViewModel(QObject *parent) : QObject(parent),
    m_alternateAxisModifier(Qt::AltModifier), m_zoomModifier(Qt::ControlModifier), m_pageModifier(Qt::ShiftModifier),
    m_usePageModifierAsAlternateAxisZoom(false), m_autoScroll(false), m_scrollTypes(Wheel | Pinch | MiddleButton) {
    }
    ScrollBehaviorViewModel::~ScrollBehaviorViewModel() = default;

    Qt::KeyboardModifier ScrollBehaviorViewModel::alternateAxisModifier() const {
        return m_alternateAxisModifier;
    }
    void ScrollBehaviorViewModel::setAlternateAxisModifier(Qt::KeyboardModifier modifier) {
        if (m_alternateAxisModifier != modifier) {
            m_alternateAxisModifier = modifier;
            emit alternateAxisModifierChanged();
        }
    }
    Qt::KeyboardModifier ScrollBehaviorViewModel::zoomModifier() const {
        return m_zoomModifier;
    }
    void ScrollBehaviorViewModel::setZoomModifier(Qt::KeyboardModifier modifier) {
        if (m_zoomModifier != modifier) {
            m_zoomModifier = modifier;
            emit zoomModifierChanged();
        }
    }
    Qt::KeyboardModifier ScrollBehaviorViewModel::pageModifier() const {
        return m_pageModifier;
    }
    void ScrollBehaviorViewModel::setPageModifier(Qt::KeyboardModifier modifier) {
        if (m_pageModifier != modifier) {
            m_pageModifier = modifier;
            emit pageModifierChanged();
        }
    }
    bool ScrollBehaviorViewModel::usePageModifierAsAlternateAxisZoom() const {
        return m_usePageModifierAsAlternateAxisZoom;
    }
    void ScrollBehaviorViewModel::setUsePageModifierAsAlternateAxisZoom(bool usePageModifierAsAlternateAxisZoom) {
        if(m_usePageModifierAsAlternateAxisZoom != usePageModifierAsAlternateAxisZoom) {
            m_usePageModifierAsAlternateAxisZoom = usePageModifierAsAlternateAxisZoom;
            emit usePageModifierAsAlternateAxisZoomChanged();
        }
    }

    bool ScrollBehaviorViewModel::isAlternateAxis(Qt::KeyboardModifiers modifiers) const {
        // FIXME
        auto isWindows =
#ifdef Q_OS_WIN
            true
#else
            false
#endif;
        ;
        auto isAxisRevert = isWindows && (modifiers & Qt::AltModifier);
        auto isRealAlternateAxis = (modifiers & m_alternateAxisModifier) || (modifiers & m_pageModifier) && m_usePageModifierAsAlternateAxisZoom;
        return isRealAlternateAxis != isAxisRevert;
    }

    bool ScrollBehaviorViewModel::isZoom(Qt::KeyboardModifiers modifiers) const {
        return (modifiers & m_zoomModifier) || (modifiers & m_pageModifier) && m_usePageModifierAsAlternateAxisZoom;
    }

    bool ScrollBehaviorViewModel::isPage(Qt::KeyboardModifiers modifiers) const {
        return (modifiers & m_pageModifier) && !m_usePageModifierAsAlternateAxisZoom;
    }

    bool ScrollBehaviorViewModel::autoScroll() const {
        return m_autoScroll;
    }
    void ScrollBehaviorViewModel::setAutoScroll(bool autoScroll) {
        if (m_autoScroll != autoScroll) {
            m_autoScroll = autoScroll;
            emit autoScrollChanged();
        }
    }
    ScrollBehaviorViewModel::ScrollTypes ScrollBehaviorViewModel::scrollTypes() const {
        return m_scrollTypes;
    }
    void ScrollBehaviorViewModel::setScrollTypes(ScrollTypes scrollTypes) {
        if (m_scrollTypes != scrollTypes) {
            m_scrollTypes = scrollTypes;
            emit scrollTypesChanged();
        }
    }

}
