#ifndef SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_H
#define SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_H

#include <QColor>
#include <QQuickItem>
#include <QVariant>

namespace sflow {

    class FreeParameterViewModel;
    class AnchorParameterViewModel;
    class ParameterAnchorViewModel;
    class TimeLayoutViewModel;
    class TimeViewModel;
    class ParameterEditorQuickItemPrivate;

    class ParameterEditorQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(ParameterEditorContent)
        Q_DECLARE_PRIVATE(ParameterEditorQuickItem)

        Q_PROPERTY(FreeParameterViewModel *freeParameterViewModel READ freeParameterViewModel WRITE setFreeParameterViewModel NOTIFY freeParameterViewModelChanged)
        Q_PROPERTY(AnchorParameterViewModel *anchorParameterViewModel READ anchorParameterViewModel WRITE setAnchorParameterViewModel NOTIFY anchorParameterViewModelChanged)
        Q_PROPERTY(FreeParameterViewModel *originalParameterViewModel READ originalParameterViewModel WRITE setOriginalParameterViewModel NOTIFY originalParameterViewModelChanged)
        Q_PROPERTY(FreeParameterViewModel *freeTransformParameterViewModel READ freeTransformParameterViewModel WRITE setFreeTransformParameterViewModel NOTIFY freeTransformParameterViewModelChanged)
        Q_PROPERTY(AnchorParameterViewModel *anchorTransformParameterViewModel READ anchorTransformParameterViewModel WRITE setAnchorTransformParameterViewModel NOTIFY anchorTransformParameterViewModelChanged)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)

        Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
        Q_PROPERTY(double fillBaseline READ fillBaseline WRITE setFillBaseline NOTIFY fillBaselineChanged)
        Q_PROPERTY(bool referenceVisible READ referenceVisible WRITE setReferenceVisible NOTIFY referenceVisibleChanged)
        Q_PROPERTY(double referenceBaseline READ referenceBaseline WRITE setReferenceBaseline NOTIFY referenceBaselineChanged)
        Q_PROPERTY(bool defaultValueEnabled READ defaultValueEnabled WRITE setDefaultValueEnabled NOTIFY defaultValueEnabledChanged)
        Q_PROPERTY(double defaultValue READ defaultValue WRITE setDefaultValue NOTIFY defaultValueChanged)
        Q_PROPERTY(CurveDisplayMode originalAndDefaultCurveDisplayMode READ originalAndDefaultCurveDisplayMode WRITE setOriginalAndDefaultCurveDisplayMode NOTIFY originalAndDefaultCurveDisplayModeChanged)
        Q_PROPERTY(EditLayer editLayer READ editLayer WRITE setEditLayer NOTIFY editLayerChanged)

        Q_PROPERTY(QColor curveColor READ curveColor WRITE setCurveColor NOTIFY curveColorChanged)
        Q_PROPERTY(QColor dimmedCurveColor READ dimmedCurveColor WRITE setDimmedCurveColor NOTIFY dimmedCurveColorChanged)
        Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
        Q_PROPERTY(QColor dimmedFillColor READ dimmedFillColor WRITE setDimmedFillColor NOTIFY dimmedFillColorChanged)
        Q_PROPERTY(QColor accentColor READ accentColor WRITE setAccentColor NOTIFY accentColorChanged)
        Q_PROPERTY(QColor referenceColor READ referenceColor WRITE setReferenceColor NOTIFY referenceColorChanged)
        Q_PROPERTY(QColor selectedAnchorColor READ selectedAnchorColor WRITE setSelectedAnchorColor NOTIFY selectedAnchorColorChanged)

    public:
        enum FillMode {
            NoFill,
            TopFill,
            BottomFill,
            BaselineFill,
        };
        Q_ENUM(FillMode)

        enum CurveDisplayMode {
            CurveHidden,
            CurveDashed,
            CurveSolid,
        };
        Q_ENUM(CurveDisplayMode)

        enum EditLayer {
            FinalLayer,
            FreeLayer,
            AnchorLayer,
        };
        Q_ENUM(EditLayer)

        explicit ParameterEditorQuickItem(QQuickItem *parent = nullptr);
        ~ParameterEditorQuickItem() override;

        FreeParameterViewModel *freeParameterViewModel() const;
        void setFreeParameterViewModel(FreeParameterViewModel *viewModel);
        AnchorParameterViewModel *anchorParameterViewModel() const;
        void setAnchorParameterViewModel(AnchorParameterViewModel *viewModel);
        FreeParameterViewModel *originalParameterViewModel() const;
        void setOriginalParameterViewModel(FreeParameterViewModel *viewModel);
        FreeParameterViewModel *freeTransformParameterViewModel() const;
        void setFreeTransformParameterViewModel(FreeParameterViewModel *viewModel);
        AnchorParameterViewModel *anchorTransformParameterViewModel() const;
        void setAnchorTransformParameterViewModel(AnchorParameterViewModel *viewModel);
        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *viewModel);
        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *viewModel);

        FillMode fillMode() const;
        void setFillMode(FillMode mode);
        double fillBaseline() const;
        void setFillBaseline(double value);
        bool referenceVisible() const;
        void setReferenceVisible(bool visible);
        double referenceBaseline() const;
        void setReferenceBaseline(double value);
        bool defaultValueEnabled() const;
        void setDefaultValueEnabled(bool enabled);
        double defaultValue() const;
        void setDefaultValue(double value);
        CurveDisplayMode originalAndDefaultCurveDisplayMode() const;
        void setOriginalAndDefaultCurveDisplayMode(CurveDisplayMode mode);
        EditLayer editLayer() const;
        void setEditLayer(EditLayer layer);

        QColor curveColor() const;
        void setCurveColor(const QColor &color);
        QColor dimmedCurveColor() const;
        void setDimmedCurveColor(const QColor &color);
        QColor fillColor() const;
        void setFillColor(const QColor &color);
        QColor dimmedFillColor() const;
        void setDimmedFillColor(const QColor &color);
        QColor accentColor() const;
        void setAccentColor(const QColor &color);
        QColor referenceColor() const;
        void setReferenceColor(const QColor &color);
        QColor selectedAnchorColor() const;
        void setSelectedAnchorColor(const QColor &color);

        Q_INVOKABLE QVariant transformedValueFromPoint(const QPointF &point) const;
        Q_INVOKABLE bool drawFreeSegment(const QPointF &from, const QPointF &to, bool erase);
        Q_INVOKABLE ParameterAnchorViewModel *anchorAt(const QPointF &point, double radius) const;
        Q_INVOKABLE QObjectList anchorsInRect(const QRectF &rect) const;
        Q_INVOKABLE QObjectList anchorsInTimeRange(int firstPosition, int lastPosition) const;
        Q_INVOKABLE bool beginAnchorMove(const QObjectList &items,
                                         ParameterAnchorViewModel *primaryItem,
                                         const QPointF &point);
        Q_INVOKABLE bool updateAnchorMove(const QPointF &point);
        Q_INVOKABLE QObjectList commitAnchorMove();
        Q_INVOKABLE bool abortAnchorMove();
        Q_INVOKABLE bool updateAnchorFromPoint(ParameterAnchorViewModel *item, const QPointF &point);

    Q_SIGNALS:
        void freeParameterViewModelChanged();
        void anchorParameterViewModelChanged();
        void originalParameterViewModelChanged();
        void freeTransformParameterViewModelChanged();
        void anchorTransformParameterViewModelChanged();
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void fillModeChanged();
        void fillBaselineChanged();
        void referenceVisibleChanged();
        void referenceBaselineChanged();
        void defaultValueEnabledChanged();
        void defaultValueChanged();
        void originalAndDefaultCurveDisplayModeChanged();
        void editLayerChanged();
        void curveColorChanged();
        void dimmedCurveColorChanged();
        void fillColorChanged();
        void dimmedFillColorChanged();
        void accentColorChanged();
        void referenceColorChanged();
        void selectedAnchorColorChanged();

    protected:
        void updatePolish() override;
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<ParameterEditorQuickItemPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_PARAMETEREDITORQUICKITEM_P_H
