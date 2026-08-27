#include "EdgeDragHandler_p.h"
#include "EdgeDragHandler_p_p.h"

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

        // Meta-object member names resolved on the controller and view model objects
        constexpr char controllerAdjustLengthStartedMethodName[] = "adjustLengthStarted";
        constexpr char controllerAdjustLengthCommittedMethodName[] = "adjustLengthCommitted";
        constexpr char controllerAdjustLengthAbortedMethodName[] = "adjustLengthAborted";
        constexpr char controllerClickSelectablePropertyName[] = "clickSelectable";
        constexpr char viewModelPositionPropertyName[] = "position";
        constexpr char viewModelLengthPropertyName[] = "length";
        constexpr char viewModelClipStartPropertyName[] = "clipStart";
        constexpr char viewModelMaxLengthPropertyName[] = "maxLength";

        bool isNumericProperty(const QMetaProperty &property) {
            if (!property.isValid())
                return false;
            switch (property.metaType().id()) {
            case QMetaType::Short:
            case QMetaType::UShort:
            case QMetaType::Int:
            case QMetaType::UInt:
            case QMetaType::Long:
            case QMetaType::ULong:
            case QMetaType::LongLong:
            case QMetaType::ULongLong:
            case QMetaType::Float:
            case QMetaType::Double:
                return true;
            default:
                return false;
            }
        }

    }

    EdgeDragHandlerPrivate::EdgeDragHandlerPrivate(EdgeDragHandler *q) : q_ptr(q) {
    }

    void EdgeDragHandlerPrivate::setController(QObject *newController) {
        Q_Q(EdgeDragHandler);
        if (controller == newController)
            return;
        controller = newController;
        controllerAdjustLengthStartedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *, QObject *, int>(
            q, controller, controllerAdjustLengthStartedMethodName);
        controllerAdjustLengthCommittedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *, QObject *, int>(
            q, controller, controllerAdjustLengthCommittedMethodName);
        controllerAdjustLengthAbortedMethod = InvokeMethodHelper::resolveMethod<QQuickItem *, QObject *, int>(
            q, controller, controllerAdjustLengthAbortedMethodName);
        controllerClickSelectableProperty =
            InvokeMethodHelper::resolveProperty(q, controller, controllerClickSelectablePropertyName);
        Q_EMIT q->controllerChanged();
    }

    void EdgeDragHandlerPrivate::setUnitedExtendItem(QObject *newUnitedExtendItem) {
        Q_Q(EdgeDragHandler);
        if (unitedExtendItem == newUnitedExtendItem)
            return;
        unitedExtendItem = newUnitedExtendItem;
        unitedExtendItemPositionProperty =
            InvokeMethodHelper::resolveProperty(q, unitedExtendItem, viewModelPositionPropertyName);
        unitedExtendItemLengthProperty =
            InvokeMethodHelper::resolveProperty(q, unitedExtendItem, viewModelLengthPropertyName);
        Q_EMIT q->unitedExtendItemChanged();
    }

    void EdgeDragHandlerPrivate::setActiveViewModel(QObject *viewModel) {
        Q_Q(EdgeDragHandler);
        activeViewModel = viewModel;
        viewModelPositionProperty = InvokeMethodHelper::resolveProperty(q, activeViewModel, viewModelPositionPropertyName);
        viewModelLengthProperty = InvokeMethodHelper::resolveProperty(q, activeViewModel, viewModelLengthPropertyName);
        // Not every view model family supports these properties; resolve them quietly
        viewModelClipStartProperty =
            InvokeMethodHelper::resolveProperty(q, activeViewModel, viewModelClipStartPropertyName, false);
        viewModelMaxLengthProperty =
            InvokeMethodHelper::resolveProperty(q, activeViewModel, viewModelMaxLengthPropertyName, false);
    }

    void EdgeDragHandlerPrivate::onStarted(const PointerInputEvent &event, const PointerHit &hit) {
        Q_Q(EdgeDragHandler);
        setActiveViewModel(hit.target);
        if (!controller || !activeViewModel) {
            q->cancel();
            return;
        }
        InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerAdjustLengthStartedMethod,
                                                paneItem.data(), activeViewModel.data(), edge);
        if (selectionController && controllerClickSelectableProperty.isValid() &&
            controllerClickSelectableProperty.read(controller).toBool()) {
            selectionController->selectByPointer(activeViewModel, SelectionController::ContextSelection,
                                                 Qt::NoModifier);
        }
        q->setUnitedExtendItem(nullptr);
        q->setUnitedExtendRestrict(0);
        if (event.modifiers & Qt::AltModifier)
            Q_EMIT q->updateUnitedExtendRequested(activeViewModel.data());
    }

    void EdgeDragHandlerPrivate::onMoved(const PointerInputEvent &event) {
        Q_Q(EdgeDragHandler);
        const double delta = event.position.x() - q->pressPosition().x();
        const QRectF targetRect = q->hit().targetRect;
        const double targetX = edge == EdgeDragHandler::LeftEdge
            ? targetRect.x() + delta
            : targetRect.x() + targetRect.width() + delta;
        dragScroller->determine(targetX, event.surfaceRect.width(), 0, 0, QJSValue());
        if (dragScroller->distanceX() == 0)
            extendSelectionTo(q->alignedPosition(targetX));
    }

    void EdgeDragHandlerPrivate::onFinished() {
        Q_Q(EdgeDragHandler);
        dragScroller->setRunning(false);
        if (controller && activeViewModel) {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerAdjustLengthCommittedMethod,
                                                    paneItem.data(), activeViewModel.data(), edge);
        }
        setActiveViewModel(nullptr);
    }

    void EdgeDragHandlerPrivate::onCanceled() {
        Q_Q(EdgeDragHandler);
        dragScroller->setRunning(false);
        if (controller && activeViewModel) {
            InvokeMethodHelper::invokeWithoutReturn(q, controller.data(), controllerAdjustLengthAbortedMethod,
                                                    paneItem.data(), activeViewModel.data(), edge);
        }
        setActiveViewModel(nullptr);
    }

    void EdgeDragHandlerPrivate::onDragScrollerMoved(double deltaX) {
        Q_Q(EdgeDragHandler);
        if (!timeManipulator) {
            qmlWarning(q) << "EdgeDragHandler: timeManipulator is not set";
            return;
        }
        timeManipulator->moveViewBy(deltaX);
        const double x = deltaX < 0 ? 0 : q->surfaceRect().width();
        extendSelectionTo(q->alignedPosition(x));
    }

    bool EdgeDragHandlerPrivate::checkViewModelProperty(QObject *item, const QMetaProperty &property,
                                                        const char *name) {
        Q_Q(EdgeDragHandler);
        if (item && property.isValid() && property.enclosingMetaObject()->cast(item))
            return true;
        qmlWarning(q) << "EdgeDragHandler: object of type "
                      << (item ? item->metaObject()->className() : "(null)")
                      << " does not declare a property named " << name;
        return false;
    }

    void EdgeDragHandlerPrivate::extendSelectionTo(int position) {
        Q_Q(EdgeDragHandler);
        if (!activeViewModel || !selectionController || !viewModelPositionProperty.isValid() ||
            !viewModelLengthProperty.isValid())
            return;
        const auto selection = selectionController->getSelectedItems();
        const bool hasClipStart = !selection.isEmpty() && isNumericProperty(viewModelClipStartProperty);
        const bool hasMaxLength = !selection.isEmpty() && isNumericProperty(viewModelMaxLengthProperty);
        if (edge == EdgeDragHandler::LeftEdge) {
            const int currentPosition = viewModelPositionProperty.read(activeViewModel).toInt();
            if (position == currentPosition)
                return;
            const int deltaPosition = position - currentPosition;
            for (const auto item : selection) {
                if (!checkViewModelProperty(item, viewModelPositionProperty, viewModelPositionPropertyName) ||
                    !checkViewModelProperty(item, viewModelLengthProperty, viewModelLengthPropertyName))
                    return;
                if (hasClipStart &&
                    !checkViewModelProperty(item, viewModelClipStartProperty, viewModelClipStartPropertyName))
                    return;
                if (viewModelPositionProperty.read(item).toInt() + deltaPosition < 0)
                    return;
                if (hasClipStart && viewModelClipStartProperty.read(item).toInt() + deltaPosition < 0)
                    return;
                if (deltaPosition > viewModelLengthProperty.read(item).toInt() - 1)
                    return;
            }
            if (unitedExtendRestrict && unitedExtendItem && unitedExtendItemPositionProperty.isValid() &&
                unitedExtendItemLengthProperty.isValid() && !selection.isEmpty()) {
                const int unitedExtendItemLength = unitedExtendItemLengthProperty.read(unitedExtendItem).toInt();
                const int firstSelectionPosition =
                    viewModelPositionProperty.read(selection.constFirst()).toInt();
                if (unitedExtendItemPositionProperty.read(unitedExtendItem).toInt() + unitedExtendItemLength ==
                        firstSelectionPosition &&
                    unitedExtendItemLength + deltaPosition <= unitedExtendRestrict) {
                    const auto timeLayoutViewModel = timeManipulator ? timeManipulator->timeLayoutViewModel() : nullptr;
                    if (!timeLayoutViewModel) {
                        qmlWarning(q) << "EdgeDragHandler: timeManipulator has no timeLayoutViewModel";
                        return;
                    }
                    if (unitedExtendItemLength + deltaPosition >= timeLayoutViewModel->positionAlignment()) {
                        unitedExtendItemLengthProperty.write(unitedExtendItem, unitedExtendItemLength + deltaPosition);
                    }
                }
            }
            for (const auto item : selection) {
                viewModelPositionProperty.write(item,
                                                viewModelPositionProperty.read(item).toInt() + deltaPosition);
                if (hasClipStart) {
                    viewModelClipStartProperty.write(item,
                                                     viewModelClipStartProperty.read(item).toInt() + deltaPosition);
                }
                viewModelLengthProperty.write(item,
                                              viewModelLengthProperty.read(item).toInt() - deltaPosition);
            }
        } else {
            const int currentEnd = viewModelPositionProperty.read(activeViewModel).toInt() +
                viewModelLengthProperty.read(activeViewModel).toInt();
            if (position == currentEnd)
                return;
            const int deltaPosition = position - currentEnd;
            for (const auto item : selection) {
                if (!checkViewModelProperty(item, viewModelPositionProperty, viewModelPositionPropertyName) ||
                    !checkViewModelProperty(item, viewModelLengthProperty, viewModelLengthPropertyName))
                    return;
                if (hasMaxLength &&
                    !checkViewModelProperty(item, viewModelMaxLengthProperty, viewModelMaxLengthPropertyName))
                    return;
                const int itemPosition = viewModelPositionProperty.read(item).toInt();
                const int itemLength = viewModelLengthProperty.read(item).toInt();
                if (itemPosition + itemLength + deltaPosition < 0)
                    return;
                if (itemPosition + itemLength + deltaPosition < itemPosition + 1)
                    return;
                if (hasMaxLength) {
                    const int itemClipStart = hasClipStart ? viewModelClipStartProperty.read(item).toInt() : 0;
                    if (itemLength + deltaPosition + itemClipStart >
                        viewModelMaxLengthProperty.read(item).toInt())
                        return;
                }
            }
            if (unitedExtendRestrict && unitedExtendItem && unitedExtendItemPositionProperty.isValid() &&
                unitedExtendItemLengthProperty.isValid() && !selection.isEmpty()) {
                const int unitedExtendItemLength = unitedExtendItemLengthProperty.read(unitedExtendItem).toInt();
                const int firstSelectionEnd =
                    viewModelPositionProperty.read(selection.constFirst()).toInt() +
                    viewModelLengthProperty.read(selection.constFirst()).toInt();
                if (unitedExtendItemPositionProperty.read(unitedExtendItem).toInt() == firstSelectionEnd &&
                    unitedExtendItemLength - deltaPosition <= unitedExtendRestrict) {
                    const auto timeLayoutViewModel = timeManipulator ? timeManipulator->timeLayoutViewModel() : nullptr;
                    if (!timeLayoutViewModel) {
                        qmlWarning(q) << "EdgeDragHandler: timeManipulator has no timeLayoutViewModel";
                        return;
                    }
                    if (unitedExtendItemLength - deltaPosition >= timeLayoutViewModel->positionAlignment()) {
                        unitedExtendItemLengthProperty.write(unitedExtendItem, unitedExtendItemLength - deltaPosition);
                        unitedExtendItemPositionProperty.write(
                            unitedExtendItem, unitedExtendItemPositionProperty.read(unitedExtendItem).toInt() +
                                                  deltaPosition);
                    }
                }
            }
            for (const auto item : selection) {
                viewModelLengthProperty.write(item,
                                              viewModelLengthProperty.read(item).toInt() + deltaPosition);
            }
        }
    }

    EdgeDragHandler::EdgeDragHandler(QObject *parent)
        : DispatchedDragHandler(parent), d_ptr(new EdgeDragHandlerPrivate(this)) {
        Q_D(EdgeDragHandler);
        d->dragScroller = new DragScroller(this);
        connect(d->dragScroller, &DragScroller::moved, this, [d](double deltaX, double) {
            d->onDragScrollerMoved(deltaX);
        });
        connect(this, &DispatchedDragHandler::started, this, [d](const PointerInputEvent &event, const PointerHit &hit) {
            d->onStarted(event, hit);
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

    EdgeDragHandler::~EdgeDragHandler() = default;

    QObject *EdgeDragHandler::controller() const {
        Q_D(const EdgeDragHandler);
        return d->controller;
    }

    void EdgeDragHandler::setController(QObject *controller) {
        Q_D(EdgeDragHandler);
        d->setController(controller);
    }

    SelectionController *EdgeDragHandler::selectionController() const {
        Q_D(const EdgeDragHandler);
        return d->selectionController;
    }

    void EdgeDragHandler::setSelectionController(SelectionController *selectionController) {
        Q_D(EdgeDragHandler);
        if (d->selectionController == selectionController)
            return;
        d->selectionController = selectionController;
        Q_EMIT selectionControllerChanged();
    }

    QQuickItem *EdgeDragHandler::paneItem() const {
        Q_D(const EdgeDragHandler);
        return d->paneItem;
    }

    void EdgeDragHandler::setPaneItem(QQuickItem *paneItem) {
        Q_D(EdgeDragHandler);
        if (d->paneItem == paneItem)
            return;
        d->paneItem = paneItem;
        Q_EMIT paneItemChanged();
    }

    TimeManipulator *EdgeDragHandler::timeManipulator() const {
        Q_D(const EdgeDragHandler);
        return d->timeManipulator;
    }

    void EdgeDragHandler::setTimeManipulator(TimeManipulator *timeManipulator) {
        Q_D(EdgeDragHandler);
        if (d->timeManipulator == timeManipulator)
            return;
        d->timeManipulator = timeManipulator;
        Q_EMIT timeManipulatorChanged();
    }

    int EdgeDragHandler::edge() const {
        Q_D(const EdgeDragHandler);
        return d->edge;
    }

    void EdgeDragHandler::setEdge(int edge) {
        Q_D(EdgeDragHandler);
        if (d->edge == edge)
            return;
        d->edge = edge;
        Q_EMIT edgeChanged();
    }

    QObject *EdgeDragHandler::unitedExtendItem() const {
        Q_D(const EdgeDragHandler);
        return d->unitedExtendItem;
    }

    void EdgeDragHandler::setUnitedExtendItem(QObject *unitedExtendItem) {
        Q_D(EdgeDragHandler);
        d->setUnitedExtendItem(unitedExtendItem);
    }

    int EdgeDragHandler::unitedExtendRestrict() const {
        Q_D(const EdgeDragHandler);
        return d->unitedExtendRestrict;
    }

    void EdgeDragHandler::setUnitedExtendRestrict(int unitedExtendRestrict) {
        Q_D(EdgeDragHandler);
        if (d->unitedExtendRestrict == unitedExtendRestrict)
            return;
        d->unitedExtendRestrict = unitedExtendRestrict;
        Q_EMIT unitedExtendRestrictChanged();
    }

    int EdgeDragHandler::alignedPosition(double x) const {
        Q_D(const EdgeDragHandler);
        if (!d->timeManipulator) {
            qmlWarning(this) << "EdgeDragHandler: timeManipulator is not set";
            return 0;
        }
        return std::max(d->timeManipulator->alignPosition(d->timeManipulator->mapToPosition(0),
                                                          ScopicFlow::AO_Ceil),
                        std::min(d->timeManipulator->alignPosition(d->timeManipulator->mapToPosition(x)),
                                 d->timeManipulator->alignPosition(
                                     d->timeManipulator->mapToPosition(surfaceRect().width()),
                                     ScopicFlow::AO_Floor)));
    }

}

#include "moc_EdgeDragHandler_p.cpp"
