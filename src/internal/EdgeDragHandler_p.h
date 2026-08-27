#ifndef SCOPIC_FLOW_EDGEDRAGHANDLER_P_H
#define SCOPIC_FLOW_EDGEDRAGHANDLER_P_H

#include <QScopedPointer>
#include <qqmlintegration.h>

#include <ScopicFlowInternal/private/DispatchedDragHandler_p.h>

class QQuickItem;

namespace sflow {

    class SelectionController;
    class TimeManipulator;

    class EdgeDragHandlerPrivate;

    class EdgeDragHandler : public DispatchedDragHandler {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(EdgeDragHandler)
        Q_PROPERTY(QObject *controller READ controller WRITE setController NOTIFY controllerChanged)
        Q_PROPERTY(SelectionController *selectionController READ selectionController WRITE setSelectionController NOTIFY selectionControllerChanged)
        Q_PROPERTY(QQuickItem *paneItem READ paneItem WRITE setPaneItem NOTIFY paneItemChanged)
        Q_PROPERTY(TimeManipulator *timeManipulator READ timeManipulator WRITE setTimeManipulator NOTIFY timeManipulatorChanged)
        Q_PROPERTY(int edge READ edge WRITE setEdge NOTIFY edgeChanged REQUIRED)
        Q_PROPERTY(QObject *unitedExtendItem READ unitedExtendItem WRITE setUnitedExtendItem NOTIFY unitedExtendItemChanged)
        Q_PROPERTY(int unitedExtendRestrict READ unitedExtendRestrict WRITE setUnitedExtendRestrict NOTIFY unitedExtendRestrictChanged)

    public:
        enum Edge {
            LeftEdge,
            RightEdge,
        };
        Q_ENUM(Edge)

        explicit EdgeDragHandler(QObject *parent = nullptr);
        ~EdgeDragHandler() override;

        QObject *controller() const;
        void setController(QObject *controller);

        SelectionController *selectionController() const;
        void setSelectionController(SelectionController *selectionController);

        QQuickItem *paneItem() const;
        void setPaneItem(QQuickItem *paneItem);

        TimeManipulator *timeManipulator() const;
        void setTimeManipulator(TimeManipulator *timeManipulator);

        int edge() const;
        void setEdge(int edge);

        QObject *unitedExtendItem() const;
        void setUnitedExtendItem(QObject *unitedExtendItem);

        int unitedExtendRestrict() const;
        void setUnitedExtendRestrict(int unitedExtendRestrict);

        Q_INVOKABLE int alignedPosition(double x) const;

    Q_SIGNALS:
        void controllerChanged();
        void selectionControllerChanged();
        void paneItemChanged();
        void timeManipulatorChanged();
        void edgeChanged();
        void unitedExtendItemChanged();
        void unitedExtendRestrictChanged();

        void updateUnitedExtendRequested(QObject *viewModel);

    private:
        QScopedPointer<EdgeDragHandlerPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_EDGEDRAGHANDLER_P_H
