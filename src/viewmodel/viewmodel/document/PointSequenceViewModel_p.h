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

        QObjectList slice(int position, int length) const {
            QObjectList ret;
            std::transform(m_items.lower_bound(position), m_items.lower_bound(position + length), std::back_inserter(ret), [](auto it) {
                return it.second;
            });
            return ret;
        }
    };

    class SCOPIC_FLOW_EXPORT PointSequenceViewModelQmlHandle : public SliceableViewModelQmlHandle {
        Q_OBJECT
        Q_PROPERTY(QObject *currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
        Q_PROPERTY(bool intermediate READ isIntermediate WRITE setIntermediate NOTIFY intermediateChanged)
        Q_PROPERTY(QObjectList selection READ selection NOTIFY selectionChanged)
    public:
        Q_DECLARE_PRIVATE(PointSequenceViewModel)
        explicit PointSequenceViewModelQmlHandle(PointSequenceViewModelPrivate *d);
        ~PointSequenceViewModelQmlHandle() override;

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

    signals:
        void currentItemChanged();
        void intermediateChanged();
        void selectionChanged();

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
