#include "AnchorParameterViewModel.h"
#include "AnchorParameterViewModel_p.h"

#include <algorithm>
#include <cmath>
#include <limits>

#include <QSet>
#include <QVector>

#include <ScopicFlowCore/ParameterAnchorViewModel.h>

namespace sflow {

    namespace {

        double internalSlope(const ParameterAnchorViewModel *left,
                             const ParameterAnchorViewModel *center,
                             const ParameterAnchorViewModel *right) {
            const double leftWidth = center->position() - left->position();
            const double rightWidth = right->position() - center->position();
            const double leftDelta = (center->value() - left->value()) / leftWidth;
            const double rightDelta = (right->value() - center->value()) / rightWidth;
            if (leftDelta * rightDelta <= 0.0) {
                return 0.0;
            }
            const double weight1 = 2.0 * rightWidth + leftWidth;
            const double weight2 = rightWidth + 2.0 * leftWidth;
            return (weight1 + weight2) / (weight1 / leftDelta + weight2 / rightDelta);
        }

        double evaluateHermite(double position,
                               const ParameterAnchorViewModel *left,
                               const ParameterAnchorViewModel *right,
                               const ParameterAnchorViewModel *previous,
                               const ParameterAnchorViewModel *next) {
            const double x1 = left->position();
            const double y1 = left->value();
            const double x2 = right->position();
            const double y2 = right->value();
            const double d1 = previous ? internalSlope(previous, left, right) : (y2 - y1) / (x2 - x1);
            const double d2 = next ? internalSlope(left, right, next) : (y2 - y1) / (x2 - x1);
            const double width = x2 - x1;
            const double normalizedPosition = (position - x1) / width;
            const double normalizedPosition2 = normalizedPosition * normalizedPosition;
            const double normalizedPosition3 = normalizedPosition2 * normalizedPosition;
            return (2.0 * normalizedPosition3 - 3.0 * normalizedPosition2 + 1.0) * y1
                 + (normalizedPosition3 - 2.0 * normalizedPosition2 + normalizedPosition) * width * d1
                 + (-2.0 * normalizedPosition3 + 3.0 * normalizedPosition2) * y2
                 + (normalizedPosition3 - normalizedPosition2) * width * d2;
        }

    }

    AnchorParameterViewModelSliceableManipulatorInterface::AnchorParameterViewModelSliceableManipulatorInterface(AnchorParameterViewModelPrivate *d)
        : SliceableViewModelManipulatorInterface(d->q_ptr), d_ptr(d) {
        auto *q = d->q_ptr;
        connect(q, &AnchorParameterViewModel::itemUpdated, this, &SliceableViewModelManipulatorInterface::itemUpdated);
        connect(q, &AnchorParameterViewModel::itemAboutToInsert, this, &SliceableViewModelManipulatorInterface::itemAboutToInsert);
        connect(q, &AnchorParameterViewModel::itemInserted, this, &SliceableViewModelManipulatorInterface::itemInserted);
        connect(q, &AnchorParameterViewModel::itemAboutToRemove, this, &SliceableViewModelManipulatorInterface::itemAboutToRemove);
        connect(q, &AnchorParameterViewModel::itemRemoved, this, &SliceableViewModelManipulatorInterface::itemRemoved);
    }

    AnchorParameterViewModelSliceableManipulatorInterface::~AnchorParameterViewModelSliceableManipulatorInterface() = default;

    QObjectList AnchorParameterViewModelSliceableManipulatorInterface::items() const {
        return d_ptr->q_ptr->items();
    }

    QObjectList AnchorParameterViewModelSliceableManipulatorInterface::slice(int position, int length) const {
        return d_ptr->q_ptr->slice(position, length);
    }

    int AnchorParameterViewModelSliceableManipulatorInterface::itemPosition(QObject *item) const {
        const auto *anchor = qobject_cast<ParameterAnchorViewModel *>(item);
        return anchor ? anchor->position() : 0;
    }

    QObjectList AnchorParameterViewModelSliceableManipulatorInterface::itemsBetween(QObject *item1, QObject *item2) const {
        auto *first = qobject_cast<ParameterAnchorViewModel *>(item1);
        auto *last = qobject_cast<ParameterAnchorViewModel *>(item2);
        if (!first || !last || !d_ptr->itemIterators.contains(first) || !d_ptr->itemIterators.contains(last)) {
            return {};
        }

        QObjectList result;
        if (first->position() <= last->position()) {
            for (auto *item = first; item; item = d_ptr->q_ptr->nextItem(item)) {
                result.append(item);
                if (item == last || item->position() >= last->position()) {
                    break;
                }
            }
        } else {
            for (auto *item = first; item; item = d_ptr->q_ptr->previousItem(item)) {
                result.append(item);
                if (item == last || item->position() <= last->position()) {
                    break;
                }
            }
        }
        return result;
    }

