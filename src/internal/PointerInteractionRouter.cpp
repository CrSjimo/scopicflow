#include "PointerInteractionRouter_p.h"
#include "PointerInteractionRouter_p_p.h"

#include <cmath>

#include <QGuiApplication>
#include <QStyleHints>

#include "DispatchedDragHandler_p_p.h"

namespace sflow {

    PointerInteractionRouterPrivate::PointerInteractionRouterPrivate(PointerInteractionRouter *q) : q_ptr(q) {
    }

    PointerInteractionRouterPrivate *PointerInteractionRouterPrivate::get(PointerInteractionRouter *router) {
        return router->d_func();
    }

    const PointerInteractionRouterPrivate *PointerInteractionRouterPrivate::get(const PointerInteractionRouter *router) {
        return router->d_func();
    }

    bool PointerInteractionRouterPrivate::isPressed() const {
        return state != Idle;
    }

    bool PointerInteractionRouterPrivate::isDragging() const {
        return state == Dragging;
    }

    DispatchedDragHandler *PointerInteractionRouterPrivate::activeDragHandler() const {
        return state == Dragging ? handler.data() : nullptr;
    }

    bool PointerInteractionRouterPrivate::hasHover() const {
        return hoverActive;
    }

    bool PointerInteractionRouterPrivate::press(QObject *newSource, const PointerInputEvent &event, const PointerHit &hit, DispatchedDragHandler *newHandler) {
        Q_Q(PointerInteractionRouter);
        if (!newSource || event.button == PointerInputEvent::NoButton || !hit.valid)
            return false;
        if (state != Idle)
            return false;
        clearClickState();

        state = Pressed;
        source = newSource;
        target = hit.target;
        handler = event.button == PointerInputEvent::PrimaryButton ? newHandler : nullptr;
        if (handler)
            DispatchedDragHandlerPrivate::get(handler)->arm(q);
        pressEvent = event;
        currentEvent = event;
        currentPressHit = hit;
        doubleClickActive = false;
        connectGestureObjects();
        Q_EMIT q->pressedChanged();

        if (handler && handler->activationPolicy() == DispatchedDragHandler::Immediately)
            startDragging();
        return true;
    }

    void PointerInteractionRouterPrivate::move(QObject *currentSource, const PointerInputEvent &event) {
        if (!matchesActivePointer(currentSource, event))
            return;
        currentEvent = event;
        if (state == Pressed && handler && !doubleClickActive) {
            const auto delta = event.position - pressEvent.position;
            const auto distance = std::abs(delta.x()) + std::abs(delta.y());
            if (distance >= QGuiApplication::styleHints()->startDragDistance())
                startDragging();
        }
        if (state == Dragging && handler && handler->isActive())
            DispatchedDragHandlerPrivate::get(handler)->update(event);
    }

    void PointerInteractionRouterPrivate::release(QObject *currentSource, const PointerInputEvent &event) {
        if (!matchesActivePointer(currentSource, event))
            return;

        const bool wasDragging = state == Dragging;
        clickSource = source;
        clickTarget = target;
        clickHit = pressHit();
        clickSuppressed = wasDragging || doubleClickActive;

        if (wasDragging && handler && handler->isActive())
            DispatchedDragHandlerPrivate::get(handler)->finish(event);
        clearGesture(true);
    }

    void PointerInteractionRouterPrivate::cancel(QObject *currentSource) {
        if (currentSource) {
            if (currentSource == source)
                cancelGesture(true);
            return;
        }
        clearHover();
        if (state == Idle) {
            if (clickSource)
                clickSuppressed = true;
            return;
        }
        cancelGesture(true);
    }

    void PointerInteractionRouterPrivate::click(QObject *currentSource, const PointerInputEvent &event, const PointerHit &hit) {
        Q_Q(PointerInteractionRouter);
        if (clickSource && currentSource != clickSource)
            return;
        if (clickSuppressed) {
            clearClickState();
            return;
        }
        auto effectiveHit = clickSource ? clickHit : hit;
        if (clickSource)
            effectiveHit.target = clickTarget;
        if (!effectiveHit.valid) {
            clearClickState();
            return;
        }
        if (event.button == PointerInputEvent::SecondaryButton) {
            Q_EMIT q->contextMenuRequested(event, effectiveHit);
        } else {
            Q_EMIT q->clicked(event, effectiveHit);
        }
        clearClickState();
    }

    void PointerInteractionRouterPrivate::doubleClick(QObject *currentSource, const PointerInputEvent &event, const PointerHit &hit) {
        Q_Q(PointerInteractionRouter);
        if (state == Dragging || (source && currentSource != source))
            return;
        doubleClickActive = true;
        clickSuppressed = true;
        auto effectiveHit = state == Pressed ? pressHit() : hit;
        if (effectiveHit.valid)
            Q_EMIT q->doubleClicked(event, effectiveHit);
    }

