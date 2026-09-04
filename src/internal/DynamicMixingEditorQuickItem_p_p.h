#ifndef SCOPIC_FLOW_DYNAMICMIXINGEDITORQUICKITEM_P_P_H
#define SCOPIC_FLOW_DYNAMICMIXINGEDITORQUICKITEM_P_P_H

#include <QPointer>
#include <QVector>

#include <ScopicFlowInternal/private/DynamicMixingEditorQuickItem_p.h>

namespace sflow {

    struct DynamicMixingEditorGeometrySnapshot {
        QVector<QVector<QPointF>> fills;
        QVector<QPointF> boundaries;
        QVector<QPointF> anchorLines;
        QVector<QPointF> selectedAnchorLines;
        QVector<QPointF> handles;
        QVector<QPointF> selectedHandleBorders;
    };

    class DynamicMixingEditorQuickItemPrivate {
        Q_DECLARE_PUBLIC(DynamicMixingEditorQuickItem)

    public:
        enum MoveAxis {
            PendingAxis,
            HorizontalAxis,
            VerticalAxis,
        };

        struct HitResult {
            DynamicMixingAnchorViewModel *anchor = nullptr;
            int handleIndex = -1;
        };

        struct MovingAnchorState {
            QPointer<DynamicMixingAnchorViewModel> item;
            int position = 0;
            QList<double> sourceRatio;
            QList<double> effectiveRatio;
        };

        DynamicMixingEditorQuickItem *q_ptr = nullptr;
        QPointer<DynamicMixingViewModel> dynamicMixingViewModel;
        QPointer<TimeViewModel> timeViewModel;
        QPointer<TimeLayoutViewModel> timeLayoutViewModel;

        QList<QColor> colors;
        QColor boundaryLineColor;
        QColor anchorLineColor;
        QColor selectedAnchorLineColor;
        QColor handleColor;
        QColor selectedHandleBorderColor;

        QVector<MovingAnchorState> movingAnchors;
        QPointer<DynamicMixingAnchorViewModel> primaryMovingAnchor;
        QPointF anchorMoveStartPoint;
        int primaryAnchorStartPosition = 0;
        int movingHandleIndex = -1;
        int movingVoiceCount = 1;
        MoveAxis moveAxis = PendingAxis;

        DynamicMixingEditorGeometrySnapshot snapshot;
        quint64 snapshotRevision = 0;

        void invalidate();
        void reconnectModels();
        double positionForX(double x) const;
        double xForPosition(double position) const;
        QVector<double> cumulativeRatio(const QList<double> &ratio) const;
        HitResult hitTest(const QPointF &point, double radius) const;
        void clearMoveState();
        void rebuildGeometry();
    };

}

#endif // SCOPIC_FLOW_DYNAMICMIXINGEDITORQUICKITEM_P_P_H
