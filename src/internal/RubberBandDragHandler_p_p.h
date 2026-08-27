#ifndef SCOPIC_FLOW_RUBBERBANDDRAGHANDLER_P_P_H
#define SCOPIC_FLOW_RUBBERBANDDRAGHANDLER_P_P_H

#include <QMetaMethod>
#include <QPointer>
#include <QPointF>

#include <ScopicFlowInternal/private/RubberBandDragHandler_p.h>

class QQuickItem;

namespace sflow {

    class DragScroller;
    class RubberBandLayerQuickItem;
    class RubberBandSelector;
    class SelectionController;
    class TimeManipulator;

    class RubberBandDragHandlerPrivate {
        Q_DECLARE_PUBLIC(RubberBandDragHandler)

    public:
        explicit RubberBandDragHandlerPrivate(RubberBandDragHandler *q);

        void setController(QObject *controller);
        void setVerticalManipulator(QObject *verticalManipulator);

        void onStarted(const PointerInputEvent &event);
        void onMoved(const PointerInputEvent &event);
        void onFinished();
        void onCanceled();
        void onDragScrollerMoved(double deltaX, double deltaY);

        QPointF mapToRubberBand(const QPointF &point) const;
        void doDragRubberBand(const QPointF &targetPoint);

        RubberBandDragHandler *q_ptr = nullptr;
        QPointer<QObject> controller;
        QPointer<TimeManipulator> timeManipulator;
        QPointer<QObject> verticalManipulator;
        QPointer<RubberBandLayerQuickItem> rubberBandLayer;
        QPointer<RubberBandSelector> selector;
        QPointer<SelectionController> selectionController;
        QPointer<QQuickItem> target;
        QPointer<QQuickItem> coordinateSpace;
        int mode = RubberBandDragHandler::Normal;
        DragScroller *dragScroller = nullptr;
        QPointF lastTargetPoint;

        QMetaMethod controllerRubberBandDraggingStartedMethod;
        QMetaMethod controllerRubberBandDraggingCommittedMethod;
        QMetaMethod controllerRubberBandDraggingAbortedMethod;
        QMetaMethod verticalManipulatorMoveViewByMethod;
    };

}

#endif // SCOPIC_FLOW_RUBBERBANDDRAGHANDLER_P_P_H
