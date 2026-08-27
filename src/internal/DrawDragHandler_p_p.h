#ifndef SCOPIC_FLOW_DRAWDRAGHANDLER_P_P_H
#define SCOPIC_FLOW_DRAWDRAGHANDLER_P_P_H

#include <QMetaMethod>
#include <QPointer>
#include <QPointF>

#include <ScopicFlowInternal/private/DrawDragHandler_p.h>

class QQuickItem;

namespace sflow {

    class DragScroller;
    class SelectionController;
    class TimeManipulator;

    class DrawDragHandlerPrivate {
        Q_DECLARE_PUBLIC(DrawDragHandler)

    public:
        explicit DrawDragHandlerPrivate(DrawDragHandler *q);

        void setController(QObject *controller);
        void setVerticalManipulator(QObject *verticalManipulator);

        void onStarted(const PointerInputEvent &event);
        void onMoved(const PointerInputEvent &event);
        void onFinished();
        void onCanceled();
        void onDragScrollerMoved(double deltaX, double deltaY);

        void draw(const QPointF &targetPoint);

        DrawDragHandler *q_ptr = nullptr;
        QPointer<QObject> controller;
        QPointer<TimeManipulator> timeManipulator;
        QPointer<QObject> verticalManipulator;
        QPointer<SelectionController> selectionController;
        QPointer<QQuickItem> target;
        int initialPosition = 0;
        int initialYIndex = 0;
        QPointer<QObject> viewModel;
        DragScroller *dragScroller = nullptr;
        QPointF lastTargetPoint;
        bool drawingStarted = false;
        bool realDrawingStarted = false;

        QMetaMethod controllerDrawingStartedMethod;
        QMetaMethod controllerDrawingCommittedMethod;
        QMetaMethod controllerDrawingAbortedMethod;
        QMetaMethod verticalManipulatorMoveViewByMethod;
        QMetaMethod verticalManipulatorMapToPositionMethod;
    };

}

#endif // SCOPIC_FLOW_DRAWDRAGHANDLER_P_P_H
