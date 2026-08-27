#ifndef SCOPIC_FLOW_RUBBERBANDDRAGHANDLER_P_H
#define SCOPIC_FLOW_RUBBERBANDDRAGHANDLER_P_H

#include <QScopedPointer>
#include <qqmlintegration.h>

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>

class QQuickItem;

namespace sflow {

    class RubberBandLayerQuickItem;
    class RubberBandSelector;
    class SelectionController;
    class TimeManipulator;

    class RubberBandDragHandlerPrivate;

    class RubberBandDragHandler : public DispatchedDragHandler {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(RubberBandDragHandler)
        Q_PROPERTY(QObject *controller READ controller WRITE setController NOTIFY controllerChanged)
        Q_PROPERTY(TimeManipulator *timeManipulator READ timeManipulator WRITE setTimeManipulator NOTIFY timeManipulatorChanged)
        Q_PROPERTY(QObject *verticalManipulator READ verticalManipulator WRITE setVerticalManipulator NOTIFY verticalManipulatorChanged)
        Q_PROPERTY(RubberBandLayerQuickItem *rubberBandLayer READ rubberBandLayer WRITE setRubberBandLayer NOTIFY rubberBandLayerChanged)
        Q_PROPERTY(RubberBandSelector *selector READ selector WRITE setSelector NOTIFY selectorChanged)
        Q_PROPERTY(SelectionController *selectionController READ selectionController WRITE setSelectionController NOTIFY selectionControllerChanged)
        Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged REQUIRED)
        Q_PROPERTY(QQuickItem *coordinateSpace READ coordinateSpace WRITE setCoordinateSpace NOTIFY coordinateSpaceChanged)
        Q_PROPERTY(int mode READ mode WRITE setMode NOTIFY modeChanged)

    public:
        enum Mode {
            Normal,
            TimeRange,
        };
        Q_ENUM(Mode)

        explicit RubberBandDragHandler(QObject *parent = nullptr);
        ~RubberBandDragHandler() override;

        QObject *controller() const;
        void setController(QObject *controller);

        TimeManipulator *timeManipulator() const;
        void setTimeManipulator(TimeManipulator *timeManipulator);

        QObject *verticalManipulator() const;
        void setVerticalManipulator(QObject *verticalManipulator);

        RubberBandLayerQuickItem *rubberBandLayer() const;
        void setRubberBandLayer(RubberBandLayerQuickItem *rubberBandLayer);

        RubberBandSelector *selector() const;
        void setSelector(RubberBandSelector *selector);

        SelectionController *selectionController() const;
        void setSelectionController(SelectionController *selectionController);

        QQuickItem *target() const;
        void setTarget(QQuickItem *target);

        QQuickItem *coordinateSpace() const;
        void setCoordinateSpace(QQuickItem *coordinateSpace);

        int mode() const;
        void setMode(int mode);

        Q_INVOKABLE QPointF mapToRubberBand(const QPointF &point) const;

    Q_SIGNALS:
        void controllerChanged();
        void timeManipulatorChanged();
        void verticalManipulatorChanged();
        void rubberBandLayerChanged();
        void selectorChanged();
        void selectionControllerChanged();
        void targetChanged();
        void coordinateSpaceChanged();
        void modeChanged();

    private:
        QScopedPointer<RubberBandDragHandlerPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_RUBBERBANDDRAGHANDLER_P_H
