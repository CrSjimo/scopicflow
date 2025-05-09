#include "RubberBandLayerQuickItem_p.h"
#include "RubberBandLayerQuickItem_p_p.h"

#include <ScopicFlow/private/SelectableViewModelManipulatorInterface_p.h>

namespace sflow {

    RubberBandLayerQuickItem::RubberBandLayerQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new RubberBandLayerQuickItemPrivate) {
        Q_D(RubberBandLayerQuickItem);
        d->q_ptr = this;
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
    TransactionControllerNotifier *RubberBandLayerQuickItem::transactionControllerNotifier() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->transactionControllerNotifier;
    }
    void RubberBandLayerQuickItem::setTransactionControllerNotifier(TransactionControllerNotifier *transactionControllerNotifier) {
        Q_D(RubberBandLayerQuickItem);
        if (d->transactionControllerNotifier == transactionControllerNotifier)
            return;
        d->transactionControllerNotifier = transactionControllerNotifier;
        emit transactionControllerNotifierChanged();
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
        if (d->transactionControllerNotifier)
            emit d->transactionControllerNotifier->transactionAboutToBegin();
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
        QRectF rubberBandRect(
            qMin(d->startPos.x(), d->endPos.x()),
            qMin(d->startPos.y(), d->endPos.y()),
            qAbs(d->endPos.x() - d->startPos.x()),
            qAbs(d->endPos.y() - d->startPos.y()));
        if (d->rubberBandItem) {
            d->rubberBandItem->setX(rubberBandRect.x());
            d->rubberBandItem->setY(rubberBandRect.y());
            d->rubberBandItem->setWidth(rubberBandRect.width());
            d->rubberBandItem->setHeight(rubberBandRect.height());
        }
        if (!d->selectionManipulator || !d->selectionManipulator->interface())
            return;
        auto selectionInterface = d->selectionManipulator->interface();
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
    QRectF RubberBandLayerQuickItem::endSelection(bool canceled) {
        Q_D(RubberBandLayerQuickItem);
        if (!d->started)
            return {};
        d->started = false;
        if (d->rubberBandItem) {
            d->rubberBandItem->setVisible(false);
        }
        d->taggedItems.clear();
        if (d->selectionManipulator && d->selectionManipulator->interface()) {
            d->selectionManipulator->interface()->viewModel()->setProperty("intermediate", false);
        }
        if (d->transactionControllerNotifier) {
            if (canceled)
                emit d->transactionControllerNotifier->transactionAborted();
            else
                emit d->transactionControllerNotifier->transactionCommitted();
        }
        emit startedChanged(false);
        return {d->rubberBandItem->x(), d->rubberBandItem->y(), d->rubberBandItem->width(), d->rubberBandItem->height()};
    }
}