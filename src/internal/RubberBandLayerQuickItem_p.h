#ifndef SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
#define SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H

#include <QQuickItem>
#include <QQmlComponent>

#include <ScopicFlowInternal/private/SelectableViewModelManipulator_p.h>
#include <ScopicFlow/TransactionControllerNotifier.h>

namespace sflow {

    class RubberBandLayerQuickItemPrivate;

    class RubberBandLayerQuickItem : public QQuickItem {
        Q_OBJECT
        QML_NAMED_ELEMENT(RubberBandLayer)
        Q_DECLARE_PRIVATE(RubberBandLayerQuickItem)
        Q_PROPERTY(SelectableViewModelManipulator *selectionManipulator READ selectionManipulator WRITE setSelectionManipulator NOTIFY selectionManipulatorChanged)
        Q_PROPERTY(QQmlComponent *rubberBand READ rubberBand WRITE setRubberBand NOTIFY rubberBandChanged)
        Q_PROPERTY(bool started READ started NOTIFY startedChanged)
        Q_PROPERTY(TransactionControllerNotifier *transactionControllerNotifier READ transactionControllerNotifier WRITE setTransactionControllerNotifier NOTIFY transactionControllerNotifierChanged)

    public:
        explicit RubberBandLayerQuickItem(QQuickItem *parent = nullptr);
        ~RubberBandLayerQuickItem() override;

        SelectableViewModelManipulator *selectionManipulator() const;
        void setSelectionManipulator(SelectableViewModelManipulator *selectionManipulator);

        QQmlComponent *rubberBand() const;
        void setRubberBand(QQmlComponent *rubberBand);

        bool started() const;

        TransactionControllerNotifier *transactionControllerNotifier() const;
        void setTransactionControllerNotifier(TransactionControllerNotifier *transactionControllerNotifier);

        Q_INVOKABLE void insertItem(const QVariant &item, const QRectF &rect);
        Q_INVOKABLE void removeItem(const QVariant &item);

        Q_INVOKABLE void startSelection(const QPointF &startPos);
        Q_INVOKABLE void updateSelection(const QPointF &pos);
        Q_INVOKABLE QRectF endSelection(bool canceled = false);

    signals:
        void selectionManipulatorChanged();
        void rubberBandChanged();
        void startedChanged(bool started);
        void transactionControllerNotifierChanged();

    private:
        QScopedPointer<RubberBandLayerQuickItemPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
