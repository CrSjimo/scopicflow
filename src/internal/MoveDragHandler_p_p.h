#ifndef SCOPIC_FLOW_MOVEDRAGHANDLER_P_P_H
#define SCOPIC_FLOW_MOVEDRAGHANDLER_P_P_H

#include <ScopicFlowInternal/private/MoveDragHandler_p.h>

#include <QMetaMethod>
#include <QMetaProperty>
#include <QPointer>

class QQuickItem;

namespace sflow {

    class DragScroller;
    class SelectionController;
    class TimeManipulator;

    class MoveDragHandlerPrivate {
        Q_DECLARE_PUBLIC(MoveDragHandler)

    public:
        explicit MoveDragHandlerPrivate(MoveDragHandler *q);

        void setController(QObject *controller);
        void setPaneItem(QQuickItem *paneItem);
        void setTimeManipulator(TimeManipulator *timeManipulator);
        void setVerticalManipulator(QObject *verticalManipulator);
        void setSelectionController(SelectionController *selectionController);
        void setMoveFlag(int moveFlag);

        void setActiveViewModel(QObject *viewModel);

        void onStarted(const PointerHit &hit);
        void onMoved(const PointerInputEvent &event);
        void onFinished();
        void onCanceled();
        void onDragScrollerMoved(double deltaX, double deltaY);

        void moveSelectionToX(double x);

        MoveDragHandler *q_ptr = nullptr;
        QPointer<QObject> controller;
        QPointer<QQuickItem> paneItem;
        QPointer<TimeManipulator> timeManipulator;
        QPointer<QObject> verticalManipulator;
        QPointer<SelectionController> selectionController;
        int moveFlag = 0;
        QPointer<QObject> activeViewModel;
        DragScroller *dragScroller = nullptr;

        QMetaMethod controllerMovingStartedMethod;
        QMetaMethod controllerMovingCommittedMethod;
        QMetaMethod controllerMovingAbortedMethod;
        QMetaMethod verticalManipulatorMoveViewByMethod;
        QMetaProperty controllerClickSelectableProperty;
        QMetaProperty viewModelPositionProperty;
    };

}

#endif // SCOPIC_FLOW_MOVEDRAGHANDLER_P_P_H
