#include "MoveDragHandler_p.h"
#include "MoveDragHandler_p_p.h"

#include <QQuickItem>
#include <QQmlInfo>

#include <ScopicFlowCore/SelectionController.h>
#include <ScopicFlowCore/TimeManipulator.h>

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>
#include <ScopicFlowInternal/private/DragScroller_p.h>
#include <ScopicFlowInternal/private/InvokeMethodHelper_p.h>

namespace sflow {

    namespace {

        // Meta-object member names resolved on the controller, manipulator, and view model objects
        constexpr char controllerMovingStartedMethodName[] = "movingStarted";
        constexpr char controllerMovingCommittedMethodName[] = "movingCommitted";
        constexpr char controllerMovingAbortedMethodName[] = "movingAborted";
        constexpr char verticalManipulatorMoveViewByMethodName[] = "moveViewBy";
        constexpr char controllerClickSelectablePropertyName[] = "clickSelectable";
        constexpr char viewModelPositionPropertyName[] = "position";

    }

    MoveDragHandlerPrivate::MoveDragHandlerPrivate(MoveDragHandler *q) : q_ptr(q) {
    }

    void MoveDragHandlerPrivate::setController(QObject *newController) {
        Q_Q(MoveDragHandler);
        if (controller == newController)
            return;
        controller = newController;
        controllerMovingStartedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *, QObject *, int>(
            q, controller, controllerMovingStartedMethodName);
        controllerMovingCommittedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *, QObject *>(
            q, controller, controllerMovingCommittedMethodName);
        controllerMovingAbortedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *, QObject *>(
            q, controller, controllerMovingAbortedMethodName);
        controllerClickSelectableProperty = InvokeMethodHelper::resolveProperty(q, controller, controllerClickSelectablePropertyName);
        Q_EMIT q->controllerChanged();
    }

    void MoveDragHandlerPrivate::setPaneItem(QQuickItem *newPaneItem) {
        Q_Q(MoveDragHandler);
        if (paneItem == newPaneItem)
            return;
        paneItem = newPaneItem;
        Q_EMIT q->paneItemChanged();
    }

    void MoveDragHandlerPrivate::setTimeManipulator(TimeManipulator *newTimeManipulator) {
        Q_Q(MoveDragHandler);
        if (timeManipulator == newTimeManipulator)
            return;
        timeManipulator = newTimeManipulator;
        Q_EMIT q->timeManipulatorChanged();
    }

    void MoveDragHandlerPrivate::setVerticalManipulator(QObject *newVerticalManipulator) {
        Q_Q(MoveDragHandler);
        if (verticalManipulator == newVerticalManipulator)
            return;
        verticalManipulator = newVerticalManipulator;
        verticalManipulatorMoveViewByMethod = InvokeMethodHelper::resolveMethod<double, bool>(
            q, verticalManipulator, verticalManipulatorMoveViewByMethodName);
        Q_EMIT q->verticalManipulatorChanged();
    }

    void MoveDragHandlerPrivate::setSelectionController(SelectionController *newSelectionController) {
        Q_Q(MoveDragHandler);
        if (selectionController == newSelectionController)
            return;
        selectionController = newSelectionController;
        Q_EMIT q->selectionControllerChanged();
    }

    void MoveDragHandlerPrivate::setMoveFlag(int newMoveFlag) {
        Q_Q(MoveDragHandler);
        if (moveFlag == newMoveFlag)
            return;
        moveFlag = newMoveFlag;
        Q_EMIT q->moveFlagChanged();
    }

    void MoveDragHandlerPrivate::setActiveViewModel(QObject *viewModel) {
        Q_Q(MoveDragHandler);
        activeViewModel = viewModel;
        viewModelPositionProperty = InvokeMethodHelper::resolveProperty(q, activeViewModel, viewModelPositionPropertyName);
    }

