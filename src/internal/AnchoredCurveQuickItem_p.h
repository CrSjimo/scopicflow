#ifndef SCOPIC_FLOW_ANCHOREDCURVEQUICKITEM_P_H
#define SCOPIC_FLOW_ANCHOREDCURVEQUICKITEM_P_H

#include <QQuickItem>

namespace sflow {

    class TimeViewModel;
    class TimeLayoutViewModel;
    class PointSequenceViewModel;
    class ParameterRangeViewModel;

    class AnchoredCurveQuickItemPrivate;

    class AnchoredCurveQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(AnchoredCurve)
        Q_DECLARE_PRIVATE(AnchoredCurveQuickItem)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(PointSequenceViewModel *anchoredCurveViewModel READ anchoredCurveViewModel WRITE setAnchoredCurveViewModel NOTIFY anchoredCurveViewModelChanged)
        Q_PROPERTY(ParameterRangeViewModel *parameterRangeViewModel READ parameterRangeViewModel WRITE setParameterRangeViewModel NOTIFY parameterRangeViewModelChanged)
        Q_PROPERTY(QList<QColor> strokeColors READ strokeColors WRITE setStrokeColors NOTIFY strokeColorsChanged)
        Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
        Q_PRIVATE_SLOT(d_func(), void handleItemUpdatedSlot())

    public:
        explicit AnchoredCurveQuickItem(QQuickItem *parent = nullptr);
        ~AnchoredCurveQuickItem() override;

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *timeLayoutViewModel);

        PointSequenceViewModel *anchoredCurveViewModel() const;
        void setAnchoredCurveViewModel(PointSequenceViewModel *anchoredCurveViewModel);

        ParameterRangeViewModel *parameterRangeViewModel() const;
        void setParameterRangeViewModel(ParameterRangeViewModel *parameterRangeViewModel);

        QList<QColor> strokeColors() const;
        void setStrokeColors(const QList<QColor> &colors);

        QColor fillColor() const;
        void setFillColor(const QColor &color);

    signals:
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void anchoredCurveViewModelChanged();
        void parameterRangeViewModelChanged();
        void strokeColorsChanged();
        void freeStrokeColorChanged();
        void fillColorChanged();

    protected:
        QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<AnchoredCurveQuickItemPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_ANCHOREDCURVEQUICKITEM_P_H
