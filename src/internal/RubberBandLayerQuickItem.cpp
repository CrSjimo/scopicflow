#include "RubberBandLayerQuickItem_p.h"
#include "RubberBandLayerQuickItem_p_p.h"

#include <ScopicFlowCore/SelectionController.h>

namespace sflow {

    RubberBandLayerQuickItem::RubberBandLayerQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new RubberBandLayerQuickItemPrivate) {
        Q_D(RubberBandLayerQuickItem);
        d->q_ptr = this;
    }
    RubberBandLayerQuickItem::~RubberBandLayerQuickItem() = default;

    SelectionController *RubberBandLayerQuickItem::selectionController() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->selectionController;
    }

    void RubberBandLayerQuickItem::setSelectionController(SelectionController *selectionController) {
        Q_D(RubberBandLayerQuickItem);
        d->selectionController = selectionController;
        emit selectionControllerChanged();
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
    void RubberBandLayerQuickItem::insertItem(QObject *item, const QRectF &rect) {
        Q_D(RubberBandLayerQuickItem);
        if (!d->selectionController)
            return;
        if (!item)
            return;
        d->itemRects.insert(item, rect);
        connect(item, &QObject::destroyed, this, [this, item] { removeItem(item); });
    }
    void RubberBandLayerQuickItem::removeItem(QObject *item) {
        Q_D(RubberBandLayerQuickItem);
        if (!d->selectionController)
            return;
        if (!item)
            return;
        d->itemRects.remove(item);
        d->taggedItems.remove(item);
        disconnect(item, nullptr, this, nullptr);
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
    }
    QRectF RubberBandLayerQuickItem::endSelection(bool canceled) {
        Q_D(RubberBandLayerQuickItem);
        if (!d->started)
            return {};
        QRectF rubberBandRect = {d->rubberBandItem->x(), d->rubberBandItem->y(), d->rubberBandItem->width(), d->rubberBandItem->height()};
        do {
            if (!d->selectionController || canceled)
                break;
            Q_EMIT selectionAboutToEnd(rubberBandRect);
            // TODO: Current implementation is high in time complexity. Optimize it in future
            // Step 1: toggle-select ALL(not covered by rubber band && tagged)
            QList<QObject *> disjointItems;
            for (auto item : d->taggedItems) {
                auto itemRect = d->itemRects.value(item);
                if (!rubberBandRect.intersects(itemRect)) {
                    d->selectionController->select(item, SelectionController::Toggle);
                    disjointItems.append(item);
                }
            }
            // Step 2: remove tag from ALL(not covered by rubber band && tagged)
            for (auto itemId : disjointItems) {
                d->taggedItems.remove(itemId);
            }
            // Step 3: toggle-select ALL(covered by rubber band && not tagged)
            for (const auto &[item, itemRect] : d->itemRects.asKeyValueRange()) {
                if (!d->taggedItems.contains(item) && rubberBandRect.intersects(itemRect)) {
                    d->selectionController->select(item, SelectionController::Toggle);
                    // Step 4: tag ALL(covered by rubber band && not tagged)
                    d->taggedItems.insert(item);
                }
            }
        } while (false);
        d->started = false;
        if (d->rubberBandItem) {
            d->rubberBandItem->setVisible(false);
        }
        d->taggedItems.clear();
        emit startedChanged(false);
        return rubberBandRect;
    }
}

#include "moc_RubberBandLayerQuickItem_p.cpp"
