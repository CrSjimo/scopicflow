#ifndef SCOPIC_FLOW_DRAWDRAGHANDLER_P_H
#define SCOPIC_FLOW_DRAWDRAGHANDLER_P_H

#include <QScopedPointer>
#include <qqmlintegration.h>

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>

class QQuickItem;

namespace sflow {

    class SelectionController;
    class TimeManipulator;

    class DrawDragHandlerPrivate;

    class DrawDragHandler : public DispatchedDragHandler {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(DrawDragHandler)
        Q_PROPERTY(QObject *controller READ controller WRITE setController NOTIFY controllerChanged)
        Q_PROPERTY(TimeManipulator *timeManipulator READ timeManipulator WRITE setTimeManipulator NOTIFY timeManipulatorChanged)
        Q_PROPERTY(QObject *verticalManipulator READ verticalManipulator WRITE setVerticalManipulator NOTIFY verticalManipulatorChanged)
        Q_PROPERTY(SelectionController *selectionController READ selectionController WRITE setSelectionController NOTIFY selectionControllerChanged)
        Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged REQUIRED)
        Q_PROPERTY(int initialPosition READ initialPosition WRITE setInitialPosition NOTIFY initialPositionChanged)
        Q_PROPERTY(int initialYIndex READ initialYIndex WRITE setInitialYIndex NOTIFY initialYIndexChanged)
        Q_PROPERTY(QObject *viewModel READ viewModel WRITE setViewModel NOTIFY viewModelChanged)

    public:
        explicit DrawDragHandler(QObject *parent = nullptr);
        ~DrawDragHandler() override;

        QObject *controller() const;
        void setController(QObject *controller);

        TimeManipulator *timeManipulator() const;
        void setTimeManipulator(TimeManipulator *timeManipulator);

        QObject *verticalManipulator() const;
        void setVerticalManipulator(QObject *verticalManipulator);

        SelectionController *selectionController() const;
        void setSelectionController(SelectionController *selectionController);

        QQuickItem *target() const;
        void setTarget(QQuickItem *target);

        int initialPosition() const;
        void setInitialPosition(int initialPosition);

        int initialYIndex() const;
        void setInitialYIndex(int initialYIndex);

        QObject *viewModel() const;
        void setViewModel(QObject *viewModel);

    Q_SIGNALS:
        void controllerChanged();
        void timeManipulatorChanged();
        void verticalManipulatorChanged();
        void selectionControllerChanged();
        void targetChanged();
        void initialPositionChanged();
        void initialYIndexChanged();
        void viewModelChanged();

        void createViewModelRequested(int position, int yIndex);
        void updateViewModelRequested(int length, int yIndex);

    private:
        QScopedPointer<DrawDragHandlerPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_DRAWDRAGHANDLER_P_H
