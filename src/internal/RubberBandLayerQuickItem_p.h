#ifndef SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
#define SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H

#include <QQuickItem>
#include <QQmlComponent>

namespace sflow {

    class RubberBandLayerQuickItemPrivate;
    class SelectableViewModelManipulatorInterface;

    class RubberBandLayerQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(RubberBandLayer)
        Q_DECLARE_PRIVATE(RubberBandLayerQuickItem)
        Q_PROPERTY(SelectableViewModelManipulatorInterface *iSelectable READ iSelectable WRITE setISelectable NOTIFY iSelectableChanged)
        Q_PROPERTY(QQmlComponent *rubberBand READ rubberBand WRITE setRubberBand NOTIFY rubberBandChanged)
        Q_PROPERTY(bool started READ started NOTIFY startedChanged)

    public:
        explicit RubberBandLayerQuickItem(QQuickItem *parent = nullptr);
        ~RubberBandLayerQuickItem() override;

        SelectableViewModelManipulatorInterface *iSelectable() const;
        void setISelectable(SelectableViewModelManipulatorInterface *iSelectable);

        QQmlComponent *rubberBand() const;
        void setRubberBand(QQmlComponent *rubberBand);

        bool started() const;

        Q_INVOKABLE void insertItem(const QVariant &item, const QRectF &rect);
        Q_INVOKABLE void removeItem(const QVariant &item);

        Q_INVOKABLE void startSelection(const QPointF &startPos);
        Q_INVOKABLE void updateSelection(const QPointF &pos);
        Q_INVOKABLE QRectF endSelection(bool canceled = false);

    signals:
        void iSelectableChanged();
        void rubberBandChanged();
        void startedChanged(bool started);
        void transactionControllerNotifierChanged();

    private:
        QScopedPointer<RubberBandLayerQuickItemPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
