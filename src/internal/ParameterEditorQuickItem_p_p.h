#ifndef SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_P_H
#define SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_P_H

#include <QList>
#include <QPointer>
#include <QPointF>
#include <QVector>

#include <ScopicFlowInternal/private/ParameterEditorQuickItem_p.h>

namespace sflow {

    struct ParameterEditorGeometrySnapshot {
        QVector<QPointF> fill;
        QVector<QPointF> solid;
        QVector<QPointF> dashed;
        QVector<QPointF> accent;
        QVector<QPointF> anchors;
        QVector<QPointF> selectedAnchors;
        QVector<QPointF> reference;
    };

    class ParameterEditorQuickItemPrivate {
        Q_DECLARE_PUBLIC(ParameterEditorQuickItem)

    public:
        enum ValueSource {
            NoSource,
            EditedSource,
            OriginalSource,
            DefaultSource,
        };

        ParameterEditorQuickItem *q_ptr = nullptr;

        QPointer<FreeParameterViewModel> freeParameterViewModel;
        QPointer<AnchorParameterViewModel> anchorParameterViewModel;
        QPointer<FreeParameterViewModel> originalParameterViewModel;
        QPointer<FreeParameterViewModel> freeTransformParameterViewModel;
        QPointer<AnchorParameterViewModel> anchorTransformParameterViewModel;
        QPointer<TimeViewModel> timeViewModel;
        QPointer<TimeLayoutViewModel> timeLayoutViewModel;

        ParameterEditorQuickItem::FillMode fillMode = ParameterEditorQuickItem::NoFill;
        double fillBaseline = 0.0;
        bool referenceVisible = false;
        double referenceBaseline = 0.0;
        bool defaultValueEnabled = false;
        double defaultValue = 0.0;
        ParameterEditorQuickItem::CurveDisplayMode fallbackDisplayMode = ParameterEditorQuickItem::CurveDashed;
        ParameterEditorQuickItem::EditLayer editLayer = ParameterEditorQuickItem::FinalLayer;

        QColor curveColor;
        QColor dimmedCurveColor;
        QColor fillColor;
        QColor dimmedFillColor;
        QColor accentColor;
        QColor referenceColor;
        QColor selectedAnchorColor;

        struct MovingAnchorState {
            QPointer<ParameterAnchorViewModel> item;
            int position = 0;
            double value = 0.0;
            double transformedValue = 0.0;
        };

        QVector<MovingAnchorState> movingAnchors;
        QPointer<ParameterAnchorViewModel> primaryMovingAnchor;
        int anchorMoveStartPosition = 0;
        double anchorMoveStartY = 0.0;

        QPointF lineStartPoint;
        int lineSnapshotFirst = 0;
        int lineSnapshotLast = -1;
        QList<QVariant> lineSnapshot;

        ParameterEditorGeometrySnapshot snapshot;

        void invalidate();
        void reconnectModels();
        double positionForX(double x) const;
        double xForPosition(double position) const;
        double transformFactor(double position) const;
        QList<double> transformFactorsAt(const QList<double> &sortedPositions) const;
        QVariant untransformedValue(double position, double transformedValue) const;
        double yForValue(double value) const;
        double transformedAnchorValue(const ParameterAnchorViewModel *item) const;
        void rebuildGeometry();
        bool writeFreeLineSegment(const QPointF &from, const QPointF &to);
    };

}

#endif // SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_P_H
