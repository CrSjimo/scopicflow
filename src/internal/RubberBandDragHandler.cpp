#include "RubberBandDragHandler_p.h"
#include "RubberBandDragHandler_p_p.h"

#include <QQuickItem>
#include <QQmlInfo>

#include <ScopicFlowCore/SelectionController.h>
#include <ScopicFlowCore/TimeManipulator.h>

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>
#include <ScopicFlowInternal/private/DragScroller_p.h>
#include <ScopicFlowInternal/private/InvokeMethodHelper_p.h>
#include <ScopicFlowInternal/private/RubberBandLayerQuickItem_p.h>
#include <ScopicFlowInternal/private/RubberBandSelector_p.h>

namespace sflow {

    namespace {

        // Meta-object member names resolved on the controller and manipulator objects
        constexpr char controllerRubberBandDraggingStartedMethodName[] = "rubberBandDraggingStarted";
        constexpr char controllerRubberBandDraggingCommittedMethodName[] = "rubberBandDraggingCommitted";
        constexpr char controllerRubberBandDraggingAbortedMethodName[] = "rubberBandDraggingAborted";
        constexpr char verticalManipulatorMoveViewByMethodName[] = "moveViewBy";

    }

    RubberBandDragHandlerPrivate::RubberBandDragHandlerPrivate(RubberBandDragHandler *q) : q_ptr(q) {
    }

    void RubberBandDragHandlerPrivate::setController(QObject *newController) {
        Q_Q(RubberBandDragHandler);
        if (controller == newController)
            return;
        controller = newController;
        controllerRubberBandDraggingStartedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *>(
            q, controller, controllerRubberBandDraggingStartedMethodName);
        controllerRubberBandDraggingCommittedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *>(
            q, controller, controllerRubberBandDraggingCommittedMethodName);
        controllerRubberBandDraggingAbortedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *>(
            q, controller, controllerRubberBandDraggingAbortedMethodName);
        Q_EMIT q->controllerChanged();
    }

    void RubberBandDragHandlerPrivate::setVerticalManipulator(QObject *newVerticalManipulator) {
        Q_Q(RubberBandDragHandler);
        if (verticalManipulator == newVerticalManipulator)
            return;
        verticalManipulator = newVerticalManipulator;
        verticalManipulatorMoveViewByMethod = InvokeMethodHelper::resolveMethod<double, bool>(
            q, verticalManipulator, verticalManipulatorMoveViewByMethodName);
        Q_EMIT q->verticalManipulatorChanged();
    }