    void PointerInteractionRouterPrivate::updateHover(QObject *currentSource, const PointerInputEvent &event, const PointerHit &hit) {
        Q_Q(PointerInteractionRouter);
        if (!currentSource || !hit.valid)
            return;
        if (state != Idle && currentSource != source)
            return;
        const bool changed = !hoverActive || hoverTarget != hit.target || currentHoverHit.hoverRegion != hit.hoverRegion;
        if (changed) {
            PointerHit oldHit;
            const bool hadHover = hoverActive;
            if (hadHover) {
                oldHit = hoverHit();
                disconnectHoverObjects();
            } else {
                hoverActive = true;
            }
            hoverSource = currentSource;
            hoverTarget = hit.target;
            currentHoverHit = hit;
            connectHoverObjects();
            if (hadHover)
                Q_EMIT q->hoverExited(oldHit);
            if (!hoverActive || hoverSource != currentSource || hoverTarget != hit.target)
                return;
            Q_EMIT q->hoverEntered(event, hoverHit());
        } else {
            hoverSource = currentSource;
            currentHoverHit = hit;
        }
        Q_EMIT q->hoverMoved(event, hoverHit());
    }

    void PointerInteractionRouterPrivate::leaveHover(QObject *currentSource) {
        if (currentSource != hoverSource)
            return;
        clearHover();
    }

    void PointerInteractionRouterPrivate::cancelHandler(DispatchedDragHandler *currentHandler) {
        if (!currentHandler || currentHandler != handler)
            return;
        cancelGesture(true);
    }

    bool PointerInteractionRouterPrivate::matchesActivePointer(QObject *currentSource, const PointerInputEvent &event) const {
        return state != Idle && currentSource == source && event.pointerId == pressEvent.pointerId && event.deviceType == pressEvent.deviceType;
    }

    PointerHit PointerInteractionRouterPrivate::pressHit() const {
        auto hit = currentPressHit;
        hit.target = target;
        return hit;
    }

    PointerHit PointerInteractionRouterPrivate::hoverHit() const {
        auto hit = currentHoverHit;
        hit.target = hoverTarget;
        return hit;
    }

    void PointerInteractionRouterPrivate::startDragging() {
        Q_Q(PointerInteractionRouter);
        if (state != Pressed || !handler)
            return;
        state = Dragging;
        Q_EMIT q->draggingChanged();
        Q_EMIT q->activeDragHandlerChanged();
        DispatchedDragHandlerPrivate::get(handler)->begin(q, pressEvent, pressHit());
        if (!handler || !handler->isActive())
            cancelGesture(true);
    }

    void PointerInteractionRouterPrivate::cancelGesture(bool requestUngrab) {
        Q_Q(PointerInteractionRouter);
        if (state == Idle)
            return;
        const auto currentSource = source;
        const bool wasDragging = state == Dragging;
        clickSource = source;
        clickTarget = target;
        clickHit = pressHit();
        clickSuppressed = true;
        if (wasDragging && handler && handler->isActive())
            DispatchedDragHandlerPrivate::get(handler)->abort();
        clearGesture(true);
        if (requestUngrab && currentSource)
            Q_EMIT q->inputCancellationRequested(currentSource);
    }

    void PointerInteractionRouterPrivate::clearGesture(bool keepClickState) {
        Q_Q(PointerInteractionRouter);
        const bool wasPressed = state != Idle;
        const bool wasDragging = state == Dragging;
        disconnectGestureObjects();
        state = Idle;
        source.clear();
        target.clear();
        if (handler && !handler->isActive()) {
            auto *handlerPrivate = DispatchedDragHandlerPrivate::get(handler);
            if (handlerPrivate->router == q)
                handlerPrivate->router.clear();
        }
        handler.clear();
        pressEvent = {};
        currentEvent = {};
        currentPressHit = {};
        doubleClickActive = false;
        if (!keepClickState)
            clearClickState();
        if (wasDragging) {
            Q_EMIT q->draggingChanged();
            Q_EMIT q->activeDragHandlerChanged();
        }
        if (wasPressed)
            Q_EMIT q->pressedChanged();
    }

    void PointerInteractionRouterPrivate::clearClickState() {
        clickSource.clear();
        clickTarget.clear();
        clickHit = {};
        clickSuppressed = false;
    }

    void PointerInteractionRouterPrivate::clearHover() {
        Q_Q(PointerInteractionRouter);
        if (!hoverActive)
            return;
        const auto oldHit = hoverHit();
        disconnectHoverObjects();
        hoverActive = false;
        hoverSource.clear();
        hoverTarget.clear();
        currentHoverHit = {};
        Q_EMIT q->hoverExited(oldHit);
    }

