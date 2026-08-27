#include "DrawDragHandler_p.h"
#include "DrawDragHandler_p_p.h"

#include <algorithm>

#include <QQuickItem>
#include <QQmlInfo>

#include <ScopicFlowCore/ScopicFlowNamespace.h>
#include <ScopicFlowCore/SelectionController.h>
#include <ScopicFlowCore/TimeLayoutViewModel.h>
#include <ScopicFlowCore/TimeManipulator.h>

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>
#include <ScopicFlowInternal/private/DragScroller_p.h>
#include <ScopicFlowInternal/private/InvokeMethodHelper_p.h>

namespace sflow {

    namespace {

        // Meta-object member names resolved on the controller and manipulator objects
        constexpr char controllerDrawingStartedMethodName[] = "drawingStarted";
        constexpr char controllerDrawingCommittedMethodName[] = "drawingCommitted";
        constexpr char controllerDrawingAbortedMethodName[] = "drawingAborted";
        constexpr char verticalManipulatorMoveViewByMethodName[] = "moveViewBy";
        constexpr char verticalManipulatorMapToPositionMethodName[] = "mapToPosition";

    }

    DrawDragHandlerPrivate::DrawDragHandlerPrivate(DrawDragHandler *q) : q_ptr(q) {
    }

    void DrawDragHandlerPrivate::setController(QObject *newController) {
        Q_Q(DrawDragHandler);
        if (controller == newController)
            return;
        controller = newController;
        controllerDrawingStartedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *>(
            q, controller, controllerDrawingStartedMethodName);
        controllerDrawingCommittedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *>(
            q, controller, controllerDrawingCommittedMethodName);
        controllerDrawingAbortedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *>(
            q, controller, controllerDrawingAbortedMethodName);
        Q_EMIT q->controllerChanged();
    }

    void DrawDragHandlerPrivate::setVerticalManipulator(QObject *newVerticalManipulator) {
        Q_Q(DrawDragHandler);
        if (verticalManipulator == newVerticalManipulator)
            return;
        verticalManipulator = newVerticalManipulator;
        verticalManipulatorMoveViewByMethod = InvokeMethodHelper::resolveMethod<double, bool>(
            q, verticalManipulator, verticalManipulatorMoveViewByMethodName);
        verticalManipulatorMapToPositionMethod = InvokeMethodHelper::resolveMethod<double>(
            q, verticalManipulator, verticalManipulatorMapToPositionMethodName);
        Q_EMIT q->verticalManipulatorChanged();
    }

