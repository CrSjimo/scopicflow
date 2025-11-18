#ifndef SCOPIC_FLOW_POINTSEQUENCECONTAINER_P_H
#define SCOPIC_FLOW_POINTSEQUENCECONTAINER_P_H

#include <map>
#include <algorithm>

#include <QHash>
#include <QObject>

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

}

#endif //SCOPIC_FLOW_POINTSEQUENCECONTAINER_P_H
