#include "RubberBandLayerQuickItem_p.h"
#include "RubberBandLayerQuickItem_p_p.h"

#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>

namespace sflow {

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
            if (item)
                item->setParentItem(this);
            d->rubberBandItem = item;
        }
        emit rubberBandChanged();
    }
    bool RubberBandLayerQuickItem::started() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->started;
    }
    void RubberBandLayerQuickItem::insertItem(const QVariant &item, const QRectF &rect) {
        Q_D(RubberBandLayerQuickItem);
        if (!d->selectionManipulator || !d->selectionManipulator->interface())
            return;
        d->itemRects.insert(d->selectionManipulator->interface()->getId(item), rect);
    }
    void RubberBandLayerQuickItem::removeItem(const QVariant &item) {
        Q_D(RubberBandLayerQuickItem);
        if (!d->selectionManipulator || !d->selectionManipulator->interface())
            return;
        auto id = d->selectionManipulator->interface()->getId(item);
        d->itemRects.remove(id);
        d->taggedItems.remove(id);
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
        if (d->selectionManipulator && d->selectionManipulator->interface()) {
            d->selectionManipulator->interface()->viewModel()->setProperty("intermediate", true);
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
        QList<qsizetype> disjointItemIds;
        for (auto itemId : d->taggedItems) {
            auto itemRect = d->itemRects.value(itemId);
            if (!rubberBandRect.intersects(itemRect)) {
                auto item = d->selectionManipulator->interface()->fromId(itemId);
                selectionInterface->setSelected(item, !selectionInterface->isSelected(item));
                disjointItemIds.append(itemId);
            }
        }
        // Step 2: remove tag from ALL(not covered by rubber band && tagged)
        for (auto itemId : disjointItemIds) {
            d->taggedItems.remove(itemId);
        }
        // Step 3: toggle-select ALL(covered by rubber band && not tagged)
        for (auto p = d->itemRects.constKeyValueBegin(); p != d->itemRects.constKeyValueEnd(); p++) {
            auto [itemId, itemRect] = *p;
            if (!d->taggedItems.contains(itemId) && rubberBandRect.intersects(itemRect)) {
                auto item = d->selectionManipulator->interface()->fromId(itemId);
                selectionInterface->setSelected(item, !selectionInterface->isSelected(item));
                // Step 4: tag ALL(covered by rubber band && not tagged)
                d->taggedItems.insert(itemId);
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
        if (d->selectionManipulator && d->selectionManipulator->interface()) {
            d->selectionManipulator->interface()->viewModel()->setProperty("intermediate", false);
        }
        emit startedChanged(false);
    }
}