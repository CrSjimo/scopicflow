#ifndef SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H
#define SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H

#include <map>
#include <algorithm>

#include <QHash>
#include <QSet>
#include <QVariant>

#include <ScopicFlow/PointSequenceViewModel.h>
#include <ScopicFlow/private/SliceableViewModelQmlHandle_p.h>

namespace sflow {

    struct PointSequenceContainer {
        std::multimap<int, QObject *> m_items;
        QHash<QObject *, int> m_positions;

        QObject *firstItem() const {
            return m_items.cbegin()->second;
        }

        QObject *lastItem() const {
            return m_items.crbegin()->second;
        }

        QObject *previousItem(QObject *item) const {
            auto it = std::find_if(m_items.find(m_positions.value(item)), m_items.end(), [=](auto it) {
                return it.second == item;
            });
            if (it == m_items.end()) {
                return nullptr;
            }
            if (it == m_items.begin()) {
                return nullptr;
            }
            return (--it)->second;
        }

        QObject *nextItem(QObject *item) const {
            auto it = std::find_if(m_items.find(m_positions.value(item)), m_items.end(), [=](auto it) {
                return it.second == item;
            });
            if (it == m_items.end()) {
                return nullptr;
            }
            if (++it == m_items.end()) {
                return nullptr;
            }
            return it->second;
        }

        void insertItem(QObject *item, int position) {
            if (m_positions.contains(item)) {
                m_items.erase(std::find_if(m_items.find(m_positions.value(item)), m_items.end(), [=](auto it) {
                    return it.second == item;
                }));
            }
            m_items.insert({position, item});
            m_positions.insert(item, position);
        }

        void removeItem(QObject *item) {
            m_items.erase(std::find_if(m_items.find(m_positions.value(item)), m_items.end(), [=](auto it) {
                return it.second == item;
            }));
            m_positions.remove(item);
        }

        QObjectList slice(int position, int length) {
            QObjectList ret;
            std::transform(m_items.lower_bound(position), m_items.lower_bound(position + length), std::back_inserter(ret), [](auto it) {
                return it.second;
            });
            return ret;
        }
    };

    class PointSequenceViewModelQmlHandle : public SliceableViewModelQmlHandle {
        Q_OBJECT
        Q_PROPERTY(QObject *currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
        Q_PROPERTY(bool intermediate READ intermediate WRITE setIntermediate NOTIFY intermediateChanged)
    public:
        Q_DECLARE_PRIVATE(PointSequenceViewModel)
        explicit PointSequenceViewModelQmlHandle(PointSequenceViewModelPrivate *d);
        ~PointSequenceViewModelQmlHandle() override;

        QObject *currentItem() const;
        void setCurrentItem(QObject *item);

        bool intermediate() const;
        void setIntermediate(bool intermediate);

        Q_INVOKABLE void insertItem(QObject *item);
        Q_INVOKABLE void removeItem(QObject *item);
        Q_INVOKABLE QObjectList slice(int position, int length) override;
        Q_INVOKABLE int itemPosition(QObject *item) const override;

    signals:
        void currentItemChanged();
        void intermediateChanged();

    private:
        PointSequenceViewModelPrivate *d_ptr;

    };

    class PointSequenceViewModelPrivate {
        Q_DECLARE_PUBLIC(PointSequenceViewModel)
    public:
        PointSequenceViewModel *q_ptr;
        PointSequenceViewModelQmlHandle *handle{};

        QByteArray selectedProperty;
        QByteArray positionProperty;

        PointSequenceContainer container;
        QSet<QObject *> selection;
        QObject *currentItem{};
        bool intermediate{};

        void insertItem(QObject *item);
        void removeItem(QObject *item);

        void handleItemSelectedChanged();
        void handleItemPositionChanged();
    };

}

#endif //SCOPIC_FLOW_POINTSEQUENCEVIEWMODEL_P_H
