#include "RubberBandLayerQuickItem_p.h"
#include "RubberBandLayerQuickItem_p_p.h"

namespace sflow {

    namespace {
        auto _ = qmlRegisterType<RubberBandLayerQuickItem>("dev.sjimo.ScopicFlow.Private.Internal", 1, 0, "RubberBandLayer");
    }

    RubberBandLayerQuickItem::RubberBandLayerQuickItem(QQuickItem *parent) : d_ptr(new RubberBandLayerQuickItemPrivate) {
    }
    RubberBandLayerQuickItem::~RubberBandLayerQuickItem() = default;
    SelectableViewModelManipulator *RubberBandLayerQuickItem::selectionManipulator() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->selectionManipulator;
    }
    void RubberBandLayerQuickItem::setSelectionManipulator(SelectableViewModelManipulator *selectionManipulator) {
        Q_D(RubberBandLayerQuickItem);
        if (d->selectionManipulator == selectionManipulator)
            return;
        d->selectionManipulator = selectionManipulator;
        emit selectionManipulatorChanged();
    }
    QQmlComponent *RubberBandLayerQuickItem::rubberBand() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->rubberBandComponent;
    }
    void RubberBandLayerQuickItem::setRubberBand(QQmlComponent *rubberBand) {
        Q_D(RubberBandLayerQuickItem);
        d->rubberBandComponent = rubberBand;
        delete d->rubberBandItem;
        d->rubberBandItem = nullptr;
        if (rubberBand) {
            auto obj = rubberBand->create(qmlContext(this));
            auto item = qobject_cast<QQuickItem *>(obj);
            item->setParentItem(this);
            d->rubberBandItem = item;
        }
        emit rubberBandChanged();
    }
    bool RubberBandLayerQuickItem::started() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->started;
    }
    void RubberBandLayerQuickItem::insertItem(QObject *item, const QRectF &rect) {
        Q_D(RubberBandLayerQuickItem);
        d->itemRects.insert(item, rect);
    }
    void RubberBandLayerQuickItem::removeItem(QObject *item) {
        Q_D(RubberBandLayerQuickItem);
        d->itemRects.remove(item);
        d->taggedItems.remove(item);
    }
    void RubberBandLayerQuickItem::startSelection(const QPointF &startPos) {
        Q_D(RubberBandLayerQuickItem);
        if (d->started)
            return;
        d->startPos = startPos;
        d->started = true;
        if (d->rubberBandItem) {
            d->rubberBandItem->setVisible(true);
            d->rubberBandItem->setX(startPos.x());
            d->rubberBandItem->setY(startPos.y());
            d->rubberBandItem->setWidth(0);
            d->rubberBandItem->setHeight(0);
        }
        emit startedChanged(true);
    }
    void RubberBandLayerQuickItem::updateSelection(const QPointF &pos) {
        Q_D(RubberBandLayerQuickItem);
        if (!d->started)
            return;
        d->endPos = pos;
        if (d->rubberBandItem) {
            d->rubberBandItem->setX(qMin(d->startPos.x(), d->endPos.x()));
            d->rubberBandItem->setY(qMin(d->startPos.y(), d->endPos.y()));
            d->rubberBandItem->setWidth(qAbs(d->endPos.x() - d->startPos.x()));
            d->rubberBandItem->setHeight(qAbs(d->endPos.y() - d->startPos.y()));
        }
        if (!d->selectionManipulator || !d->selectionManipulator->interface())
            return;
        auto selectionInterface = d->selectionManipulator->interface();
        QRectF rubberBandRect(d->rubberBandItem->x(), d->rubberBandItem->y(), d->rubberBandItem->width(), d->rubberBandItem->height());
        // TODO: Current implementation is high in time complexity. Optimize it in future
        // Step 1: toggle-select ALL(not covered by rubber band && tagged)
        QList<QObject *> disjointItems;
        for (auto item : d->taggedItems) {
            auto itemRect = d->itemRects.value(item);
            if (!rubberBandRect.intersects(itemRect)) {
                selectionInterface->setSelected(item, !selectionInterface->isSelected(item));
                disjointItems.append(item);
            }
        }
        // Step 2: remove tag from ALL(not covered by rubber band && tagged)
        for (auto item : disjointItems) {
            d->taggedItems.remove(item);
        }
        // Step 3: toggle-select ALL(covered by rubber band && not tagged)
        for (auto p = d->itemRects.constKeyValueBegin(); p != d->itemRects.constKeyValueEnd(); p++) {
            auto [item, itemRect] = *p;
            if (!d->taggedItems.contains(item) && rubberBandRect.intersects(itemRect)) {
                selectionInterface->setSelected(item, !selectionInterface->isSelected(item));
                // Step 4: tag ALL(covered by rubber band && not tagged)
                d->taggedItems.insert(item);
            }
        }
    }
    void RubberBandLayerQuickItem::endSelection() {
        Q_D(RubberBandLayerQuickItem);
        if (!d->started)
            return;
        d->started = false;
        if (d->rubberBandItem) {
            d->rubberBandItem->setVisible(false);
        }
        d->taggedItems.clear();
        emit startedChanged(false);
    }
}