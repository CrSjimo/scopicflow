#ifndef SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_P_H
#define SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_P_H

#include <ScopicFlowInternal/private/ParameterEditorQuickItem_p.h>

#include <QList>
#include <QPointer>
#include <QPointF>
#include <QVector>

#include <ScopicFlowCore/ParameterAnchorViewModel.h>

namespace sflow {

    struct ParameterEditorGeometryVertex {
        QPointF point;
        float coverage = 1.0f;
    };

    struct ParameterEditorGeometry {
        QVector<ParameterEditorGeometryVertex> vertices;
        QVector<quint32> indices;
    };

    struct ParameterEditorGeometrySnapshot {
        ParameterEditorGeometry fill;
        ParameterEditorGeometry solid;
        ParameterEditorGeometry dashed;
        ParameterEditorGeometry accent;
        ParameterEditorGeometry anchors;
        ParameterEditorGeometry selectedAnchors;
        ParameterEditorGeometry reference;
    };

    struct ParameterEditorSemanticSample {
        QPointF finalPoint;
        QPointF overlayPoint;
        int finalSource = 0;
        bool finalValid = false;
        bool overlayValid = false;
    };

    struct ParameterEditorSemanticAnchor {
        QPointF center;
        ParameterAnchorViewModel::InterpolationMode interpolationMode = ParameterAnchorViewModel::Hermite;
        bool selected = false;
    };

    struct ParameterEditorSemanticSnapshot {
        QVector<ParameterEditorSemanticSample> samples;
        QVector<ParameterEditorSemanticAnchor> anchors;
        double fillY = 0.0;
        double referenceY = 0.0;
        double antialiasWidth = 1.0;
        ParameterEditorQuickItem::FillMode fillMode = ParameterEditorQuickItem::NoFill;
        ParameterEditorQuickItem::CurveDisplayMode fallbackDisplayMode = ParameterEditorQuickItem::CurveDashed;
        bool referenceVisible = false;
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

        ParameterEditorSemanticSnapshot semanticSnapshot;
        quint64 snapshotRevision = 0;
        bool geometryDirty = true;

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