    void PointerInteractionRouterPrivate::connectGestureObjects() {
        Q_Q(PointerInteractionRouter);
        disconnectGestureObjects();
        sourceDestroyedConnection = QObject::connect(source.data(), &QObject::destroyed, q, [this] {
            cancelGesture(false);
        });
        if (target) {
            targetDestroyedConnection = QObject::connect(target.data(), &QObject::destroyed, q, [this] {
                cancelGesture(true);
            });
        }
        if (handler) {
            handlerDestroyedConnection = QObject::connect(handler.data(), &QObject::destroyed, q, [this] {
                cancelGesture(true);
            });
        }
    }

    void PointerInteractionRouterPrivate::disconnectGestureObjects() {
        QObject::disconnect(sourceDestroyedConnection);
        QObject::disconnect(targetDestroyedConnection);
        QObject::disconnect(handlerDestroyedConnection);
        sourceDestroyedConnection = {};
        targetDestroyedConnection = {};
        handlerDestroyedConnection = {};
    }

    void PointerInteractionRouterPrivate::connectHoverObjects() {
        Q_Q(PointerInteractionRouter);
        disconnectHoverObjects();
        hoverSourceDestroyedConnection = QObject::connect(hoverSource.data(), &QObject::destroyed, q, [this] {
            clearHover();
        });
        if (hoverTarget) {
            hoverTargetDestroyedConnection = QObject::connect(hoverTarget.data(), &QObject::destroyed, q, [this] {
                clearHover();
            });
        }
    }

    void PointerInteractionRouterPrivate::disconnectHoverObjects() {
        QObject::disconnect(hoverSourceDestroyedConnection);
        QObject::disconnect(hoverTargetDestroyedConnection);
        hoverSourceDestroyedConnection = {};
        hoverTargetDestroyedConnection = {};
    }

    PointerInteractionRouter::PointerInteractionRouter(QObject *parent) : QObject(parent), d_ptr(new PointerInteractionRouterPrivate(this)) {
    }

    PointerInteractionRouter::~PointerInteractionRouter() {
        Q_D(PointerInteractionRouter);
        d->cancelGesture(false);
        d->clearHover();
    }

    bool PointerInteractionRouter::isPressed() const {
        Q_D(const PointerInteractionRouter);
        return d->isPressed();
    }

    bool PointerInteractionRouter::isDragging() const {
        Q_D(const PointerInteractionRouter);
        return d->isDragging();
    }

    DispatchedDragHandler *PointerInteractionRouter::activeDragHandler() const {
        Q_D(const PointerInteractionRouter);
        return d->activeDragHandler();
    }

    bool PointerInteractionRouter::hasHover() const {
        Q_D(const PointerInteractionRouter);
        return d->hasHover();
    }

    PointerInputEvent PointerInteractionRouter::createPointerEvent(int pointerId, int deviceType, int button, const QPointF &position, const QRectF &surfaceRect, int modifiers) {
        PointerInputEvent event;
        event.pointerId = pointerId;
        event.deviceType = static_cast<PointerInputEvent::DeviceType>(deviceType);
        event.button = static_cast<PointerInputEvent::Button>(button);
        event.position = position;
        event.surfaceRect = surfaceRect;
        event.modifiers = Qt::KeyboardModifiers::fromInt(modifiers);
        return event;
    }

    PointerHit PointerInteractionRouter::createPointerHit(bool valid, QObject *target, const QRectF &targetRect, int hoverRegion, const QVariant &payload) {
        PointerHit hit;
        hit.valid = valid;
        hit.target = target;
        hit.targetRect = targetRect;
        hit.hoverRegion = hoverRegion;
        hit.payload = payload;
        return hit;
    }

    bool PointerInteractionRouter::press(QObject *source, const PointerInputEvent &event, const PointerHit &hit, DispatchedDragHandler *handler) {
        Q_D(PointerInteractionRouter);
        return d->press(source, event, hit, handler);
    }

    void PointerInteractionRouter::move(QObject *source, const PointerInputEvent &event) {
        Q_D(PointerInteractionRouter);
        d->move(source, event);
    }

    void PointerInteractionRouter::release(QObject *source, const PointerInputEvent &event) {
        Q_D(PointerInteractionRouter);
        d->release(source, event);
    }

    void PointerInteractionRouter::cancel(QObject *source) {
        Q_D(PointerInteractionRouter);
        d->cancel(source);
    }

    void PointerInteractionRouter::click(QObject *source, const PointerInputEvent &event, const PointerHit &hit) {
        Q_D(PointerInteractionRouter);
        d->click(source, event, hit);
    }

    void PointerInteractionRouter::doubleClick(QObject *source, const PointerInputEvent &event, const PointerHit &hit) {
        Q_D(PointerInteractionRouter);
        d->doubleClick(source, event, hit);
    }

    void PointerInteractionRouter::updateHover(QObject *source, const PointerInputEvent &event, const PointerHit &hit) {
        Q_D(PointerInteractionRouter);
        d->updateHover(source, event, hit);
    }

    void PointerInteractionRouter::leaveHover(QObject *source) {
        Q_D(PointerInteractionRouter);
        d->leaveHover(source);
    }

}
