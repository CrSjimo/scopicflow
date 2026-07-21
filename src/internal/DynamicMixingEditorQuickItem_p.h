#ifndef SCOPIC_FLOW_DYNAMICMIXINGEDITORQUICKITEM_P_H
#define SCOPIC_FLOW_DYNAMICMIXINGEDITORQUICKITEM_P_H

#include <QColor>
#include <QList>
#include <QQuickItem>
#include <QVariantMap>

namespace sflow {

    class DynamicMixingAnchorViewModel;
    class DynamicMixingViewModel;
    class TimeLayoutViewModel;
    class TimeViewModel;
    class DynamicMixingEditorQuickItemPrivate;

    class DynamicMixingEditorQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(DynamicMixingEditorContent)
        Q_DECLARE_PRIVATE(DynamicMixingEditorQuickItem)

        Q_PROPERTY(DynamicMixingViewModel *dynamicMixingViewModel READ dynamicMixingViewModel WRITE setDynamicMixingViewModel NOTIFY dynamicMixingViewModelChanged)
        Q_PROPERTY(TimeViewModel *timeViewModel READ timeViewModel WRITE setTimeViewModel NOTIFY timeViewModelChanged)
        Q_PROPERTY(TimeLayoutViewModel *timeLayoutViewModel READ timeLayoutViewModel WRITE setTimeLayoutViewModel NOTIFY timeLayoutViewModelChanged)
        Q_PROPERTY(QList<QColor> colors READ colors WRITE setColors NOTIFY colorsChanged)
        Q_PROPERTY(QColor boundaryLineColor READ boundaryLineColor WRITE setBoundaryLineColor NOTIFY boundaryLineColorChanged)
        Q_PROPERTY(QColor anchorLineColor READ anchorLineColor WRITE setAnchorLineColor NOTIFY anchorLineColorChanged)
        Q_PROPERTY(QColor selectedAnchorLineColor READ selectedAnchorLineColor WRITE setSelectedAnchorLineColor NOTIFY selectedAnchorLineColorChanged)
        Q_PROPERTY(QColor handleColor READ handleColor WRITE setHandleColor NOTIFY handleColorChanged)
        Q_PROPERTY(QColor selectedHandleBorderColor READ selectedHandleBorderColor WRITE setSelectedHandleBorderColor NOTIFY selectedHandleBorderColorChanged)

    public:
        explicit DynamicMixingEditorQuickItem(QQuickItem *parent = nullptr);
        ~DynamicMixingEditorQuickItem() override;

        DynamicMixingViewModel *dynamicMixingViewModel() const;
        void setDynamicMixingViewModel(DynamicMixingViewModel *viewModel);
        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *viewModel);
        TimeLayoutViewModel *timeLayoutViewModel() const;
        void setTimeLayoutViewModel(TimeLayoutViewModel *viewModel);

        QList<QColor> colors() const;
        void setColors(const QList<QColor> &colors);
        QColor boundaryLineColor() const;
        void setBoundaryLineColor(const QColor &color);
        QColor anchorLineColor() const;
        void setAnchorLineColor(const QColor &color);
        QColor selectedAnchorLineColor() const;
        void setSelectedAnchorLineColor(const QColor &color);
        QColor handleColor() const;
        void setHandleColor(const QColor &color);
        QColor selectedHandleBorderColor() const;
        void setSelectedHandleBorderColor(const QColor &color);

        Q_INVOKABLE QVariantMap hitTest(const QPointF &point, double radius) const;
        Q_INVOKABLE DynamicMixingAnchorViewModel *anchorAt(const QPointF &point, double radius) const;
        Q_INVOKABLE QObjectList anchorsInTimeRange(int firstPosition, int lastPosition) const;
        Q_INVOKABLE bool beginAnchorMove(const QObjectList &items,
                                         DynamicMixingAnchorViewModel *primaryItem,
                                         int handleIndex,
                                         const QPointF &point);
        Q_INVOKABLE bool updateAnchorMove(const QPointF &point, int targetPosition);
        Q_INVOKABLE QObjectList commitAnchorMove();
        Q_INVOKABLE bool abortAnchorMove();

    Q_SIGNALS:
        void dynamicMixingViewModelChanged();
        void timeViewModelChanged();
        void timeLayoutViewModelChanged();
        void colorsChanged();
        void boundaryLineColorChanged();
        void anchorLineColorChanged();
        void selectedAnchorLineColorChanged();
        void handleColorChanged();
        void selectedHandleBorderColorChanged();

    protected:
        void updatePolish() override;
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<DynamicMixingEditorQuickItemPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_DYNAMICMIXINGEDITORQUICKITEM_P_H
