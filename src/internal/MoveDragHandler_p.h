#ifndef SCOPIC_FLOW_MOVEDRAGHANDLER_P_H
#define SCOPIC_FLOW_MOVEDRAGHANDLER_P_H

#include <QScopedPointer>
#include <qqmlintegration.h>

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>

class QQuickItem;

namespace sflow {

    class SelectionController;
    class TimeManipulator;

    class MoveDragHandlerPrivate;

    class MoveDragHandler : public DispatchedDragHandler {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(MoveDragHandler)
        Q_PROPERTY(QObject *controller READ controller WRITE setController NOTIFY controllerChanged)
        Q_PROPERTY(QQuickItem *paneItem READ paneItem WRITE setPaneItem NOTIFY paneItemChanged)
        Q_PROPERTY(TimeManipulator *timeManipulator READ timeManipulator WRITE setTimeManipulator NOTIFY timeManipulatorChanged)
        Q_PROPERTY(QObject *verticalManipulator READ verticalManipulator WRITE setVerticalManipulator NOTIFY verticalManipulatorChanged)
        Q_PROPERTY(SelectionController *selectionController READ selectionController WRITE setSelectionController NOTIFY selectionControllerChanged)
        Q_PROPERTY(int moveFlag READ moveFlag WRITE setMoveFlag NOTIFY moveFlagChanged)

    public:
        explicit MoveDragHandler(QObject *parent = nullptr);
        ~MoveDragHandler() override;

        QObject *controller() const;
        void setController(QObject *controller);

        QQuickItem *paneItem() const;
        void setPaneItem(QQuickItem *paneItem);

        TimeManipulator *timeManipulator() const;
        void setTimeManipulator(TimeManipulator *timeManipulator);

        QObject *verticalManipulator() const;
        void setVerticalManipulator(QObject *verticalManipulator);

        SelectionController *selectionController() const;
        void setSelectionController(SelectionController *selectionController);

        int moveFlag() const;
        void setMoveFlag(int moveFlag);

    Q_SIGNALS:
        void controllerChanged();
        void paneItemChanged();
        void timeManipulatorChanged();
        void verticalManipulatorChanged();
        void selectionControllerChanged();
        void moveFlagChanged();

        void moveSelectionToYRequested(double y, QObject *viewModel);

    private:
        QScopedPointer<MoveDragHandlerPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_MOVEDRAGHANDLER_P_H
