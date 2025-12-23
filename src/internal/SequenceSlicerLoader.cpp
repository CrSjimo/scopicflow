#include "SequenceSlicerLoader_p.h"
#include "SequenceSlicerLoader_p_p.h"

#include <QSet>
#include <QQmlInfo>

namespace sflow {

    SequenceSlicerLoaderContext::SequenceSlicerLoaderContext(QObject *parent) : QObject(parent) {
    }

    SequenceSlicerLoaderContext::~SequenceSlicerLoaderContext() = default;

    bool SequenceSlicerLoaderContext::isInRange() const {
        return m_inRange;
    }

    void SequenceSlicerLoaderContext::setInRange(bool inRange) {
        if (m_inRange == inRange)
            return;
        m_inRange = inRange;
        Q_EMIT inRangeChanged();
    }

    QObject *SequenceSlicerLoaderContext::viewModel() const {
        return m_viewModel;
    }

    void SequenceSlicerLoaderContext::setViewModel(QObject *viewModel) {
        if (m_viewModel == viewModel)
            return;
        m_viewModel = viewModel;
        Q_EMIT viewModelChanged();
    }

    void SequenceSlicerLoaderPrivate::setHandle(SliceableViewModelManipulatorInterface *handle_) {
        Q_Q(SequenceSlicerLoader);
        if (handle == handle_)
            return;
        if (handle)
            QObject::disconnect(handle, nullptr, q, nullptr);
        handle = handle_;
        if (handle) {
            QObject::connect(handle, &SliceableViewModelManipulatorInterface::itemInserted, q, [=](QObject *item) {
                handleItemInserted(item);
            });
            QObject::connect(handle, &SliceableViewModelManipulatorInterface::itemRemoved, q, [=](QObject *item) {
                handleItemRemoved(item);
            });
            QObject::connect(handle, &SliceableViewModelManipulatorInterface::itemUpdated, q, [=](QObject *item) {
                handleItemUpdated(item);
            });
        }
        handleHandleChanged();
    }

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
    QQuickItem *SequenceSlicerLoaderPrivate::createView(QObject *itemModel) {
        Q_Q(SequenceSlicerLoader);
        if (!delegate)
            return nullptr;
        bool visible = inRange(itemModel);
        auto item = qobject_cast<QQuickItem *>(delegate->createWithInitialProperties({
            {"parent", QVariant::fromValue(q->parentItem())},
        }, qmlContext(q)));
        if (!item) {
            qmlWarning(q) << "Failed to create item: " << delegate->errorString();
            return nullptr;
        }
        auto ctx = qobject_cast<SequenceSlicerLoaderContext *>(qmlAttachedPropertiesObject<SequenceSlicerLoader>(item));
        ctx->setViewModel(itemModel);
        ctx->setInRange(visible);
        if (visible) {
            visibleItems.insert(itemModel, item);
        } else {
            invisibleItems.insert(itemModel, item);
        }
        return item;
    }
    void SequenceSlicerLoaderPrivate::showViewIfExistsOrElseCreate(QObject *itemModel) {
        if (invisibleItems.contains(itemModel)) {
            auto item = invisibleItems.value(itemModel);
            auto ctx = qobject_cast<SequenceSlicerLoaderContext *>(qmlAttachedPropertiesObject<SequenceSlicerLoader>(item));
            ctx->setInRange(true);
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
            auto ctx = qobject_cast<SequenceSlicerLoaderContext *>(qmlAttachedPropertiesObject<SequenceSlicerLoader>(item));
            ctx->setInRange(false);
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

    SequenceSlicerLoaderContext *SequenceSlicerLoader::qmlAttachedProperties(QObject *object) {
        return new SequenceSlicerLoaderContext(object);
    }

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
        Q_EMIT delegateChanged();
    }

    QObject * SequenceSlicerLoader::viewModel() const {
        Q_D(const SequenceSlicerLoader);
        return d->viewModel;
    }

    void SequenceSlicerLoader::setViewModel(QObject *viewModel) {
        Q_D(SequenceSlicerLoader);
        if (d->viewModel == viewModel)
            return;
        d->viewModel = viewModel;
        d->setHandle(viewModel ? viewModel->property("iSliceable").value<SliceableViewModelManipulatorInterface *>() : nullptr);
        Q_EMIT viewModelChanged();
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
        Q_EMIT rangeChanged();
    }

    QQuickItem * SequenceSlicerLoader::itemForModel(QObject *model) {
        Q_D(SequenceSlicerLoader);
        if (!model)
            return nullptr;
        if (auto item = d->visibleItems.value(model))
            return item;
        if (auto item = d->invisibleItems.value(model))
            return item;
        return d->createView(model);
    }
}

#include "moc_SequenceSlicerLoader_p.cpp"
