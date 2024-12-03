#ifndef SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
#define SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H

#include <QQuickItem>
#include <QQmlComponent>

#include <ScopicFlow/private/SelectableViewModelManipulator_p.h>

namespace sflow {

    class RubberBandLayerQuickItemPrivate;

    class RubberBandLayerQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(RubberBandLayerQuickItem)
        Q_PROPERTY(SelectableViewModelManipulator *selectionManipulator READ selectionManipulator WRITE setSelectionManipulator NOTIFY selectionManipulatorChanged)
        Q_PROPERTY(QQmlComponent *rubberBand READ rubberBand WRITE setRubberBand NOTIFY rubberBandChanged)
        Q_PROPERTY(bool started READ started NOTIFY startedChanged)

    public:
        explicit RubberBandLayerQuickItem(QQuickItem *parent = nullptr);
        ~RubberBandLayerQuickItem() override;

        SelectableViewModelManipulator *selectionManipulator() const;
        void setSelectionManipulator(SelectableViewModelManipulator *selectionManipulator);

        QQmlComponent *rubberBand() const;
        void setRubberBand(QQmlComponent *rubberBand);

        bool started() const;

        Q_INVOKABLE void insertItem(QObject *item, const QRectF &rect);
        Q_INVOKABLE void removeItem(QObject *item);

        Q_INVOKABLE void startSelection(const QPointF &startPos);
        Q_INVOKABLE void updateSelection(const QPointF &pos);
        Q_INVOKABLE void endSelection();

    signals:
        void selectionManipulatorChanged();
        void rubberBandChanged();
        void startedChanged(bool started);

    private:
        QScopedPointer<RubberBandLayerQuickItemPrivate> d_ptr;
    };

}

#endif //SCOPIC_FLOW_RUBBERBANDLAYERQUICKITEM_P_H
