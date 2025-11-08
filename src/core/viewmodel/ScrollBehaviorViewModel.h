#ifndef SCOPIC_FLOW_SCROLLBEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_SCROLLBEHAVIORVIEWMODEL_H

#include <QObject>
#include <QHash>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT ScrollBehaviorViewModel : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(Qt::KeyboardModifier alternateAxisModifier READ alternateAxisModifier WRITE setAlternateAxisModifier NOTIFY alternateAxisModifierChanged)
        Q_PROPERTY(Qt::KeyboardModifier zoomModifier READ zoomModifier WRITE setZoomModifier NOTIFY zoomModifierChanged)
        Q_PROPERTY(Qt::KeyboardModifier pageModifier READ pageModifier WRITE setPageModifier NOTIFY pageModifierChanged)
        Q_PROPERTY(bool usePageModifierAsAlternateAxisZoom READ usePageModifierAsAlternateAxisZoom WRITE setUsePageModifierAsAlternateAxisZoom NOTIFY usePageModifierAsAlternateAxisZoomChanged)
        Q_PROPERTY(bool autoScroll READ autoScroll WRITE setAutoScroll NOTIFY autoScrollChanged)
        Q_PROPERTY(ScrollTypes scrollTypes READ scrollTypes WRITE setScrollTypes NOTIFY scrollTypesChanged)
    public:
        explicit ScrollBehaviorViewModel(QObject *parent = nullptr);
        ~ScrollBehaviorViewModel() override;

        Qt::KeyboardModifier alternateAxisModifier() const;
        void setAlternateAxisModifier(Qt::KeyboardModifier modifier);

        Qt::KeyboardModifier zoomModifier() const;
        void setZoomModifier(Qt::KeyboardModifier modifier);

        Qt::KeyboardModifier pageModifier() const;
        void setPageModifier(Qt::KeyboardModifier modifier);

        bool usePageModifierAsAlternateAxisZoom() const;
        void setUsePageModifierAsAlternateAxisZoom(bool usePageModifierAsAlternateAxisZoom);

        Q_INVOKABLE bool isAlternateAxis(Qt::KeyboardModifiers modifiers) const;
        Q_INVOKABLE bool isZoom(Qt::KeyboardModifiers modifiers) const;
        Q_INVOKABLE bool isPage(Qt::KeyboardModifiers modifiers) const;

        bool autoScroll() const;
        void setAutoScroll(bool autoScroll);

        enum ScrollType {
            Wheel = 0x1,
            Pinch = 0x2,
            MiddleButton = 0x4,
            LeftButton = 0x8,
        };
        Q_ENUM(ScrollType)
        Q_DECLARE_FLAGS(ScrollTypes, ScrollType)

        ScrollTypes scrollTypes() const;
        void setScrollTypes(ScrollTypes scrollTypes);

    signals:
        void alternateAxisModifierChanged();
        void zoomModifierChanged();
        void pageModifierChanged();
        void usePageModifierAsAlternateAxisZoomChanged();
        void autoScrollChanged();
        void scrollTypesChanged();

    private:
        Qt::KeyboardModifier m_alternateAxisModifier;
        Qt::KeyboardModifier m_zoomModifier;
        Qt::KeyboardModifier m_pageModifier;
        bool m_usePageModifierAsAlternateAxisZoom;
        bool m_autoScroll;
        ScrollTypes m_scrollTypes;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(ScrollBehaviorViewModel::ScrollTypes)

} // sflow

#endif //SCOPIC_FLOW_SCROLLBEHAVIORVIEWMODEL_H
