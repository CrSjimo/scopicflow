#ifndef SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_P_H
#define SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_P_H

#include <algorithm>

#include <QSet>
#include <QHash>

#include <interval-tree/interval_tree.hpp>

#include <ScopicFlow/RangeSequenceViewModel.h>
#include <ScopicFlow/private/PointSequenceViewModel_p.h>


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

    class SCOPIC_FLOW_EXPORT RangeSequenceViewModelQmlHandle : public SliceableViewModelQmlHandle {
        Q_OBJECT
        Q_PROPERTY(QObject *currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
        Q_PROPERTY(bool intermediate READ isIntermediate WRITE setIntermediate NOTIFY intermediateChanged)
        Q_PROPERTY(QObjectList selection READ selection NOTIFY selectionChanged)

    public:
        Q_DECLARE_PRIVATE(RangeSequenceViewModel)
        explicit RangeSequenceViewModelQmlHandle(RangeSequenceViewModelPrivate *d);
        ~RangeSequenceViewModelQmlHandle() override;

        QObject *currentItem() const;
        void setCurrentItem(QObject *item);

        bool isIntermediate() const;
        void setIntermediate(bool intermediate);

        Q_INVOKABLE QObject *previousItem(QObject *item) const;
        Q_INVOKABLE QObject *nextItem(QObject *item) const;
        Q_INVOKABLE QObject *firstItem() const;
        Q_INVOKABLE QObject *lastItem() const;

        QObjectList items() const override;

        QObjectList selection() const;

        Q_INVOKABLE void insertItem(QObject *item);
        Q_INVOKABLE void removeItem(QObject *item);
        Q_INVOKABLE QObjectList slice(int position, int length) const override;
        Q_INVOKABLE int itemPosition(QObject *item) const override;
        Q_INVOKABLE int itemLength(QObject *item) const override;

    signals:
        void currentItemChanged();
        void intermediateChanged();
        void selectionChanged();

    private:
        RangeSequenceViewModelPrivate *d_ptr;
    };

    class RangeSequenceViewModelPrivate {
        Q_DECLARE_PUBLIC(RangeSequenceViewModel)
    public:
        RangeSequenceViewModel *q_ptr;
        RangeSequenceViewModelQmlHandle *handle{};

        QByteArray selectedProperty;
        QByteArray positionProperty;
        QByteArray lengthProperty;
        QByteArray overlappedProperty;

        PointSequenceContainer pointSequenceContainer;
        RangeSequenceContainer rangeSequenceContainer;

        QSet<QObject *> selection;
        QObject *currentItem{};
        bool intermediate{};

        void insertItem(QObject *item);
        void removeItem(QObject *item);

        void updateOverlapped(const QObjectList &affectedItems) const;

        void handleItemSelectedChanged();
        void handleItemPositionChanged();
        void handleItemLengthChanged();
    };

}

#endif //SCOPIC_FLOW_RANGESEQUENCEVIEWMODEL_P_H
