#ifndef SCOPIC_FLOW_RANGESEQUENCECONTAINER_P_H
#define SCOPIC_FLOW_RANGESEQUENCECONTAINER_P_H

#include <algorithm>

#include <QSet>
#include <QHash>
#include <QObject>

#include <interval-tree/interval_tree.hpp>

namespace sflow {

    struct RangeSequenceContainer {
        struct Interval : lib_interval_tree::interval<int> {
            constexpr Interval() : interval(0, 0), m_item(nullptr) {
            }
            constexpr Interval(int position, int length, QObject *item) : lib_interval_tree::interval<int>(position, position + std::max(length, 1) - 1), m_item(item) {
            }
            constexpr QObject *item() const {
                return m_item;
            }
            constexpr bool operator==(const Interval &other) const {
                return m_item == other.m_item;
            }
        private:
            QObject *m_item;
        };
        QHash<QObject *, QSet<QObject *>> m_overlapList;
        QHash<QObject *, Interval> m_intervals;
        lib_interval_tree::interval_tree<Interval> m_intervalTree;

        QObjectList insertItem(QObject *item, int position, int length) {
            QSet<QObject *> affectedItems;
            if (m_intervals.contains(item)) {
                m_intervalTree.erase(m_intervalTree.find(m_intervals.value(item)));
                for (auto overlappedItem : m_overlapList.value(item)) {
                    affectedItems.insert(overlappedItem);
                    m_overlapList[overlappedItem].remove(item);
                }
                m_overlapList[item].clear();
            }
            Interval interval(position, length, item);
            m_overlapList.insert(item, {});
            m_intervalTree.overlap_find_all(interval, [&](const auto &it) {
                auto overlappedItem = it.interval().item();
                if (affectedItems.contains(overlappedItem)) {
                    affectedItems.remove(overlappedItem);
                } else {
                    affectedItems.insert(overlappedItem);
                }
                m_overlapList[item].insert(overlappedItem);
                m_overlapList[overlappedItem].insert(item);
                return true;
            });
            m_intervalTree.insert(interval);
            m_intervals.insert(item, interval);
            affectedItems.insert(item);
            return QObjectList(affectedItems.cbegin(), affectedItems.cend());
        }

        QObjectList removeItem(QObject *item) {
            QSet<QObject *> affectedItems;
            if (m_intervals.contains(item)) {
                m_intervalTree.erase(m_intervalTree.find(m_intervals.value(item)));
                for (auto overlappedItem : m_overlapList.value(item)) {
                    affectedItems.insert(overlappedItem);
                    m_overlapList[overlappedItem].remove(item);
                }
                m_overlapList.remove(item);
                m_intervals.remove(item);
            }
            return QObjectList(affectedItems.cbegin(), affectedItems.cend());
        }

        bool isOverlapped(QObject *item) const {
            return !m_overlapList.value(item).isEmpty();
        }

        QObjectList slice(int position, int length) const {
            QObjectList result;
            Interval interval(position, length, nullptr);
            m_intervalTree.overlap_find_all(interval, [&](const auto &it) {
                auto overlappedItem = it.interval().item();
                result.append(overlappedItem);
                return true;
            });
            return result;
        }

    };

}

#endif //SCOPIC_FLOW_RANGESEQUENCECONTAINER_P_H
