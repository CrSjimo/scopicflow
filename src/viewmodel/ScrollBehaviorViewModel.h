#ifndef SCOPIC_FLOW_SCROLLBEHAVIORVIEWMODEL_H
#define SCOPIC_FLOW_SCROLLBEHAVIORVIEWMODEL_H

#include <QObject>
#include <QHash>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT ScrollBehaviorViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(Qt::KeyboardModifier alternateAxisModifier READ alternateAxisModifier WRITE setAlternateAxisModifier NOTIFY alternateAxisModifierChanged)
        Q_PROPERTY(Qt::KeyboardModifier zoomModifier READ zoomModifier WRITE setZoomModifier NOTIFY zoomModifierChanged)
        Q_PROPERTY(Qt::KeyboardModifier pageModifier READ pageModifier WRITE setPageModifier NOTIFY pageModifierChanged)
        Q_PROPERTY(bool usePageModifierAsAlternateAxisZoom READ usePageModifierAsAlternateAxisZoom WRITE setUsePageModifierAsAlternateAxisZoom NOTIFY usePageModifierAsAlternateAxisZoomChanged)
        Q_PROPERTY(bool affectVelocity READ affectVelocity WRITE setAffectVelocity NOTIFY affectVelocityChanged)
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

        bool affectVelocity() const;
        void setAffectVelocity(bool affectVelocity);

        bool pinchDecomposed() const;
        void setPinchDecomposed(bool pinchDecomposed);

    signals:
        void alternateAxisModifierChanged(Qt::KeyboardModifier modifier);
        void zoomModifierChanged(Qt::KeyboardModifier modifier);
        void pageModifierChanged(Qt::KeyboardModifier modifier);
        void usePageModifierAsAlternateAxisZoomChanged(bool usePageModifierAsAlternateAxisZoom);
        void affectVelocityChanged(bool affectVeloCity);
        void pinchDecomposedChanged(bool pinchDecomposed);

    private:
        Qt::KeyboardModifier m_alternateAxisModifier;
        Qt::KeyboardModifier m_zoomModifier;
        Qt::KeyboardModifier m_pageModifier;
        bool m_usePageModifierAsAlternateAxisZoom;
        bool m_affectVelocity;
        bool m_pinchDecomposed;
    };

} // sflow

#endif //SCOPIC_FLOW_SCROLLBEHAVIORVIEWMODEL_H