    void RubberBandDragHandlerPrivate::onStarted(const PointerInputEvent &event) {
        Q_Q(RubberBandDragHandler);
        if (!controller || !rubberBandLayer) {
            q->cancel();
            return;
        }
        InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerRubberBandDraggingStartedMethod,
                                                target.data());
        if (selectionController)
            selectionController->selectByPointer(nullptr, SelectionController::PrimarySelection, event.modifiers);
        lastTargetPoint = event.position;
        const QPointF point = mapToRubberBand(event.position);
        if (mode == RubberBandDragHandler::Normal) {
            rubberBandLayer->startSelection(point);
        } else {
            rubberBandLayer->startSelection(QPointF(point.x(), 0.0));
        }
    }

    void RubberBandDragHandlerPrivate::onMoved(const PointerInputEvent &event) {
        const QPointF point = event.position;
        dragScroller->determine(point.x(), event.surfaceRect.width(), point.y(), event.surfaceRect.height(),
                                QJSValue());
        if (dragScroller->distanceX() == 0)
            doDragRubberBand(QPointF(point.x(), lastTargetPoint.y()));
        if (dragScroller->distanceY() == 0)
            doDragRubberBand(QPointF(lastTargetPoint.x(), point.y()));
    }

    void RubberBandDragHandlerPrivate::onFinished() {
        Q_Q(RubberBandDragHandler);
        if (rubberBandLayer) {
            const QRectF rect = rubberBandLayer->endSelection();
            if (selector)
                selector->select(q->mapToSelector(rect));
            else
                qmlWarning(q) << "RubberBandDragHandler: selector is not set";
        }
        dragScroller->setRunning(false);
        InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerRubberBandDraggingCommittedMethod,
                                                target.data());
    }

    void RubberBandDragHandlerPrivate::onCanceled() {
        Q_Q(RubberBandDragHandler);
        if (rubberBandLayer)
            rubberBandLayer->endSelection();
        dragScroller->setRunning(false);
        if (controller) {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerRubberBandDraggingAbortedMethod,
                                                    target.data());
        }
    }

    void RubberBandDragHandlerPrivate::onDragScrollerMoved(double deltaX, double deltaY) {
        Q_Q(RubberBandDragHandler);
        if (timeManipulator)
            timeManipulator->moveViewBy(deltaX);
        if (verticalManipulator) {
            InvokeMethodHelper::invokeWithoutReturn(q, verticalManipulator.data(), verticalManipulatorMoveViewByMethod,
                                                    deltaY, false);
        }
        const QRectF surfaceRect = q->surfaceRect();
        if (deltaX != 0)
            doDragRubberBand(QPointF(deltaX > 0 ? surfaceRect.width() : 0, lastTargetPoint.y()));
        if (deltaY != 0)
            doDragRubberBand(QPointF(lastTargetPoint.x(), deltaY > 0 ? surfaceRect.height() : 0));
    }

    QPointF RubberBandDragHandlerPrivate::mapToRubberBand(const QPointF &point) const {
        Q_Q(const RubberBandDragHandler);
        const auto coordinateSpace = q->coordinateSpace();
        if (!coordinateSpace || !rubberBandLayer) {
            qmlWarning(q) << "RubberBandDragHandler: coordinateSpace or rubberBandLayer is not set";
            return point;
        }
        return coordinateSpace->mapToItem(rubberBandLayer, point);
    }

    void RubberBandDragHandlerPrivate::doDragRubberBand(const QPointF &targetPoint) {
        if (!rubberBandLayer)
            return;
        const QPointF point = mapToRubberBand(targetPoint);
        if (mode == RubberBandDragHandler::Normal) {
            rubberBandLayer->updateSelection(point);
        } else {
            rubberBandLayer->updateSelection(QPointF(point.x(), rubberBandLayer->height()));
        }
        lastTargetPoint = targetPoint;
    }

    RubberBandDragHandler::RubberBandDragHandler(QObject *parent)
        : DispatchedDragHandler(parent), d_ptr(new RubberBandDragHandlerPrivate(this)) {
        Q_D(RubberBandDragHandler);
        d->dragScroller = new DragScroller(this);
        connect(d->dragScroller, &DragScroller::moved, this, [d](double deltaX, double deltaY) {
            d->onDragScrollerMoved(deltaX, deltaY);
        });
        connect(this, &DispatchedDragHandler::started, this, [d](const PointerInputEvent &event, const PointerHit &) {
            d->onStarted(event);
        });
        connect(this, &DispatchedDragHandler::moved, this, [d](const PointerInputEvent &event, const PointerHit &) {
            d->onMoved(event);
        });
        connect(this, &DispatchedDragHandler::finished, this, [d] {
            d->onFinished();
        });
        connect(this, &DispatchedDragHandler::canceled, this, [d] {
            d->onCanceled();
        });
    }

    RubberBandDragHandler::~RubberBandDragHandler() = default;

    QObject *RubberBandDragHandler::controller() const {
        Q_D(const RubberBandDragHandler);
        return d->controller;
    }

    void RubberBandDragHandler::setController(QObject *controller) {
        Q_D(RubberBandDragHandler);
        d->setController(controller);
    }

    TimeManipulator *RubberBandDragHandler::timeManipulator() const {
        Q_D(const RubberBandDragHandler);
        return d->timeManipulator;
    }

    void RubberBandDragHandler::setTimeManipulator(TimeManipulator *timeManipulator) {
        Q_D(RubberBandDragHandler);
        if (d->timeManipulator == timeManipulator)
            return;
        d->timeManipulator = timeManipulator;
        Q_EMIT timeManipulatorChanged();
    }

    QObject *RubberBandDragHandler::verticalManipulator() const {
        Q_D(const RubberBandDragHandler);
        return d->verticalManipulator;
    }

    void RubberBandDragHandler::setVerticalManipulator(QObject *verticalManipulator) {
        Q_D(RubberBandDragHandler);
        d->setVerticalManipulator(verticalManipulator);
    }

    RubberBandLayerQuickItem *RubberBandDragHandler::rubberBandLayer() const {
        Q_D(const RubberBandDragHandler);
        return d->rubberBandLayer;
    }

    void RubberBandDragHandler::setRubberBandLayer(RubberBandLayerQuickItem *rubberBandLayer) {
        Q_D(RubberBandDragHandler);
        if (d->rubberBandLayer == rubberBandLayer)
            return;
        d->rubberBandLayer = rubberBandLayer;
        Q_EMIT rubberBandLayerChanged();
    }

    RubberBandSelector *RubberBandDragHandler::selector() const {
        Q_D(const RubberBandDragHandler);
        return d->selector;
    }

    void RubberBandDragHandler::setSelector(RubberBandSelector *selector) {
        Q_D(RubberBandDragHandler);
        if (d->selector == selector)
            return;
        d->selector = selector;
        Q_EMIT selectorChanged();
    }

    SelectionController *RubberBandDragHandler::selectionController() const {
        Q_D(const RubberBandDragHandler);
        return d->selectionController;
    }

    void RubberBandDragHandler::setSelectionController(SelectionController *selectionController) {
        Q_D(RubberBandDragHandler);
        if (d->selectionController == selectionController)
            return;
        d->selectionController = selectionController;
        Q_EMIT selectionControllerChanged();
    }

    QQuickItem *RubberBandDragHandler::target() const {
        Q_D(const RubberBandDragHandler);
        return d->target;
    }

    void RubberBandDragHandler::setTarget(QQuickItem *target) {
        Q_D(RubberBandDragHandler);
        if (d->target == target)
            return;
        d->target = target;
        Q_EMIT targetChanged();
        if (!d->coordinateSpace)
            Q_EMIT coordinateSpaceChanged();
    }

    QQuickItem *RubberBandDragHandler::coordinateSpace() const {
        Q_D(const RubberBandDragHandler);
        return d->coordinateSpace ? d->coordinateSpace.data() : d->target.data();
    }

    void RubberBandDragHandler::setCoordinateSpace(QQuickItem *coordinateSpace) {
        Q_D(RubberBandDragHandler);
        if (d->coordinateSpace == coordinateSpace)
            return;
        d->coordinateSpace = coordinateSpace;
        Q_EMIT coordinateSpaceChanged();
    }

    int RubberBandDragHandler::mode() const {
        Q_D(const RubberBandDragHandler);
        return d->mode;
    }

    void RubberBandDragHandler::setMode(int mode) {
        Q_D(RubberBandDragHandler);
        if (d->mode == mode)
            return;
        d->mode = mode;
        Q_EMIT modeChanged();
    }

    QPointF RubberBandDragHandler::mapToRubberBand(const QPointF &point) const {
        Q_D(const RubberBandDragHandler);
        return d->mapToRubberBand(point);
    }

    QRectF RubberBandDragHandler::mapToSelector(const QRectF &rect) const {
        return rect;
    }

}

#include "moc_RubberBandDragHandler_p.cpp"
