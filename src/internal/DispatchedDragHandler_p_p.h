#ifndef SCOPIC_FLOW_DISPATCHEDDRAGHANDLER_P_P_H
#define SCOPIC_FLOW_DISPATCHEDDRAGHANDLER_P_P_H

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>

#include <QList>
#include <QPointer>

namespace sflow {

    class PointerInteractionRouter;

    class DispatchedDragHandlerPrivate {
        Q_DECLARE_PUBLIC(DispatchedDragHandler)

    public:
        explicit DispatchedDragHandlerPrivate(DispatchedDragHandler *q);

        static DispatchedDragHandlerPrivate *get(DispatchedDragHandler *handler);
        static const DispatchedDragHandlerPrivate *get(const DispatchedDragHandler *handler);

        DispatchedDragHandler::ActivationPolicy activationPolicy() const;
        void setActivationPolicy(DispatchedDragHandler::ActivationPolicy activationPolicy);
        bool isActive() const;
        QPointF pressPosition() const;
        QPointF currentPosition() const;
        Qt::KeyboardModifiers pressModifiers() const;
        PointerHit hit() const;
        QRectF surfaceRect() const;
        QQmlListProperty<QObject> data();

        void cancel();
        void arm(PointerInteractionRouter *router);
        void begin(PointerInteractionRouter *router, const PointerInputEvent &event, const PointerHit &hit);
        void update(const PointerInputEvent &event);
        void finish(const PointerInputEvent &event);
        void abort();
        void reset();

        static void appendData(QQmlListProperty<QObject> *property, QObject *object);
        static qsizetype dataCount(QQmlListProperty<QObject> *property);
        static QObject *dataAt(QQmlListProperty<QObject> *property, qsizetype index);
        static void clearData(QQmlListProperty<QObject> *property);

        DispatchedDragHandler *q_ptr = nullptr;
        DispatchedDragHandler::ActivationPolicy activationPolicyValue = DispatchedDragHandler::AfterThreshold;
        bool active = false;
        QPointer<PointerInteractionRouter> router;
        PointerInputEvent pressEvent;
        PointerInputEvent currentEvent;
        PointerHit currentHit;
        QPointer<QObject> target;
        QList<QPointer<QObject>> childData;
    };

}

#endif // SCOPIC_FLOW_DISPATCHEDDRAGHANDLER_P_P_H