    void MoveDragHandlerPrivate::onStarted(const PointerHit &hit) {
        Q_Q(MoveDragHandler);
        setActiveViewModel(hit.target);
        if (!controller || !activeViewModel) {
            q->cancel();
            return;
        }
        InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerMovingStartedMethod,
                                                paneItem.data(), activeViewModel.data(), moveFlag);
        if (selectionController && controllerClickSelectableProperty.isValid() &&
            controllerClickSelectableProperty.read(controller).toBool()) {
            selectionController->selectByPointer(activeViewModel, SelectionController::ContextSelection, Qt::NoModifier);
        }
    }

    void MoveDragHandlerPrivate::onMoved(const PointerInputEvent &event) {
        Q_Q(MoveDragHandler);
        const QPointF point = event.position;
        const QRectF targetRect = q->hit().targetRect;
        const QPointF pressPoint = q->pressPosition();
        const double targetX = targetRect.x() + point.x() - pressPoint.x();
        const double targetY = targetRect.y() + point.y() - pressPoint.y();
        dragScroller->determine(point.x(), event.surfaceRect.width(), point.y(), event.surfaceRect.height(), QJSValue());
        if (dragScroller->distanceX() == 0)
            moveSelectionToX(targetX);
        if (dragScroller->distanceY() == 0)
            Q_EMIT q->moveSelectionToYRequested(targetY, activeViewModel.data());
    }

    void MoveDragHandlerPrivate::onFinished() {
        Q_Q(MoveDragHandler);
        if (controller && activeViewModel) {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerMovingCommittedMethod,
                                                    paneItem.data(), activeViewModel.data());
        }
        dragScroller->setRunning(false);
        setActiveViewModel(nullptr);
    }

    void MoveDragHandlerPrivate::onCanceled() {
        Q_Q(MoveDragHandler);
        if (controller && activeViewModel) {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerMovingAbortedMethod,
                                                    paneItem.data(), activeViewModel.data());
        }
        dragScroller->setRunning(false);
        setActiveViewModel(nullptr);
    }

    void MoveDragHandlerPrivate::onDragScrollerMoved(double deltaX, double deltaY) {
        Q_Q(MoveDragHandler);
        if (timeManipulator)
            timeManipulator->moveViewBy(deltaX);
        if (verticalManipulator) {
            InvokeMethodHelper::invokeWithoutReturn(q, verticalManipulator.data(), verticalManipulatorMoveViewByMethod,
                                                    deltaY, false);
        }
        const QRectF surfaceRect = q->surfaceRect();
        const QRectF targetRect = q->hit().targetRect;
        const QPointF pressPoint = q->pressPosition();
        if (deltaX > 0) {
            moveSelectionToX(targetRect.x() + surfaceRect.width() - pressPoint.x());
        } else if (deltaX < 0) {
            moveSelectionToX(targetRect.x() - pressPoint.x());
        }
        if (deltaY > 0) {
            Q_EMIT q->moveSelectionToYRequested(targetRect.y() + surfaceRect.height() - pressPoint.y(), activeViewModel.data());
        } else if (deltaY < 0) {
            Q_EMIT q->moveSelectionToYRequested(targetRect.y() - pressPoint.y(), activeViewModel.data());
        }
    }

    void MoveDragHandlerPrivate::moveSelectionToX(double x) {
        Q_Q(MoveDragHandler);
        if (!activeViewModel || !timeManipulator || !selectionController || !viewModelPositionProperty.isValid())
            return;
        const int position = timeManipulator->alignPosition(timeManipulator->mapToPosition(x));
        const int deltaPosition = position - viewModelPositionProperty.read(activeViewModel).toInt();
        if (deltaPosition == 0)
            return;
        const auto selection = selectionController->getSelectedItems();
        for (const auto item : selection) {
            if (!item || !viewModelPositionProperty.enclosingMetaObject()->cast(item)) {
                qmlWarning(q) << "MoveDragHandler: object of type "
                              << (item ? item->metaObject()->className() : "(null)")
                              << " does not declare a property named " << viewModelPositionPropertyName;
                return;
            }
            if (viewModelPositionProperty.read(item).toInt() + deltaPosition < 0)
                return;
        }
        for (const auto item : selection) {
            viewModelPositionProperty.write(item, viewModelPositionProperty.read(item).toInt() + deltaPosition);
        }
    }

    MoveDragHandler::MoveDragHandler(QObject *parent) : DispatchedDragHandler(parent), d_ptr(new MoveDragHandlerPrivate(this)) {
        Q_D(MoveDragHandler);
        d->dragScroller = new DragScroller(this);
        connect(d->dragScroller, &DragScroller::moved, this, [d](double deltaX, double deltaY) {
            d->onDragScrollerMoved(deltaX, deltaY);
        });
        connect(this, &DispatchedDragHandler::started, this, [d](const PointerInputEvent &, const PointerHit &hit) {
            d->onStarted(hit);
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

    MoveDragHandler::~MoveDragHandler() = default;

    QObject *MoveDragHandler::controller() const {
        Q_D(const MoveDragHandler);
        return d->controller;
    }

    void MoveDragHandler::setController(QObject *controller) {
        Q_D(MoveDragHandler);
        d->setController(controller);
    }

    QQuickItem *MoveDragHandler::paneItem() const {
        Q_D(const MoveDragHandler);
        return d->paneItem;
    }

    void MoveDragHandler::setPaneItem(QQuickItem *paneItem) {
        Q_D(MoveDragHandler);
        d->setPaneItem(paneItem);
    }

    TimeManipulator *MoveDragHandler::timeManipulator() const {
        Q_D(const MoveDragHandler);
        return d->timeManipulator;
    }

    void MoveDragHandler::setTimeManipulator(TimeManipulator *timeManipulator) {
        Q_D(MoveDragHandler);
        d->setTimeManipulator(timeManipulator);
    }

    QObject *MoveDragHandler::verticalManipulator() const {
        Q_D(const MoveDragHandler);
        return d->verticalManipulator;
    }

    void MoveDragHandler::setVerticalManipulator(QObject *verticalManipulator) {
        Q_D(MoveDragHandler);
        d->setVerticalManipulator(verticalManipulator);
    }

    SelectionController *MoveDragHandler::selectionController() const {
        Q_D(const MoveDragHandler);
        return d->selectionController;
    }

    void MoveDragHandler::setSelectionController(SelectionController *selectionController) {
        Q_D(MoveDragHandler);
        d->setSelectionController(selectionController);
    }

    int MoveDragHandler::moveFlag() const {
        Q_D(const MoveDragHandler);
        return d->moveFlag;
    }

    void MoveDragHandler::setMoveFlag(int moveFlag) {
        Q_D(MoveDragHandler);
        d->setMoveFlag(moveFlag);
    }

}

#include "moc_MoveDragHandler_p.cpp"
