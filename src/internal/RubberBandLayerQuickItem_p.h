#ifndef SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
#define SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H

#include <QQuickItem>
#include <QQmlComponent>

namespace sflow {

    class RubberBandLayerQuickItemPrivate;
    class SelectionController;

    class RubberBandLayerQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(RubberBandLayer)
        Q_DECLARE_PRIVATE(RubberBandLayerQuickItem)
        Q_PROPERTY(SelectionController *selectionController READ selectionController WRITE setSelectionController NOTIFY selectionControllerChanged)
        Q_PROPERTY(QQmlComponent *rubberBand READ rubberBand WRITE setRubberBand NOTIFY rubberBandChanged)
        Q_PROPERTY(bool started READ started NOTIFY startedChanged)

    public:
        explicit RubberBandLayerQuickItem(QQuickItem *parent = nullptr);
        ~RubberBandLayerQuickItem() override;

        SelectionController *selectionController() const;
        void setSelectionController(SelectionController *selectionController);

        QQmlComponent *rubberBand() const;
        void setRubberBand(QQmlComponent *rubberBand);

        bool started() const;

        Q_INVOKABLE void insertItem(QObject *item, const QRectF &rect);
        Q_INVOKABLE void removeItem(QObject *item);

        Q_INVOKABLE void startSelection(const QPointF &startPos);
        Q_INVOKABLE void updateSelection(const QPointF &pos);
        Q_INVOKABLE QRectF endSelection(bool canceled = false);

    signals:
        void selectionControllerChanged();
        void rubberBandChanged();
        void startedChanged(bool started);
        void selectionAboutToEnd(const QRectF &rect);
        void transactionControllerNotifierChanged();

    private:
        QScopedPointer<RubberBandLayerQuickItemPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
