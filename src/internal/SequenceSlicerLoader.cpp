#include "SequenceSlicerLoader_p.h"
#include "SequenceSlicerLoader_p_p.h"

#include <QSet>

namespace sflow {

    void SequenceSlicerLoaderPrivate::handleRangeChanged() {
        if (!handle)
            return;
        auto slicedItemModels = handle->slice(range.first, range.second - range.first + 1);
        QSet slicedItemModelSet(slicedItemModels.begin(), slicedItemModels.end());
        for (auto itemModel : visibleItems.keys()) {
            if (slicedItemModelSet.contains(itemModel))
                continue;
            hideView(itemModel);
        }
        for (auto itemModel : slicedItemModels) {
            showViewIfExistsOrElseCreate(itemModel);
        }
    }
    void SequenceSlicerLoaderPrivate::handleDelegateChanged() {
        auto itemModels = visibleItems.keys();
        itemModels << invisibleItems.keys();
        for (auto itemModel : itemModels) {
            destroyView(itemModel);
            createView(itemModel);
        }
    }
    void SequenceSlicerLoaderPrivate::handleHandleChanged() {
        auto itemModels = visibleItems.keys();
        itemModels << invisibleItems.keys();
        for (auto itemModel : itemModels) {
            destroyView(itemModel);
        }
        handleRangeChanged();
    }
    constexpr static bool rangeIntersects(QPair<int, int> range1, QPair<int, int> range2) {
        return std::max(range1.first, range2.first) <= std::min(range1.second, range2.second);
    }
    void SequenceSlicerLoaderPrivate::handleItemInserted(QObject *itemModel) {
        if (inRange(itemModel)) {
            createView(itemModel);
        }
    }
    void SequenceSlicerLoaderPrivate::handleItemRemoved(QObject *itemModel) {
        destroyView(itemModel);
    }
    void SequenceSlicerLoaderPrivate::handleItemUpdated(QObject *itemModel) {
        if (inRange(itemModel)) {
            showViewIfExistsOrElseCreate(itemModel);
        } else {
            hideView(itemModel);
        }
    }
    void SequenceSlicerLoaderPrivate::createView(QObject *itemModel) {
        Q_Q(SequenceSlicerLoader);
        if (!delegate)
            return;
        bool visible = inRange(itemModel);
        auto item = qobject_cast<QQuickItem *>(delegate->createWithInitialProperties({
            {"model", QVariant::fromValue(itemModel)},
            {"parent", QVariant::fromValue(q->parentItem())},
            {"visible", visible}
        }, qmlContext(q)));
        if (visible) {
            visibleItems.insert(itemModel, item);
        } else {
            invisibleItems.insert(itemModel, item);
        }
    }
    void SequenceSlicerLoaderPrivate::showViewIfExistsOrElseCreate(QObject *itemModel) {
        if (invisibleItems.contains(itemModel)) {
            auto item = invisibleItems.value(itemModel);
            item->setVisible(true);
            invisibleItems.remove(itemModel);
            visibleItems.insert(itemModel, item);
        } else if (!visibleItems.contains(itemModel)) {
            createView(itemModel);
        }
    }
    void SequenceSlicerLoaderPrivate::destroyView(QObject *itemModel) {
        if (visibleItems.contains(itemModel)) {
            visibleItems.value(itemModel)->deleteLater();
            visibleItems.remove(itemModel);
        } else if (invisibleItems.contains(itemModel)) {
            invisibleItems.value(itemModel)->deleteLater();
            invisibleItems.remove(itemModel);
        }
    }
    void SequenceSlicerLoaderPrivate::hideView(QObject *itemModel) {
        if (visibleItems.contains(itemModel)) {
            auto item = visibleItems.value(itemModel);
            item->setVisible(false);
            visibleItems.remove(itemModel);
            invisibleItems.insert(itemModel, item);
        }
    }
    bool SequenceSlicerLoaderPrivate::inRange(QObject *itemModel) const {
        auto itemRangeStart = handle->itemPosition(itemModel);
        auto itemRangeEnd = handle->itemPosition(itemModel) + handle->itemLength(itemModel) - 1;
        return rangeIntersects({itemRangeStart, itemRangeEnd}, range);
    }

    SequenceSlicerLoader::SequenceSlicerLoader(QQuickItem *parent) : QQuickItem(parent), d_ptr(new SequenceSlicerLoaderPrivate) {
        Q_D(SequenceSlicerLoader);
        d->q_ptr = this;
    }
    SequenceSlicerLoader::~SequenceSlicerLoader() = default;
    QQmlComponent *SequenceSlicerLoader::delegate() const {
        Q_D(const SequenceSlicerLoader);
        return d->delegate;
    }
    void SequenceSlicerLoader::setDelegate(QQmlComponent *delegate) {
        Q_D(SequenceSlicerLoader);
        if (d->delegate == delegate)
            return;
        d->delegate = delegate;
        d->handleDelegateChanged();
        emit delegateChanged();
    }
    QObject *SequenceSlicerLoader::handle() const {
        Q_D(const SequenceSlicerLoader);
        return d->handle;
    }
    void SequenceSlicerLoader::setHandle(QObject *handle) {
        Q_D(SequenceSlicerLoader);
        if (d->handle == handle)
            return;
        if (d->handle)
            disconnect(d->handle, nullptr, this, nullptr);
        d->handle = qobject_cast<SliceableViewModelQmlHandle *>(handle);
        if (d->handle) {
            connect(d->handle, &SliceableViewModelQmlHandle::itemInserted, this, [=](QObject *item) {
                d->handleItemInserted(item);
            });
            connect(d->handle, &SliceableViewModelQmlHandle::itemRemoved, this, [=](QObject *item) {
                d->handleItemRemoved(item);
            });
            connect(d->handle, &SliceableViewModelQmlHandle::itemUpdated, this, [=](QObject *item) {
                d->handleItemUpdated(item);
            });
        }
        d->handleHandleChanged();
        emit handleChanged();
    }
    QList<int> SequenceSlicerLoader::range() const {
        Q_D(const SequenceSlicerLoader);
        return {d->range.first, d->range.second};
    }
    void SequenceSlicerLoader::setRange(const QList<int> &range) {
        Q_D(SequenceSlicerLoader);
        auto p = range.isEmpty() ? qMakePair(0, 0) : qMakePair(range.first(), range.last());
        if (d->range == p)
            return;
        d->range = p;
        d->handleRangeChanged();
        emit rangeChanged();
    }
}