    void DrawDragHandlerPrivate::onStarted(const PointerInputEvent &event) {
        Q_Q(DrawDragHandler);
        if (!controller) {
            q->cancel();
            return;
        }
        InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerDrawingStartedMethod, target.data());
        if (selectionController)
            selectionController->selectByPointer(nullptr, SelectionController::PrimarySelection, Qt::NoModifier);
        lastTargetPoint = event.position;
        if (!timeManipulator) {
            qmlWarning(q) << "DrawDragHandler: timeManipulator is not set";
            return;
        }
        q->setInitialPosition(timeManipulator->alignPosition(timeManipulator->mapToPosition(event.position.x()),
                                                             ScopicFlow::AO_Visible));
        q->setInitialYIndex(verticalManipulator
                                ? InvokeMethodHelper::invokeWithReturn<int>(q, verticalManipulator.data(),
                                                                            verticalManipulatorMapToPositionMethod,
                                                                            event.position.y())
                                : 0);
    }

    void DrawDragHandlerPrivate::onMoved(const PointerInputEvent &event) {
        const QPointF point = event.position;
        dragScroller->determine(point.x(), event.surfaceRect.width(), point.y(), event.surfaceRect.height(),
                                QJSValue());
        if (dragScroller->distanceX() == 0)
            draw(QPointF(point.x(), lastTargetPoint.y()));
        if (dragScroller->distanceY() == 0)
            draw(QPointF(lastTargetPoint.x(), point.y()));
    }

    void DrawDragHandlerPrivate::onFinished() {
        Q_Q(DrawDragHandler);
        dragScroller->setRunning(false);
        drawingStarted = false;
        realDrawingStarted = false;
        if (viewModel) {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerDrawingCommittedMethod,
                                                    target.data());
        } else {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerDrawingAbortedMethod,
                                                    target.data());
        }
        q->setViewModel(nullptr);
    }

    void DrawDragHandlerPrivate::onCanceled() {
        Q_Q(DrawDragHandler);
        dragScroller->setRunning(false);
        drawingStarted = false;
        realDrawingStarted = false;
        if (controller) {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerDrawingAbortedMethod,
                                                    target.data());
        }
        q->setViewModel(nullptr);
    }

    void DrawDragHandlerPrivate::onDragScrollerMoved(double deltaX, double deltaY) {
        Q_Q(DrawDragHandler);
        if (timeManipulator)
            timeManipulator->moveViewBy(deltaX);
        if (verticalManipulator) {
            InvokeMethodHelper::invokeWithoutReturn(q, verticalManipulator.data(), verticalManipulatorMoveViewByMethod,
                                                    deltaY, false);
        }
        const QRectF surfaceRect = q->surfaceRect();
        if (deltaX != 0)
            draw(QPointF(deltaX > 0 ? surfaceRect.width() : 0, lastTargetPoint.y()));
        if (deltaY != 0)
            draw(QPointF(lastTargetPoint.x(), deltaY > 0 ? surfaceRect.height() : 0));
    }

    void DrawDragHandlerPrivate::draw(const QPointF &targetPoint) {
        Q_Q(DrawDragHandler);
        if (!drawingStarted && targetPoint.x() - q->pressPosition().x() < 4)
            return;
        drawingStarted = true;
        if (!timeManipulator) {
            qmlWarning(q) << "DrawDragHandler: timeManipulator is not set";
            return;
        }
        const int position = timeManipulator->alignPosition(timeManipulator->mapToPosition(targetPoint.x()),
                                                            ScopicFlow::AO_Visible);
        const int yIndex = verticalManipulator
            ? InvokeMethodHelper::invokeWithReturn<int>(q, verticalManipulator.data(),
                                                        verticalManipulatorMapToPositionMethod, targetPoint.y())
            : 0;
        const auto timeLayoutViewModel = timeManipulator->timeLayoutViewModel();
        if (realDrawingStarted && !timeLayoutViewModel) {
            qmlWarning(q) << "DrawDragHandler: timeManipulator has no timeLayoutViewModel";
            return;
        }
        const int length =
            std::max(realDrawingStarted ? timeLayoutViewModel->positionAlignment() : 0, position - initialPosition);
        if (!viewModel) {
            Q_EMIT q->createViewModelRequested(initialPosition, initialYIndex);
        } else if (length > 0) {
            realDrawingStarted = true;
            Q_EMIT q->updateViewModelRequested(length, yIndex);
        }
        lastTargetPoint = targetPoint;
    }

    DrawDragHandler::DrawDragHandler(QObject *parent)
        : DispatchedDragHandler(parent), d_ptr(new DrawDragHandlerPrivate(this)) {
        Q_D(DrawDragHandler);
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

    DrawDragHandler::~DrawDragHandler() = default;

    QObject *DrawDragHandler::controller() const {
        Q_D(const DrawDragHandler);
        return d->controller;
    }

    void DrawDragHandler::setController(QObject *controller) {
        Q_D(DrawDragHandler);
        d->setController(controller);
    }

    TimeManipulator *DrawDragHandler::timeManipulator() const {
        Q_D(const DrawDragHandler);
        return d->timeManipulator;
    }

    void DrawDragHandler::setTimeManipulator(TimeManipulator *timeManipulator) {
        Q_D(DrawDragHandler);
        if (d->timeManipulator == timeManipulator)
            return;
        d->timeManipulator = timeManipulator;
        Q_EMIT timeManipulatorChanged();
    }

    QObject *DrawDragHandler::verticalManipulator() const {
        Q_D(const DrawDragHandler);
        return d->verticalManipulator;
    }

    void DrawDragHandler::setVerticalManipulator(QObject *verticalManipulator) {
        Q_D(DrawDragHandler);
        d->setVerticalManipulator(verticalManipulator);
    }

    SelectionController *DrawDragHandler::selectionController() const {
        Q_D(const DrawDragHandler);
        return d->selectionController;
    }

    void DrawDragHandler::setSelectionController(SelectionController *selectionController) {
        Q_D(DrawDragHandler);
        if (d->selectionController == selectionController)
            return;
        d->selectionController = selectionController;
        Q_EMIT selectionControllerChanged();
    }

    QQuickItem *DrawDragHandler::target() const {
        Q_D(const DrawDragHandler);
        return d->target;
    }

    void DrawDragHandler::setTarget(QQuickItem *target) {
        Q_D(DrawDragHandler);
        if (d->target == target)
            return;
        d->target = target;
        Q_EMIT targetChanged();
    }

    int DrawDragHandler::initialPosition() const {
        Q_D(const DrawDragHandler);
        return d->initialPosition;
    }

    void DrawDragHandler::setInitialPosition(int initialPosition) {
        Q_D(DrawDragHandler);
        if (d->initialPosition == initialPosition)
            return;
        d->initialPosition = initialPosition;
        Q_EMIT initialPositionChanged();
    }

    int DrawDragHandler::initialYIndex() const {
        Q_D(const DrawDragHandler);
        return d->initialYIndex;
    }

    void DrawDragHandler::setInitialYIndex(int initialYIndex) {
        Q_D(DrawDragHandler);
        if (d->initialYIndex == initialYIndex)
            return;
        d->initialYIndex = initialYIndex;
        Q_EMIT initialYIndexChanged();
    }

    QObject *DrawDragHandler::viewModel() const {
        Q_D(const DrawDragHandler);
        return d->viewModel;
    }

    void DrawDragHandler::setViewModel(QObject *viewModel) {
        Q_D(DrawDragHandler);
        if (d->viewModel == viewModel)
            return;
        d->viewModel = viewModel;
        Q_EMIT viewModelChanged();
    }

}

#include "moc_DrawDragHandler_p.cpp"
