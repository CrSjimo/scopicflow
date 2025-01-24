#include "SequenceSlicerLoader_p.h"
#include "SequenceSlicerLoader_p_p.h"

#include <QSet>

namespace sflow {

    void SequenceSlicerLoaderPrivate::handleRangeChanged() {
        if (!handle)
            return;
        auto itemModels = handle->slice(range.first, range.second - range.first + 1);
        QSet itemModelSet(itemModels.begin(), itemModels.end());
        for (auto itemModel : items.keys()) {
            if (itemModelSet.contains(itemModel))
                continue;
            delete items.value(itemModel);
            items.remove(itemModel);
        }
        for (auto itemModel : itemModels) {
            if (items.contains(itemModel))
                continue;
            items.insert(itemModel, createView(itemModel));
        }
    }
    void SequenceSlicerLoaderPrivate::handleDelegateChanged() {
        auto itemModels = items.keys();
        for (auto itemModel : itemModels) {
            items.insert(itemModel, createView(itemModel));
        }
    }
    void SequenceSlicerLoaderPrivate::handleHandleChanged() {
        auto itemModels = items.keys();
        for (auto itemModel : itemModels) {
            delete items.value(itemModel);
        }
        items.clear();
        handleRangeChanged();
    }
    constexpr static bool rangeIntersects(QPair<int, int> range1, QPair<int, int> range2) {
        return std::max(range1.first, range2.first) <= std::min(range1.second, range2.second);
    }
    void SequenceSlicerLoaderPrivate::handleItemInserted(QObject *itemModel) {
        auto pos = handle->itemPosition(itemModel);
        auto len = handle->itemLength(itemModel);
        if (rangeIntersects(range, {pos, pos + len - 1})) {
            items.insert(itemModel, createView(itemModel));
        }
    }
    void SequenceSlicerLoaderPrivate::handleItemRemoved(QObject *itemModel) {
        if (items.contains(itemModel)) {
            delete items.value(itemModel);
            items.remove(itemModel);
        }
    }
    void SequenceSlicerLoaderPrivate::handleItemUpdated(QObject *itemModel) {
        auto pos = handle->itemPosition(itemModel);
        auto len = handle->itemLength(itemModel);
        if (rangeIntersects(range, {pos, pos + len - 1})) {
            if (!items.contains(itemModel)) {
                items.insert(itemModel, createView(itemModel));
            }
        } else {
            if (items.contains(itemModel)) {
                delete items.value(itemModel);
                items.remove(itemModel);
            }
        }
    }
    QQuickItem *SequenceSlicerLoaderPrivate::createView(QObject *itemModel) {
        Q_Q(SequenceSlicerLoader);
        if (!delegate)
            return nullptr;
        auto item = qobject_cast<QQuickItem *>(delegate->createWithInitialProperties({
            {"model", QVariant::fromValue(itemModel)},
            {"parent", QVariant::fromValue(q->parentItem())}
        }, qmlContext(q)));
        return item;
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
        connect(d->handle, &SliceableViewModelQmlHandle::itemInserted, this, [=](QObject *item) {
            d->handleItemInserted(item);
        });
        connect(d->handle, &SliceableViewModelQmlHandle::itemRemoved, this, [=](QObject *item) {
            d->handleItemRemoved(item);
        });
        connect(d->handle, &SliceableViewModelQmlHandle::itemUpdated, this, [=](QObject *item) {
            d->handleItemUpdated(item);
        });
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