#ifndef SCOPIC_FLOW_DISPATCHEDDRAGHANDLER_P_H
#define SCOPIC_FLOW_DISPATCHEDDRAGHANDLER_P_H

#include <QObject>
#include <QQmlListProperty>
#include <QScopedPointer>
#include <qqmlintegration.h>

#include <ScopicFlowInternal/private/PointerInteractionTypes_p.h>

namespace sflow {

    class DispatchedDragHandlerPrivate;

    class DispatchedDragHandler : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(DispatchedDragHandler)
        Q_CLASSINFO("DefaultProperty", "data")
        Q_PROPERTY(QQmlListProperty<QObject> data READ data)
        Q_PROPERTY(ActivationPolicy activationPolicy READ activationPolicy WRITE setActivationPolicy NOTIFY activationPolicyChanged)
        Q_PROPERTY(bool active READ isActive NOTIFY activeChanged)
        Q_PROPERTY(QPointF pressPosition READ pressPosition NOTIFY stateChanged)
        Q_PROPERTY(QPointF currentPosition READ currentPosition NOTIFY stateChanged)
        Q_PROPERTY(Qt::KeyboardModifiers pressModifiers READ pressModifiers NOTIFY stateChanged)
        Q_PROPERTY(PointerHit hit READ hit NOTIFY stateChanged)
        Q_PROPERTY(QRectF surfaceRect READ surfaceRect NOTIFY stateChanged)

    public:
        enum ActivationPolicy {
            AfterThreshold,
            Immediately,
        };
        Q_ENUM(ActivationPolicy)

        explicit DispatchedDragHandler(QObject *parent = nullptr);
        ~DispatchedDragHandler() override;

        ActivationPolicy activationPolicy() const;
        void setActivationPolicy(ActivationPolicy activationPolicy);

        bool isActive() const;
        QPointF pressPosition() const;
        QPointF currentPosition() const;
        Qt::KeyboardModifiers pressModifiers() const;
        PointerHit hit() const;
        QRectF surfaceRect() const;
        QQmlListProperty<QObject> data();

        Q_INVOKABLE void cancel();

    Q_SIGNALS:
        void activationPolicyChanged();
        void activeChanged();
        void stateChanged();

        void started(const PointerInputEvent &event, const PointerHit &hit);
        void moved(const PointerInputEvent &event, const PointerHit &hit);
        void finished(const PointerInputEvent &event, const PointerHit &hit);
        void canceled(const PointerHit &hit);

    private:
        QScopedPointer<DispatchedDragHandlerPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_DISPATCHEDDRAGHANDLER_P_H