    QObject *AnchorParameterViewModelSliceableManipulatorInterface::previousItem(QObject *item) const {
        return d_ptr->q_ptr->previousItem(qobject_cast<ParameterAnchorViewModel *>(item));
    }

    QObject *AnchorParameterViewModelSliceableManipulatorInterface::nextItem(QObject *item) const {
        return d_ptr->q_ptr->nextItem(qobject_cast<ParameterAnchorViewModel *>(item));
    }

    bool AnchorParameterViewModelPrivate::isMovingItem(const ParameterAnchorViewModel *item) const {
        return movingItems.contains(const_cast<ParameterAnchorViewModel *>(item));
    }

    ParameterAnchorViewModel *AnchorParameterViewModelPrivate::effectiveItemAtPosition(int position) const {
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

    ParameterAnchorViewModel *AnchorParameterViewModelPrivate::effectiveItemBefore(int position) const {
        auto it = items.lower_bound(position);
        if (it == items.begin()) {
            return nullptr;
        }
        --it;
        return effectiveItemAtPosition(it->first);
    }

    ParameterAnchorViewModel *AnchorParameterViewModelPrivate::effectiveItemAfter(int position) const {
        const auto it = items.upper_bound(position);
        return it == items.end() ? nullptr : effectiveItemAtPosition(it->first);
    }

    bool AnchorParameterViewModelPrivate::relocateItem(ParameterAnchorViewModel *item, int position) {
        Q_Q(AnchorParameterViewModel);
        if (!item || position < 0) {
            return false;
        }
        const auto iteratorIt = itemIterators.find(item);
        if (iteratorIt == itemIterators.end()) {
            return false;
        }
        const bool interactiveItem = isMovingItem(item);
        if (!movingItems.empty() && !interactiveItem) {
            return false;
        }
        if (item->position() == position) {
            return true;
        }
        if (!interactiveItem) {
            const auto range = items.equal_range(position);
            if (range.first != range.second) {
                return false;
            }
        }

        items.erase(iteratorIt->second);
        const auto newIterator = items.emplace(position, item);
        iteratorIt->second = newIterator;
        item->setPositionDirect(position);
        Q_EMIT q->itemUpdated(item);
        Q_EMIT q->itemsChanged();
        return true;
    }

    void AnchorParameterViewModelPrivate::attachItem(ParameterAnchorViewModel *item) {
        Q_Q(AnchorParameterViewModel);
        QObject::connect(item, &ParameterAnchorViewModel::valueChanged, q, [q, item] {
            Q_EMIT q->itemUpdated(item);
        });
        QObject::connect(item, &ParameterAnchorViewModel::interpolationModeChanged, q, [q, item] {
            Q_EMIT q->itemUpdated(item);
        });
        QObject::connect(item, &ParameterAnchorViewModel::selectedChanged, q, [q, item] {
            Q_EMIT q->itemUpdated(item);
        });
        QObject::connect(item, &QObject::destroyed, q, [this, item] {
            removeItem(item, true);
        });
    }

    bool AnchorParameterViewModelPrivate::removeItem(ParameterAnchorViewModel *item, bool itemIsBeingDestroyed) {
        Q_Q(AnchorParameterViewModel);
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
            item->setAnchorParameterViewModel(nullptr);
        }
        Q_EMIT q->itemRemoved(item);
        Q_EMIT q->countChanged(static_cast<int>(items.size()));
        Q_EMIT q->itemsChanged();
        return true;
    }

    AnchorParameterViewModel::AnchorParameterViewModel(QObject *parent)
        : QObject(parent), d_ptr(new AnchorParameterViewModelPrivate) {
        Q_D(AnchorParameterViewModel);
        d->q_ptr = this;
        d->iSliceable = new AnchorParameterViewModelSliceableManipulatorInterface(d);
    }

    AnchorParameterViewModel::~AnchorParameterViewModel() {
        Q_D(AnchorParameterViewModel);
        for (const auto &[item, iterator] : d->itemIterators) {
            Q_UNUSED(iterator)
            QObject::disconnect(item, nullptr, this, nullptr);
            item->setAnchorParameterViewModel(nullptr);
        }
        d->items.clear();
        d->itemIterators.clear();
        d->movingItem = nullptr;
        d->movingItems.clear();
    }

