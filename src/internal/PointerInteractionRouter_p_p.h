#ifndef SCOPIC_FLOW_POINTERINTERACTIONROUTER_P_P_H
#define SCOPIC_FLOW_POINTERINTERACTIONROUTER_P_P_H

#include <QPointer>

#include "PointerInteractionRouter_p.h"

namespace sflow {

    class PointerInteractionRouterPrivate {
        Q_DECLARE_PUBLIC(PointerInteractionRouter)

    public:
        enum State {
            Idle,
            Pressed,
            Dragging,
        };

        explicit PointerInteractionRouterPrivate(PointerInteractionRouter *q);

        static PointerInteractionRouterPrivate *get(PointerInteractionRouter *router);
        static const PointerInteractionRouterPrivate *get(const PointerInteractionRouter *router);

        bool isPressed() const;
        bool isDragging() const;
        DispatchedDragHandler *activeDragHandler() const;
        bool hasHover() const;

        bool press(QObject *source, const PointerInputEvent &event, const PointerHit &hit, DispatchedDragHandler *handler);
        void move(QObject *source, const PointerInputEvent &event);
        void release(QObject *source, const PointerInputEvent &event);
        void cancel(QObject *source);
        void click(QObject *source, const PointerInputEvent &event, const PointerHit &hit);
        void doubleClick(QObject *source, const PointerInputEvent &event, const PointerHit &hit);
        void updateHover(QObject *source, const PointerInputEvent &event, const PointerHit &hit);
        void leaveHover(QObject *source);
        void cancelHandler(DispatchedDragHandler *handler);

        bool matchesActivePointer(QObject *source, const PointerInputEvent &event) const;
        PointerHit pressHit() const;
        PointerHit hoverHit() const;
        void startDragging();
        void cancelGesture(bool requestUngrab);
        void clearGesture(bool keepClickState = false);
        void clearClickState();
        void clearHover();
        void connectGestureObjects();
        void disconnectGestureObjects();
        void connectHoverObjects();
        void disconnectHoverObjects();

        PointerInteractionRouter *q_ptr = nullptr;
        State state = Idle;
        QPointer<QObject> source;
        QPointer<QObject> target;
        QPointer<DispatchedDragHandler> handler;
        PointerInputEvent pressEvent;
        PointerInputEvent currentEvent;
        PointerHit currentPressHit;
        bool doubleClickActive = false;

        QPointer<QObject> clickSource;
        QPointer<QObject> clickTarget;
        PointerHit clickHit;
        bool clickSuppressed = false;

        QPointer<QObject> hoverSource;
        QPointer<QObject> hoverTarget;
        PointerHit currentHoverHit;
        bool hoverActive = false;

        QMetaObject::Connection sourceDestroyedConnection;
        QMetaObject::Connection targetDestroyedConnection;
        QMetaObject::Connection handlerDestroyedConnection;
        QMetaObject::Connection hoverSourceDestroyedConnection;
        QMetaObject::Connection hoverTargetDestroyedConnection;
    };

}

#endif // SCOPIC_FLOW_POINTERINTERACTIONROUTER_P_P_H
