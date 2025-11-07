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
        Q_PROPERTY(bool pinchDecomposed READ pinchDecomposed WRITE setPinchDecomposed NOTIFY pinchDecomposedChanged)
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

        bool pinchDecomposed() const;
        void setPinchDecomposed(bool pinchDecomposed);

    signals:
        void alternateAxisModifierChanged(Qt::KeyboardModifier modifier);
        void zoomModifierChanged(Qt::KeyboardModifier modifier);
        void pageModifierChanged(Qt::KeyboardModifier modifier);
        void usePageModifierAsAlternateAxisZoomChanged(bool usePageModifierAsAlternateAxisZoom);
        void autoScrollChanged(bool autoScroll);
        void pinchDecomposedChanged(bool pinchDecomposed);

    private:
        Qt::KeyboardModifier m_alternateAxisModifier;
        Qt::KeyboardModifier m_zoomModifier;
        Qt::KeyboardModifier m_pageModifier;
        bool m_usePageModifierAsAlternateAxisZoom;
        bool m_autoScroll;
        bool m_pinchDecomposed;
    };

} // sflow

#endif //SCOPIC_FLOW_SCROLLBEHAVIORVIEWMODEL_H