    int AnchorParameterViewModel::count() const {
        Q_D(const AnchorParameterViewModel);
        return static_cast<int>(d->items.size());
    }

    QObjectList AnchorParameterViewModel::items() const {
        Q_D(const AnchorParameterViewModel);
        QObjectList result;
        result.reserve(static_cast<qsizetype>(d->items.size()));
        for (const auto &[position, item] : d->items) {
            Q_UNUSED(position)
            result.append(item);
        }
        return result;
    }

    ParameterAnchorViewModel *AnchorParameterViewModel::movingItem() const {
        Q_D(const AnchorParameterViewModel);
        return d->movingItem;
    }

    QObjectList AnchorParameterViewModel::slice(int position, int length) const {
        Q_D(const AnchorParameterViewModel);
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

    QObjectList AnchorParameterViewModel::effectiveSlice(int position, int length) const {
        Q_D(const AnchorParameterViewModel);
        if (position < 0 || length < 0) {
            return {};
        }
        QObjectList result;
        const qint64 end = static_cast<qint64>(position) + length;
        auto iterator = d->items.lower_bound(position);
        while (iterator != d->items.end() && (end > std::numeric_limits<int>::max() || iterator->first < end)) {
            const int itemPosition = iterator->first;
            result.append(d->effectiveItemAtPosition(itemPosition));
            iterator = d->items.upper_bound(itemPosition);
        }
        return result;
    }

    ParameterAnchorViewModel *AnchorParameterViewModel::itemAtPosition(int position) const {
        Q_D(const AnchorParameterViewModel);
        return d->effectiveItemAtPosition(position);
    }

    ParameterAnchorViewModel *AnchorParameterViewModel::previousItem(ParameterAnchorViewModel *item) const {
        Q_D(const AnchorParameterViewModel);
        if (!item || !d->itemIterators.contains(item)) {
            return nullptr;
        }
        return d->effectiveItemBefore(item->position());
    }

    ParameterAnchorViewModel *AnchorParameterViewModel::nextItem(ParameterAnchorViewModel *item) const {
        Q_D(const AnchorParameterViewModel);
        if (!item || !d->itemIterators.contains(item)) {
            return nullptr;
        }
        return d->effectiveItemAfter(item->position());
    }

    QVariant AnchorParameterViewModel::valueAt(double position) const {
        Q_D(const AnchorParameterViewModel);
        if (!std::isfinite(position) || position < 0.0
            || position > std::numeric_limits<int>::max() || d->items.empty()) {
            return {};
        }

        const int flooredPosition = static_cast<int>(std::floor(position));
        if (qFuzzyIsNull(position - flooredPosition)) {
            if (const auto *exactItem = d->effectiveItemAtPosition(flooredPosition)) {
                return exactItem->value();
            }
        }

        const auto upper = d->items.upper_bound(flooredPosition);
        if (upper == d->items.begin()) {
            return {};
        }
        auto lower = upper;
        --lower;
        const auto *left = d->effectiveItemAtPosition(lower->first);
        const auto *right = d->effectiveItemAfter(left->position());
        if (!left || !right || position <= left->position() || position >= right->position()) {
            return {};
        }
        if (left->interpolationMode() == ParameterAnchorViewModel::None) {
            return {};
        }
        if (left->interpolationMode() == ParameterAnchorViewModel::Linear) {
            const double ratio = (position - left->position()) / (right->position() - left->position());
            return left->value() + ratio * (right->value() - left->value());
        }
        return evaluateHermite(position, left, right,
                               d->effectiveItemBefore(left->position()),
                               d->effectiveItemAfter(right->position()));
    }

    QList<QVariant> AnchorParameterViewModel::valuesAt(const QList<double> &sortedPositions) const {
        Q_D(const AnchorParameterViewModel);
        QList<QVariant> result(sortedPositions.size(), QVariant());
        if (sortedPositions.isEmpty() || d->items.empty()) {
            return result;
        }

        bool isSorted = true;
        for (qsizetype i = 0; i < sortedPositions.size(); ++i) {
            const double position = sortedPositions.at(i);
            if (!std::isfinite(position)
                || (i > 0 && position < sortedPositions.at(i - 1))) {
                isSorted = false;
                break;
            }
        }
        if (!isSorted) {
            for (qsizetype i = 0; i < sortedPositions.size(); ++i) {
                result[i] = valueAt(sortedPositions.at(i));
            }
            return result;
        }

        const double minimumPosition = sortedPositions.constFirst();
        const double maximumPosition = sortedPositions.constLast();
        if (maximumPosition < 0.0 || minimumPosition > std::numeric_limits<int>::max()) {
            return result;
        }
        const int rangeStart = static_cast<int>(std::floor(std::max(0.0, minimumPosition)));
        const int rangeEnd = maximumPosition > std::numeric_limits<int>::max()
            ? std::numeric_limits<int>::max()
            : static_cast<int>(std::ceil(std::max(0.0, maximumPosition)));

        auto iterator = d->items.lower_bound(rangeStart);
        for (int i = 0; i < 2 && iterator != d->items.begin(); ++i) {
            --iterator;
            iterator = d->items.lower_bound(iterator->first);
        }

        QVector<ParameterAnchorViewModel *> anchors;
        int positionsPastRange = 0;
        while (iterator != d->items.end()) {
            const int anchorPosition = iterator->first;
            if (anchorPosition > rangeEnd && ++positionsPastRange > 2) {
                break;
            }
            anchors.append(d->effectiveItemAtPosition(anchorPosition));
            iterator = d->items.upper_bound(anchorPosition);
        }

        qsizetype rightIndex = 0;
        for (qsizetype i = 0; i < sortedPositions.size(); ++i) {
            const double position = sortedPositions.at(i);
            if (position < 0.0 || position > std::numeric_limits<int>::max()) {
                continue;
            }
            const int flooredPosition = static_cast<int>(std::floor(position));
            const bool isIntegerPosition = qFuzzyIsNull(position - flooredPosition);
            const double comparisonPosition = isIntegerPosition ? flooredPosition : position;
            while (rightIndex < anchors.size()
                   && anchors.at(rightIndex)->position() < comparisonPosition) {
                ++rightIndex;
            }
            if (rightIndex < anchors.size() && anchors.at(rightIndex)->position() == flooredPosition
                && isIntegerPosition) {
                result[i] = anchors.at(rightIndex)->value();
                continue;
            }
            if (rightIndex == 0 || rightIndex >= anchors.size()) {
                continue;
            }
            const auto *left = anchors.at(rightIndex - 1);
            const auto *right = anchors.at(rightIndex);
            if (left->interpolationMode() == ParameterAnchorViewModel::None) {
                continue;
            }
            if (left->interpolationMode() == ParameterAnchorViewModel::Linear) {
                const double ratio = (position - left->position()) / (right->position() - left->position());
                result[i] = left->value() + ratio * (right->value() - left->value());
                continue;
            }
            result[i] = evaluateHermite(position, left, right,
                                        rightIndex >= 2 ? anchors.at(rightIndex - 2) : nullptr,
                                        rightIndex + 1 < anchors.size() ? anchors.at(rightIndex + 1) : nullptr);
        }
        return result;
    }

    bool AnchorParameterViewModel::insertItem(ParameterAnchorViewModel *item) {
        Q_D(AnchorParameterViewModel);
        if (!item || !d->movingItems.empty() || item->position() < 0
            || item->anchorParameterViewModel() || d->itemIterators.contains(item)) {
            return false;
        }
        if (d->items.find(item->position()) != d->items.end()) {
            return false;
        }
        Q_EMIT itemAboutToInsert(item);
        const auto iterator = d->items.emplace(item->position(), item);
        d->itemIterators.emplace(item, iterator);
        item->setAnchorParameterViewModel(this);
        d->attachItem(item);
        Q_EMIT itemInserted(item);
        Q_EMIT countChanged(count());
        Q_EMIT itemsChanged();
        return true;
    }

    bool AnchorParameterViewModel::removeItem(ParameterAnchorViewModel *item) {
        Q_D(AnchorParameterViewModel);
        return d->removeItem(item, false);
    }

    bool AnchorParameterViewModel::moveItem(ParameterAnchorViewModel *item, int position) {
        Q_D(AnchorParameterViewModel);
        return d->relocateItem(item, position);
    }

    bool AnchorParameterViewModel::beginInteractiveMove(ParameterAnchorViewModel *item) {
        return beginInteractiveMove(QList<ParameterAnchorViewModel *> {item}, item);
    }

    bool AnchorParameterViewModel::beginInteractiveMove(const QList<ParameterAnchorViewModel *> &items,
                                                         ParameterAnchorViewModel *primaryItem) {
        Q_D(AnchorParameterViewModel);
        if (!primaryItem || items.isEmpty() || !d->movingItems.empty()) {
            return false;
        }

        QSet<ParameterAnchorViewModel *> uniqueItems;
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
            d->movingItems.emplace(item, AnchorParameterViewModelPrivate::InteractiveMoveState {
                item->position(), item->value(),
            });
        }
        d->movingItem = primaryItem;
        Q_EMIT movingItemChanged();
        return true;
    }

