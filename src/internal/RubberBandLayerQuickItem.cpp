#include "RubberBandLayerQuickItem_p.h"

#include <QSizeF>

#include <ScopicFlowInternal/private/RubberBandLayerQuickItem_p_p.h>

namespace sflow {

    RubberBandLayerQuickItem::RubberBandLayerQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new RubberBandLayerQuickItemPrivate) {
        Q_D(RubberBandLayerQuickItem);
        d->q_ptr = this;
    }
    RubberBandLayerQuickItem::~RubberBandLayerQuickItem() = default;

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
        Q_EMIT rubberBandChanged();
    }
    bool RubberBandLayerQuickItem::started() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->started;
    }

    QRectF RubberBandLayerQuickItem::selectionRect() const {
        Q_D(const RubberBandLayerQuickItem);
        return d->selectionRect;
    }

    void RubberBandLayerQuickItem::startSelection(const QPointF &startPos) {
        Q_D(RubberBandLayerQuickItem);
        if (d->started)
            return;
        d->startPos = startPos;
        d->endPos = startPos;
        d->selectionRect = QRectF(startPos, QSizeF());
        d->started = true;
        if (d->rubberBandItem) {
            d->rubberBandItem->setVisible(true);
            d->rubberBandItem->setX(startPos.x());
            d->rubberBandItem->setY(startPos.y());
            d->rubberBandItem->setWidth(0);
            d->rubberBandItem->setHeight(0);
        }
        Q_EMIT selectionRectChanged();
        Q_EMIT startedChanged(true);
    }
    void RubberBandLayerQuickItem::updateSelection(const QPointF &pos) {
        Q_D(RubberBandLayerQuickItem);
        d->endPos = pos;
        d->selectionRect = QRectF(
            qMin(d->startPos.x(), d->endPos.x()),
            qMin(d->startPos.y(), d->endPos.y()),
            qAbs(d->endPos.x() - d->startPos.x()),
            qAbs(d->endPos.y() - d->startPos.y()));
        if (!d->started)
            return;
        if (d->rubberBandItem) {
            d->rubberBandItem->setX(d->selectionRect.x());
            d->rubberBandItem->setY(d->selectionRect.y());
            d->rubberBandItem->setWidth(d->selectionRect.width());
            d->rubberBandItem->setHeight(d->selectionRect.height());
        }
        Q_EMIT selectionRectChanged();
    }
    QRectF RubberBandLayerQuickItem::endSelection() {
        Q_D(RubberBandLayerQuickItem);
        if (!d->started)
            return {};
        const QRectF selectionRect = d->selectionRect;
        d->started = false;
        if (d->rubberBandItem) {
            d->rubberBandItem->setVisible(false);
        }
        Q_EMIT startedChanged(false);
        return selectionRect;
    }
}

#include "moc_RubberBandLayerQuickItem_p.cpp"
