#include "DispatchedDragHandler_p.h"
#include "DispatchedDragHandler_p_p.h"

#include <ScopicFlowInternal/private/PointerInteractionRouter_p_p.h>

namespace sflow {

    DispatchedDragHandlerPrivate::DispatchedDragHandlerPrivate(DispatchedDragHandler *q) : q_ptr(q) {
    }

    DispatchedDragHandlerPrivate *DispatchedDragHandlerPrivate::get(DispatchedDragHandler *handler) {
        return handler->d_func();
    }

    const DispatchedDragHandlerPrivate *DispatchedDragHandlerPrivate::get(const DispatchedDragHandler *handler) {
        return handler->d_func();
    }

    DispatchedDragHandler::ActivationPolicy DispatchedDragHandlerPrivate::activationPolicy() const {
        return activationPolicyValue;
    }

    void DispatchedDragHandlerPrivate::setActivationPolicy(DispatchedDragHandler::ActivationPolicy activationPolicy) {
        Q_Q(DispatchedDragHandler);
        if (activationPolicyValue == activationPolicy)
            return;
        activationPolicyValue = activationPolicy;
        Q_EMIT q->activationPolicyChanged();
    }

    bool DispatchedDragHandlerPrivate::isActive() const {
        return active;
    }

    QPointF DispatchedDragHandlerPrivate::pressPosition() const {
        return pressEvent.position;
    }

    QPointF DispatchedDragHandlerPrivate::currentPosition() const {
        return currentEvent.position;
    }

    Qt::KeyboardModifiers DispatchedDragHandlerPrivate::pressModifiers() const {
        return pressEvent.modifiers;
    }

    PointerHit DispatchedDragHandlerPrivate::hit() const {
        auto hit = currentHit;
        hit.target = target;
        return hit;
    }

    QRectF DispatchedDragHandlerPrivate::surfaceRect() const {
        return currentEvent.surfaceRect;
    }

    QQmlListProperty<QObject> DispatchedDragHandlerPrivate::data() {
        Q_Q(DispatchedDragHandler);
        return {q, this, appendData, dataCount, dataAt, clearData};
    }

    void DispatchedDragHandlerPrivate::cancel() {
        Q_Q(DispatchedDragHandler);
        if (router) {
            PointerInteractionRouterPrivate::get(router)->cancelHandler(q);
        } else if (active) {
            abort();
        }
    }

    void DispatchedDragHandlerPrivate::arm(PointerInteractionRouter *newRouter) {
        if (router == newRouter)
            return;
        cancel();
        router = newRouter;
    }

    void DispatchedDragHandlerPrivate::begin(PointerInteractionRouter *newRouter, const PointerInputEvent &event, const PointerHit &hit) {
        Q_Q(DispatchedDragHandler);
        if (active)
            abort();
        router = newRouter;
        pressEvent = event;
        currentEvent = event;
        currentHit = hit;
        target = hit.target;
        active = true;
        Q_EMIT q->stateChanged();
        Q_EMIT q->activeChanged();
        Q_EMIT q->started(pressEvent, this->hit());
    }

    void DispatchedDragHandlerPrivate::update(const PointerInputEvent &event) {
        Q_Q(DispatchedDragHandler);
        if (!active)
            return;
        currentEvent = event;
        Q_EMIT q->stateChanged();
        Q_EMIT q->moved(currentEvent, hit());
    }

    void DispatchedDragHandlerPrivate::finish(const PointerInputEvent &event) {
        Q_Q(DispatchedDragHandler);
        if (!active)
            return;
        currentEvent = event;
        const auto currentPointerHit = hit();
        active = false;
        Q_EMIT q->stateChanged();
        Q_EMIT q->activeChanged();
        Q_EMIT q->finished(currentEvent, currentPointerHit);
        reset();
    }

    void DispatchedDragHandlerPrivate::abort() {
        Q_Q(DispatchedDragHandler);
        if (!active)
            return;
        const auto currentPointerHit = hit();
        active = false;
        Q_EMIT q->stateChanged();
        Q_EMIT q->activeChanged();
        Q_EMIT q->canceled(currentPointerHit);
        reset();
    }

    void DispatchedDragHandlerPrivate::reset() {
        Q_Q(DispatchedDragHandler);
        router.clear();
        pressEvent = {};
        currentEvent = {};
        currentHit = {};
        target.clear();
        Q_EMIT q->stateChanged();
    }

    void DispatchedDragHandlerPrivate::appendData(QQmlListProperty<QObject> *property, QObject *object) {
        auto *d = static_cast<DispatchedDragHandlerPrivate *>(property->data);
        if (!object || d->childData.contains(QPointer<QObject>(object)))
            return;
        if (!object->parent())
            object->setParent(property->object);
        d->childData.append(object);
    }

    qsizetype DispatchedDragHandlerPrivate::dataCount(QQmlListProperty<QObject> *property) {
        return static_cast<DispatchedDragHandlerPrivate *>(property->data)->childData.size();
    }

    QObject *DispatchedDragHandlerPrivate::dataAt(QQmlListProperty<QObject> *property, qsizetype index) {
        return static_cast<DispatchedDragHandlerPrivate *>(property->data)->childData.value(index).data();
    }

    void DispatchedDragHandlerPrivate::clearData(QQmlListProperty<QObject> *property) {
        static_cast<DispatchedDragHandlerPrivate *>(property->data)->childData.clear();
    }

    DispatchedDragHandler::DispatchedDragHandler(QObject *parent) : QObject(parent), d_ptr(new DispatchedDragHandlerPrivate(this)) {
    }

    DispatchedDragHandler::~DispatchedDragHandler() {
        Q_D(DispatchedDragHandler);
        d->abort();
    }

    DispatchedDragHandler::ActivationPolicy DispatchedDragHandler::activationPolicy() const {
        Q_D(const DispatchedDragHandler);
        return d->activationPolicy();
    }

    void DispatchedDragHandler::setActivationPolicy(ActivationPolicy activationPolicy) {
        Q_D(DispatchedDragHandler);
        d->setActivationPolicy(activationPolicy);
    }

    bool DispatchedDragHandler::isActive() const {
        Q_D(const DispatchedDragHandler);
        return d->isActive();
    }

    QPointF DispatchedDragHandler::pressPosition() const {
        Q_D(const DispatchedDragHandler);
        return d->pressPosition();
    }

    QPointF DispatchedDragHandler::currentPosition() const {
        Q_D(const DispatchedDragHandler);
        return d->currentPosition();
    }

    Qt::KeyboardModifiers DispatchedDragHandler::pressModifiers() const {
        Q_D(const DispatchedDragHandler);
        return d->pressModifiers();
    }

    PointerHit DispatchedDragHandler::hit() const {
        Q_D(const DispatchedDragHandler);
        return d->hit();
    }

    QRectF DispatchedDragHandler::surfaceRect() const {
        Q_D(const DispatchedDragHandler);
        return d->surfaceRect();
    }

    QQmlListProperty<QObject> DispatchedDragHandler::data() {
        Q_D(DispatchedDragHandler);
        return d->data();
    }

    void DispatchedDragHandler::cancel() {
        Q_D(DispatchedDragHandler);
        d->cancel();
    }

}