    bool AnchorParameterViewModel::updateInteractiveMove(const QList<ParameterAnchorViewModel *> &items,
                                                          const QList<int> &positions,
                                                          const QList<double> &values) {
        Q_D(AnchorParameterViewModel);
        if (items.size() != positions.size() || items.size() != values.size()
            || items.size() != static_cast<qsizetype>(d->movingItems.size())) {
            return false;
        }

        QSet<ParameterAnchorViewModel *> uniqueItems;
        QSet<int> uniquePositions;
        QList<bool> positionChangedItems;
        positionChangedItems.reserve(items.size());
        bool anyPositionChanged = false;
        for (qsizetype i = 0; i < items.size(); ++i) {
            auto *item = items.at(i);
            const int position = positions.at(i);
            const double value = values.at(i);
            if (!item || !d->isMovingItem(item) || uniqueItems.contains(item)
                || position < 0 || uniquePositions.contains(position) || !std::isfinite(value)) {
                return false;
            }
            uniqueItems.insert(item);
            uniquePositions.insert(position);
            const bool positionChanged = item->position() != position;
            positionChangedItems.append(positionChanged);
            anyPositionChanged |= positionChanged;
        }

        if (anyPositionChanged) {
            for (auto *item : items) {
                d->items.erase(d->itemIterators.at(item));
            }
            for (qsizetype i = 0; i < items.size(); ++i) {
                auto *item = items.at(i);
                const int position = positions.at(i);
                const auto iterator = d->items.emplace(position, item);
                d->itemIterators.at(item) = iterator;
                item->setPositionDirect(position);
            }
        }

        for (qsizetype i = 0; i < items.size(); ++i) {
            auto *item = items.at(i);
            const bool valueChanged = item->value() != values.at(i);
            item->setValue(values.at(i));
            if (positionChangedItems.at(i) && !valueChanged) {
                Q_EMIT itemUpdated(item);
            }
        }
        if (anyPositionChanged) {
            Q_EMIT itemsChanged();
        }
        return true;
    }

