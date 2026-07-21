#include "DynamicMixingViewModel.h"
#include "DynamicMixingViewModel_p.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>

#include <QSet>

#include <ScopicFlowCore/DynamicMixingAnchorViewModel.h>

namespace sflow {

    namespace {

        bool isValidRatio(const QList<double> &ratio) {
            double sum = 0.0;
            for (const double value : ratio) {
                if (!std::isfinite(value) || value < 0.0 || value > 1.0) {
                    return false;
                }
                sum += value;
                if (sum > 1.0) {
                    return false;
                }
            }
            return true;
        }

    }

    DynamicMixingViewModelSliceableManipulatorInterface::DynamicMixingViewModelSliceableManipulatorInterface(DynamicMixingViewModelPrivate *d)
        : SliceableViewModelManipulatorInterface(d->q_ptr), d_ptr(d) {
        auto *q = d->q_ptr;
        connect(q, &DynamicMixingViewModel::itemUpdated, this, &SliceableViewModelManipulatorInterface::itemUpdated);
        connect(q, &DynamicMixingViewModel::itemAboutToInsert, this, &SliceableViewModelManipulatorInterface::itemAboutToInsert);
        connect(q, &DynamicMixingViewModel::itemInserted, this, &SliceableViewModelManipulatorInterface::itemInserted);
        connect(q, &DynamicMixingViewModel::itemAboutToRemove, this, &SliceableViewModelManipulatorInterface::itemAboutToRemove);
        connect(q, &DynamicMixingViewModel::itemRemoved, this, &SliceableViewModelManipulatorInterface::itemRemoved);
    }

    DynamicMixingViewModelSliceableManipulatorInterface::~DynamicMixingViewModelSliceableManipulatorInterface() = default;

    QObjectList DynamicMixingViewModelSliceableManipulatorInterface::items() const {
        return d_ptr->q_ptr->items();
    }

    QObjectList DynamicMixingViewModelSliceableManipulatorInterface::slice(int position, int length) const {
        return d_ptr->q_ptr->slice(position, length);
    }

    int DynamicMixingViewModelSliceableManipulatorInterface::itemPosition(QObject *item) const {
        const auto *anchor = qobject_cast<DynamicMixingAnchorViewModel *>(item);
        return anchor ? anchor->position() : 0;
    }

    QObjectList DynamicMixingViewModelSliceableManipulatorInterface::itemsBetween(QObject *item1, QObject *item2) const {
        auto *first = qobject_cast<DynamicMixingAnchorViewModel *>(item1);
        auto *last = qobject_cast<DynamicMixingAnchorViewModel *>(item2);
        const auto firstIt = d_ptr->itemIterators.find(first);
        const auto lastIt = d_ptr->itemIterators.find(last);
        if (!first || !last || firstIt == d_ptr->itemIterators.end()
            || lastIt == d_ptr->itemIterators.end()) {
            return {};
        }
        if (first == last) {
            return {first};
        }

        QObjectList result;
        if (first->position() < last->position()) {
            for (auto it = firstIt->second; it != d_ptr->items.end(); ++it) {
                result.append(it->second);
                if (it == lastIt->second) {
                    break;
                }
            }
            return result;
        }
        if (first->position() > last->position()) {
            auto it = firstIt->second;
            while (true) {
                result.append(it->second);
                if (it == lastIt->second || it == d_ptr->items.begin()) {
                    break;
                }
                --it;
            }
            return result;
        }

        const auto range = d_ptr->items.equal_range(first->position());
        for (auto it = firstIt->second; it != range.second; ++it) {
            result.append(it->second);
            if (it == lastIt->second) {
                return result;
            }
        }
        result.clear();
        auto it = firstIt->second;
        while (true) {
            result.append(it->second);
            if (it == lastIt->second || it == range.first) {
                break;
            }
            --it;
        }
        return result;
    }

    QObject *DynamicMixingViewModelSliceableManipulatorInterface::previousItem(QObject *item) const {
        return d_ptr->q_ptr->previousItem(qobject_cast<DynamicMixingAnchorViewModel *>(item));
    }

    QObject *DynamicMixingViewModelSliceableManipulatorInterface::nextItem(QObject *item) const {
        return d_ptr->q_ptr->nextItem(qobject_cast<DynamicMixingAnchorViewModel *>(item));
    }

