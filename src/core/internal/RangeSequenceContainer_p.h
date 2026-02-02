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
        QHash<QObject *, Interval> m_intervals;
        lib_interval_tree::interval_tree<Interval> m_intervalTree;

        void insertItem(QObject *item, int position, int length) {
            if (m_intervals.contains(item)) {
                m_intervalTree.erase(m_intervalTree.find(m_intervals.value(item)));
            }
            Interval interval(position, length, item);
            m_intervalTree.overlap_find_all(interval, [&](const auto &it) {
                auto overlappedItem = it.interval().item();
                return true;
            });
            m_intervalTree.insert(interval);
            m_intervals.insert(item, interval);
        }

        void removeItem(QObject *item) {
            if (m_intervals.contains(item)) {
                m_intervalTree.erase(m_intervalTree.find(m_intervals.value(item)));
                m_intervals.remove(item);
            }
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