    QObjectList AnchorParameterViewModel::commitInteractiveMove(ParameterAnchorViewModel *item) {
        Q_D(AnchorParameterViewModel);
        if (!item || d->movingItem != item || d->movingItems.empty()) {
            return {};
        }

        QObjectList removedItems;
        QSet<ParameterAnchorViewModel *> removedItemSet;
        QList<ParameterAnchorViewModel *> movedItems;
        movedItems.reserve(static_cast<qsizetype>(d->movingItems.size()));
        for (const auto &[movingItem, state] : d->movingItems) {
            Q_UNUSED(state)
            movedItems.append(movingItem);
            const auto range = d->items.equal_range(movingItem->position());
            for (auto it = range.first; it != range.second; ++it) {
                if (!d->isMovingItem(it->second) && !removedItemSet.contains(it->second)) {
                    removedItemSet.insert(it->second);
                    removedItems.append(it->second);
                }
            }
        }
        for (auto *removedItem : removedItems) {
            d->removeItem(qobject_cast<ParameterAnchorViewModel *>(removedItem), false);
        }
        d->movingItems.clear();
        d->movingItem = nullptr;
        Q_EMIT movingItemChanged();
        for (auto *movedItem : movedItems) {
            Q_EMIT itemUpdated(movedItem);
        }
        return removedItems;
    }

    bool AnchorParameterViewModel::abortInteractiveMove(ParameterAnchorViewModel *item) {
        Q_D(AnchorParameterViewModel);
        if (!item || d->movingItem != item || d->movingItems.empty()) {
            return false;
        }

        QList<ParameterAnchorViewModel *> items;
        QList<int> positions;
        QList<double> values;
        items.reserve(static_cast<qsizetype>(d->movingItems.size()));
        positions.reserve(static_cast<qsizetype>(d->movingItems.size()));
        values.reserve(static_cast<qsizetype>(d->movingItems.size()));
        for (const auto &[movingItem, state] : d->movingItems) {
            items.append(movingItem);
            positions.append(state.position);
            values.append(state.value);
        }
        if (!updateInteractiveMove(items, positions, values)) {
            return false;
        }

        d->movingItems.clear();
        d->movingItem = nullptr;
        Q_EMIT movingItemChanged();
        return true;
    }

}

#include "moc_AnchorParameterViewModel.cpp"
#include "moc_AnchorParameterViewModel_p.cpp"
