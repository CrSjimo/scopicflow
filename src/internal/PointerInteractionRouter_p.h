#ifndef SCOPIC_FLOW_POINTERINTERACTIONROUTER_P_H
#define SCOPIC_FLOW_POINTERINTERACTIONROUTER_P_H

#include <QObject>
#include <QScopedPointer>
#include <qqmlintegration.h>

#include "PointerInteractionTypes_p.h"

namespace sflow {

    class DispatchedDragHandler;
    class PointerInteractionRouterPrivate;

    class PointerInteractionRouter : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(PointerInteractionRouter)
        Q_PROPERTY(bool pressed READ isPressed NOTIFY pressedChanged)
        Q_PROPERTY(bool dragging READ isDragging NOTIFY draggingChanged)
        Q_PROPERTY(DispatchedDragHandler *activeDragHandler READ activeDragHandler NOTIFY activeDragHandlerChanged)

    public:
        explicit PointerInteractionRouter(QObject *parent = nullptr);
        ~PointerInteractionRouter() override;

        bool isPressed() const;
        bool isDragging() const;
        DispatchedDragHandler *activeDragHandler() const;
        Q_INVOKABLE bool hasHover() const;

        Q_INVOKABLE static PointerInputEvent createPointerEvent(int pointerId, int deviceType, int button, const QPointF &position, const QRectF &surfaceRect, int modifiers);
        Q_INVOKABLE static PointerHit createPointerHit(bool valid, QObject *target, const QRectF &targetRect, int hoverRegion, const QVariant &payload);
        Q_INVOKABLE bool press(QObject *source, const PointerInputEvent &event, const PointerHit &hit, DispatchedDragHandler *handler);
        Q_INVOKABLE void move(QObject *source, const PointerInputEvent &event);
        Q_INVOKABLE void release(QObject *source, const PointerInputEvent &event);
        Q_INVOKABLE void cancel(QObject *source = nullptr);
        Q_INVOKABLE void click(QObject *source, const PointerInputEvent &event, const PointerHit &hit);
        Q_INVOKABLE void doubleClick(QObject *source, const PointerInputEvent &event, const PointerHit &hit);
        Q_INVOKABLE void updateHover(QObject *source, const PointerInputEvent &event, const PointerHit &hit);
        Q_INVOKABLE void leaveHover(QObject *source);

    Q_SIGNALS:
        void pressedChanged();
        void draggingChanged();
        void activeDragHandlerChanged();
        void inputCancellationRequested(QObject *source);

        void clicked(const PointerInputEvent &event, const PointerHit &hit);
        void doubleClicked(const PointerInputEvent &event, const PointerHit &hit);
        void contextMenuRequested(const PointerInputEvent &event, const PointerHit &hit);
        void hoverEntered(const PointerInputEvent &event, const PointerHit &hit);
        void hoverMoved(const PointerInputEvent &event, const PointerHit &hit);
        void hoverExited(const PointerHit &hit);

    private:
        QScopedPointer<PointerInteractionRouterPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_POINTERINTERACTIONROUTER_P_H
