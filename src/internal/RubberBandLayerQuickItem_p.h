#ifndef SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
#define SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H

#include <QQuickItem>
#include <QRectF>
#include <QQmlComponent>

namespace sflow {

    class RubberBandLayerQuickItemPrivate;

    class RubberBandLayerQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(RubberBandLayer)
        Q_DECLARE_PRIVATE(RubberBandLayerQuickItem)
        Q_PROPERTY(QQmlComponent *rubberBand READ rubberBand WRITE setRubberBand NOTIFY rubberBandChanged)
        Q_PROPERTY(bool started READ started NOTIFY startedChanged)
        Q_PROPERTY(QRectF selectionRect READ selectionRect NOTIFY selectionRectChanged)

    public:
        explicit RubberBandLayerQuickItem(QQuickItem *parent = nullptr);
        ~RubberBandLayerQuickItem() override;

        QQmlComponent *rubberBand() const;
        void setRubberBand(QQmlComponent *rubberBand);

        bool started() const;
        QRectF selectionRect() const;

        Q_INVOKABLE void startSelection(const QPointF &startPos);
        Q_INVOKABLE void updateSelection(const QPointF &pos);
        Q_INVOKABLE QRectF endSelection();

    Q_SIGNALS:
        void rubberBandChanged();
        void startedChanged(bool started);
        void selectionRectChanged();

    private:
        QScopedPointer<RubberBandLayerQuickItemPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
