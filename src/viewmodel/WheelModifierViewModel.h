#ifndef SCOPIC_FLOW_WHEELMODIFIERVIEWMODEL_H
#define SCOPIC_FLOW_WHEELMODIFIERVIEWMODEL_H

#include <QObject>
#include <QHash>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT WheelModifierViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(Qt::KeyboardModifier alternateAxisModifier READ alternateAxisModifier WRITE setAlternateAxisModifier NOTIFY alternateAxisModifierChanged)
        Q_PROPERTY(Qt::KeyboardModifier zoomModifier READ zoomModifier WRITE setZoomModifier NOTIFY zoomModifierChanged)
        Q_PROPERTY(Qt::KeyboardModifier pageModifier READ pageModifier WRITE setPageModifier NOTIFY pageModifierChanged)
    public:
        explicit WheelModifierViewModel(QObject *parent = nullptr);
        ~WheelModifierViewModel() override;

        Qt::KeyboardModifier alternateAxisModifier() const;
        void setAlternateAxisModifier(Qt::KeyboardModifier modifier);

        Qt::KeyboardModifier zoomModifier() const;
        void setZoomModifier(Qt::KeyboardModifier modifier);

        Qt::KeyboardModifier pageModifier() const;
        void setPageModifier(Qt::KeyboardModifier modifier);

    signals:
        void alternateAxisModifierChanged(Qt::KeyboardModifier modifier);
        void zoomModifierChanged(Qt::KeyboardModifier modifier);
        void pageModifierChanged(Qt::KeyboardModifier modifier);

    private:
        Qt::KeyboardModifier m_alternateAxisModifier;
        Qt::KeyboardModifier m_zoomModifier;
        Qt::KeyboardModifier m_pageModifier;
    };

} // sflow

#endif //SCOPIC_FLOW_WHEELMODIFIERVIEWMODEL_H
