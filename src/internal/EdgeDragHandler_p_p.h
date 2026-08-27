#ifndef SCOPIC_FLOW_EDGEDRAGHANDLER_P_P_H
#define SCOPIC_FLOW_EDGEDRAGHANDLER_P_P_H

#include <QMetaMethod>
#include <QMetaProperty>
#include <QPointer>

#include <ScopicFlowInternal/private/EdgeDragHandler_p.h>

class QQuickItem;

namespace sflow {

    class DragScroller;
    class SelectionController;
    class TimeManipulator;

    class EdgeDragHandlerPrivate {
        Q_DECLARE_PUBLIC(EdgeDragHandler)

    public:
        explicit EdgeDragHandlerPrivate(EdgeDragHandler *q);

        void setController(QObject *controller);
        void setUnitedExtendItem(QObject *unitedExtendItem);

        void setActiveViewModel(QObject *viewModel);

        void onStarted(const PointerInputEvent &event, const PointerHit &hit);
        void onMoved(const PointerInputEvent &event);
        void onFinished();
        void onCanceled();
        void onDragScrollerMoved(double deltaX);

        void extendSelectionTo(int position);

        bool checkViewModelProperty(QObject *item, const QMetaProperty &property, const char *name);

        EdgeDragHandler *q_ptr = nullptr;
        QPointer<QObject> controller;
        QPointer<SelectionController> selectionController;
        QPointer<QQuickItem> paneItem;
        QPointer<TimeManipulator> timeManipulator;
        int edge = EdgeDragHandler::LeftEdge;
        QPointer<QObject> activeViewModel;
        QPointer<QObject> unitedExtendItem;
        int unitedExtendRestrict = 0;
        DragScroller *dragScroller = nullptr;

        QMetaMethod controllerAdjustLengthStartedMethod;
        QMetaMethod controllerAdjustLengthCommittedMethod;
        QMetaMethod controllerAdjustLengthAbortedMethod;
        QMetaProperty controllerClickSelectableProperty;
        QMetaProperty viewModelPositionProperty;
        QMetaProperty viewModelLengthProperty;
        QMetaProperty viewModelClipStartProperty;
        QMetaProperty viewModelMaxLengthProperty;
        QMetaProperty unitedExtendItemPositionProperty;
        QMetaProperty unitedExtendItemLengthProperty;
    };

}

#endif // SCOPIC_FLOW_EDGEDRAGHANDLER_P_P_H