    bool DynamicMixingViewModelPrivate::isMovingItem(const DynamicMixingAnchorViewModel *item) const {
        return movingItems.contains(const_cast<DynamicMixingAnchorViewModel *>(item));
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModelPrivate::effectiveItemAtPosition(int position) const {
        if (movingItem && movingItem->position() == position) {
            return movingItem;
        }
        const auto range = items.equal_range(position);
        for (auto it = range.first; it != range.second; ++it) {
            if (isMovingItem(it->second)) {
                return it->second;
            }
        }
        return range.first == range.second ? nullptr : range.first->second;
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModelPrivate::effectiveItemBefore(int position) const {
        auto it = items.lower_bound(position);
        if (it == items.begin()) {
            return nullptr;
        }
        --it;
        return effectiveItemAtPosition(it->first);
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModelPrivate::effectiveItemAfter(int position) const {
        const auto it = items.upper_bound(position);
        return it == items.end() ? nullptr : effectiveItemAtPosition(it->first);
    }

    bool DynamicMixingViewModelPrivate::relocateItem(DynamicMixingAnchorViewModel *item, int position) {
        Q_Q(DynamicMixingViewModel);
        if (!item || position < 0) {
            return false;
        }
        const auto iteratorIt = itemIterators.find(item);
        if (iteratorIt == itemIterators.end()) {
            return false;
        }
        if (!movingItems.empty() && !isMovingItem(item)) {
            return false;
        }
        if (item->position() == position) {
            return true;
        }

        items.erase(iteratorIt->second);
        const auto newIterator = items.emplace(position, item);
        iteratorIt->second = newIterator;
        item->setPositionDirect(position);
        Q_EMIT q->itemUpdated(item);
        Q_EMIT q->itemsChanged();
        return true;
    }

    void DynamicMixingViewModelPrivate::attachItem(DynamicMixingAnchorViewModel *item) {
        Q_Q(DynamicMixingViewModel);
        QObject::connect(item, &DynamicMixingAnchorViewModel::ratioChanged, q, [q, item] {
            Q_EMIT q->itemUpdated(item);
        });
        QObject::connect(item, &DynamicMixingAnchorViewModel::selectedChanged, q, [q, item] {
            Q_EMIT q->itemUpdated(item);
        });
        QObject::connect(item, &QObject::destroyed, q, [this, item] {
            removeItem(item, true);
        });
    }

    bool DynamicMixingViewModelPrivate::removeItem(DynamicMixingAnchorViewModel *item,
                                                    bool itemIsBeingDestroyed,
                                                    bool notifyCollection) {
        Q_Q(DynamicMixingViewModel);
        const auto iteratorIt = itemIterators.find(item);
        if (iteratorIt == itemIterators.end()) {
            return false;
        }
        Q_EMIT q->itemAboutToRemove(item);
        items.erase(iteratorIt->second);
        itemIterators.erase(iteratorIt);
        const bool primaryMovingItemRemoved = movingItem == item;
        movingItems.erase(item);
        if (primaryMovingItemRemoved) {
            movingItem = movingItems.empty() ? nullptr : movingItems.begin()->first;
            Q_EMIT q->movingItemChanged();
        }
        if (!itemIsBeingDestroyed) {
            QObject::disconnect(item, nullptr, q, nullptr);
            item->setDynamicMixingViewModel(nullptr);
        }
        Q_EMIT q->itemRemoved(item);
        if (notifyCollection) {
            Q_EMIT q->countChanged(static_cast<int>(items.size()));
            Q_EMIT q->itemsChanged();
        }
        return true;
    }

    DynamicMixingViewModel::DynamicMixingViewModel(QObject *parent)
        : QObject(parent), d_ptr(new DynamicMixingViewModelPrivate) {
        Q_D(DynamicMixingViewModel);
        d->q_ptr = this;
        d->iSliceable = new DynamicMixingViewModelSliceableManipulatorInterface(d);
    }

    DynamicMixingViewModel::~DynamicMixingViewModel() {
        Q_D(DynamicMixingViewModel);
        for (const auto &[item, iterator] : d->itemIterators) {
            Q_UNUSED(iterator)
            QObject::disconnect(item, nullptr, this, nullptr);
            item->setDynamicMixingViewModel(nullptr);
        }
        d->items.clear();
        d->itemIterators.clear();
        d->movingItems.clear();
        d->movingItem = nullptr;
    }

    int DynamicMixingViewModel::voiceCount() const {
        Q_D(const DynamicMixingViewModel);
        return d->voiceCount;
    }

    void DynamicMixingViewModel::setVoiceCount(int voiceCount) {
        Q_D(DynamicMixingViewModel);
        if (voiceCount < 1 || d->voiceCount == voiceCount) {
            return;
        }
        if (d->movingItem) {
            abortInteractiveMove(d->movingItem);
        }
        d->voiceCount = voiceCount;
        Q_EMIT voiceCountChanged();
    }

    int DynamicMixingViewModel::count() const {
        Q_D(const DynamicMixingViewModel);
        return static_cast<int>(d->items.size());
    }

    QObjectList DynamicMixingViewModel::items() const {
        Q_D(const DynamicMixingViewModel);
        QObjectList result;
        result.reserve(static_cast<qsizetype>(d->items.size()));
        for (const auto &[position, item] : d->items) {
            Q_UNUSED(position)
            result.append(item);
        }
        return result;
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModel::movingItem() const {
        Q_D(const DynamicMixingViewModel);
        return d->movingItem;
    }

    QObjectList DynamicMixingViewModel::slice(int position, int length) const {
        Q_D(const DynamicMixingViewModel);
        if (position < 0 || length < 0) {
            return {};
        }
        QObjectList result;
        const qint64 end = static_cast<qint64>(position) + length;
        const auto first = d->items.lower_bound(position);
        const auto last = end > std::numeric_limits<int>::max()
            ? d->items.end()
            : d->items.lower_bound(static_cast<int>(end));
        for (auto it = first; it != last; ++it) {
            result.append(it->second);
        }
        return result;
    }

    QObjectList DynamicMixingViewModel::effectiveSlice(int position, int length) const {
        Q_D(const DynamicMixingViewModel);
        if (position < 0 || length < 0) {
            return {};
        }
        QObjectList result;
        const qint64 end = static_cast<qint64>(position) + length;
        auto iterator = d->items.lower_bound(position);
        while (iterator != d->items.end()
               && (end > std::numeric_limits<int>::max() || iterator->first < end)) {
            const int itemPosition = iterator->first;
            result.append(d->effectiveItemAtPosition(itemPosition));
            iterator = d->items.upper_bound(itemPosition);
        }
        return result;
    }

    QObjectList DynamicMixingViewModel::itemsAtPosition(int position) const {
        Q_D(const DynamicMixingViewModel);
        QObjectList result;
        const auto range = d->items.equal_range(position);
        for (auto it = range.first; it != range.second; ++it) {
            result.append(it->second);
        }
        return result;
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModel::itemAtPosition(int position) const {
        Q_D(const DynamicMixingViewModel);
        return d->effectiveItemAtPosition(position);
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModel::itemAtOrBeforePosition(int position) const {
        Q_D(const DynamicMixingViewModel);
        const auto at = d->effectiveItemAtPosition(position);
        return at ? at : d->effectiveItemBefore(position);
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModel::itemAtOrAfterPosition(int position) const {
        Q_D(const DynamicMixingViewModel);
        const auto at = d->effectiveItemAtPosition(position);
        return at ? at : d->effectiveItemAfter(position);
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModel::previousItem(DynamicMixingAnchorViewModel *item) const {
        Q_D(const DynamicMixingViewModel);
        const auto iteratorIt = d->itemIterators.find(item);
        if (!item || iteratorIt == d->itemIterators.end() || iteratorIt->second == d->items.begin()) {
            return nullptr;
        }
        auto it = iteratorIt->second;
        return (--it)->second;
    }

    DynamicMixingAnchorViewModel *DynamicMixingViewModel::nextItem(DynamicMixingAnchorViewModel *item) const {
        Q_D(const DynamicMixingViewModel);
        const auto iteratorIt = d->itemIterators.find(item);
        if (!item || iteratorIt == d->itemIterators.end()) {
            return nullptr;
        }
        auto it = iteratorIt->second;
        return ++it == d->items.end() ? nullptr : it->second;
    }

    QList<double> DynamicMixingViewModel::effectiveRatio(const DynamicMixingAnchorViewModel *item) const {
        Q_D(const DynamicMixingViewModel);
        QList<double> result(qMax(0, d->voiceCount - 1), 0.0);
        if (!item) {
            return result;
        }
        const auto source = item->ratio();
        const qsizetype copyCount = std::min(source.size(), result.size());
        for (qsizetype i = 0; i < copyCount; ++i) {
            result[i] = source.at(i);
        }
        return result;
    }

    QList<double> DynamicMixingViewModel::ratioAt(double position) const {
        Q_D(const DynamicMixingViewModel);
        if (!std::isfinite(position) || position < 0.0
            || position > std::numeric_limits<int>::max()) {
            return {};
        }
        if (d->items.empty()) {
            return QList<double>(qMax(0, d->voiceCount - 1),
                                 1.0 / static_cast<double>(d->voiceCount));
        }

        const int flooredPosition = static_cast<int>(std::floor(position));
        if (qFuzzyIsNull(position - flooredPosition)) {
            if (const auto *exactItem = d->effectiveItemAtPosition(flooredPosition)) {
                return effectiveRatio(exactItem);
            }
        }

        const auto upper = d->items.upper_bound(flooredPosition);
        if (upper == d->items.begin()) {
            return effectiveRatio(d->effectiveItemAtPosition(upper->first));
        }
        if (upper == d->items.end()) {
            return effectiveRatio(d->effectiveItemAtPosition(d->items.rbegin()->first));
        }
        auto lower = upper;
        --lower;
        const auto *left = d->effectiveItemAtPosition(lower->first);
        const auto *right = d->effectiveItemAfter(lower->first);
        if (!left || !right || right->position() <= left->position()) {
            return effectiveRatio(left ? left : right);
        }

        const auto leftRatio = effectiveRatio(left);
        const auto rightRatio = effectiveRatio(right);
        const double factor = std::clamp((position - left->position())
                                             / static_cast<double>(right->position() - left->position()),
                                         0.0, 1.0);
        QList<double> result;
        result.reserve(leftRatio.size());
        for (qsizetype i = 0; i < leftRatio.size(); ++i) {
            result.append(leftRatio.at(i) + factor * (rightRatio.at(i) - leftRatio.at(i)));
        }
        return result;
    }

    bool DynamicMixingViewModel::contains(DynamicMixingAnchorViewModel *item) const {
        Q_D(const DynamicMixingViewModel);
        return item && d->itemIterators.contains(item);
    }

    bool DynamicMixingViewModel::insertItem(DynamicMixingAnchorViewModel *item) {
        Q_D(DynamicMixingViewModel);
        if (!item || !d->movingItems.empty() || item->position() < 0
            || item->dynamicMixingViewModel() || d->itemIterators.contains(item)
            || !isValidRatio(item->ratio())) {
            return false;
        }
        Q_EMIT itemAboutToInsert(item);
        const auto iterator = d->items.emplace(item->position(), item);
        d->itemIterators.emplace(item, iterator);
        item->setDynamicMixingViewModel(this);
        d->attachItem(item);
        Q_EMIT itemInserted(item);
        Q_EMIT countChanged(count());
        Q_EMIT itemsChanged();
        return true;
    }

    bool DynamicMixingViewModel::removeItem(DynamicMixingAnchorViewModel *item) {
        Q_D(DynamicMixingViewModel);
        return d->removeItem(item, false);
    }

    bool DynamicMixingViewModel::moveItem(DynamicMixingAnchorViewModel *item, int position) {
        Q_D(DynamicMixingViewModel);
        return d->relocateItem(item, position);
    }

    bool DynamicMixingViewModel::beginInteractiveMove(DynamicMixingAnchorViewModel *item) {
        return beginInteractiveMove(QList<DynamicMixingAnchorViewModel *> {item}, item);
    }

    bool DynamicMixingViewModel::beginInteractiveMove(const QList<DynamicMixingAnchorViewModel *> &items,
                                                       DynamicMixingAnchorViewModel *primaryItem) {
        Q_D(DynamicMixingViewModel);
        if (!primaryItem || items.isEmpty() || !d->movingItems.empty()) {
            return false;
        }
        QSet<DynamicMixingAnchorViewModel *> uniqueItems;
        for (auto *item : items) {
            if (!item || !d->itemIterators.contains(item) || uniqueItems.contains(item)) {
                return false;
            }
            uniqueItems.insert(item);
        }
        if (!uniqueItems.contains(primaryItem)) {
            return false;
        }

        d->movingItems.reserve(static_cast<std::size_t>(items.size()));
        for (auto *item : items) {
            d->movingItems.emplace(item, DynamicMixingViewModelPrivate::InteractiveMoveState {
                item->position(), item->ratio(),
            });
        }
        d->movingItem = primaryItem;
        Q_EMIT movingItemChanged();
        return true;
    }

    bool DynamicMixingViewModel::updateInteractiveMove(
        const QList<DynamicMixingAnchorViewModel *> &items,
        const QList<int> &positions,
        const QList<QList<double>> &ratios) {
        Q_D(DynamicMixingViewModel);
        if (items.size() != positions.size() || items.size() != ratios.size()
            || items.size() != static_cast<qsizetype>(d->movingItems.size())) {
            return false;
        }

        QSet<DynamicMixingAnchorViewModel *> uniqueItems;
        QList<bool> positionChangedItems;
        positionChangedItems.reserve(items.size());
        bool anyPositionChanged = false;
        for (qsizetype i = 0; i < items.size(); ++i) {
            auto *item = items.at(i);
            if (!item || !d->isMovingItem(item) || uniqueItems.contains(item)
                || positions.at(i) < 0 || !isValidRatio(ratios.at(i))) {
                return false;
            }
            uniqueItems.insert(item);
            const bool positionChanged = item->position() != positions.at(i);
            positionChangedItems.append(positionChanged);
            anyPositionChanged |= positionChanged;
        }

        if (anyPositionChanged) {
            for (auto *item : items) {
                d->items.erase(d->itemIterators.at(item));
            }
            for (qsizetype i = 0; i < items.size(); ++i) {
                auto *item = items.at(i);
                const auto iterator = d->items.emplace(positions.at(i), item);
                d->itemIterators.at(item) = iterator;
                item->setPositionDirect(positions.at(i));
            }
        }

        for (qsizetype i = 0; i < items.size(); ++i) {
            auto *item = items.at(i);
            const bool ratioChanged = item->ratio() != ratios.at(i);
            item->setRatio(ratios.at(i));
            if (positionChangedItems.at(i) && !ratioChanged) {
                Q_EMIT itemUpdated(item);
            }
        }
        if (anyPositionChanged) {
            Q_EMIT itemsChanged();
        }
        return true;
    }

    QObjectList DynamicMixingViewModel::commitInteractiveMove(DynamicMixingAnchorViewModel *item) {
        Q_D(DynamicMixingViewModel);
        if (!item || d->movingItem != item || d->movingItems.empty()) {
            return {};
        }

        QObjectList removedItems;
        QSet<DynamicMixingAnchorViewModel *> removedItemSet;
        QSet<int> targetPositions;
        QList<DynamicMixingAnchorViewModel *> movedItems;
        movedItems.reserve(static_cast<qsizetype>(d->movingItems.size()));
        for (const auto &[movingItem, state] : d->movingItems) {
            Q_UNUSED(state)
            movedItems.append(movingItem);
            targetPositions.insert(movingItem->position());
        }
        for (const int position : std::as_const(targetPositions)) {
            const auto range = d->items.equal_range(position);
            for (auto it = range.first; it != range.second; ++it) {
                if (!d->isMovingItem(it->second) && !removedItemSet.contains(it->second)) {
                    removedItemSet.insert(it->second);
                    removedItems.append(it->second);
                }
            }
        }
        for (auto *removedItem : std::as_const(removedItemSet)) {
            d->removeItem(removedItem, false, false);
        }
        if (!removedItemSet.isEmpty()) {
            Q_EMIT countChanged(count());
            Q_EMIT itemsChanged();
        }
        d->movingItems.clear();
        d->movingItem = nullptr;
        Q_EMIT movingItemChanged();
        for (auto *movedItem : std::as_const(movedItems)) {
            Q_EMIT itemUpdated(movedItem);
        }
        return removedItems;
    }

    bool DynamicMixingViewModel::abortInteractiveMove(DynamicMixingAnchorViewModel *item) {
        Q_D(DynamicMixingViewModel);
        if (!item || d->movingItem != item || d->movingItems.empty()) {
            return false;
        }

        QList<DynamicMixingAnchorViewModel *> items;
        QList<int> positions;
        QList<QList<double>> ratios;
        items.reserve(static_cast<qsizetype>(d->movingItems.size()));
        positions.reserve(static_cast<qsizetype>(d->movingItems.size()));
        ratios.reserve(static_cast<qsizetype>(d->movingItems.size()));
        for (const auto &[movingItem, state] : d->movingItems) {
            items.append(movingItem);
            positions.append(state.position);
            ratios.append(state.ratio);
        }
        if (!updateInteractiveMove(items, positions, ratios)) {
            return false;
        }
        d->movingItems.clear();
        d->movingItem = nullptr;
        Q_EMIT movingItemChanged();
        return true;
    }

}

#include "moc_DynamicMixingViewModel.cpp"
#include "moc_DynamicMixingViewModel_p.cpp"
