#include "TimeItemModel_p.h"

namespace sflow {
    TimeItemModel::TimeItemModel(QObject *parent) : QObject(parent) {
    }
    TimeItemModel::~TimeItemModel() = default;
    void TimeItemModel::insertItem(QObject *item, int position) {
        if (m_positions.contains(item)) {
            auto oldPosition = m_positions.value(item);
            m_items.erase(std::find_if(m_items.find(oldPosition), m_items.end(), [=](const auto &v) {
                return v.second == item;
            }));
        }
        m_items.insert({position, item});
        m_positions.insert(item, position);
    }
    void TimeItemModel::removeItem(QObject *item) {
        auto position = m_positions.value(item);
        m_items.erase(std::find_if(m_items.find(position), m_items.end(), [=](const auto &v) {
            return v.second == item;
        }));
        m_positions.remove(item);
    }
    QObject *TimeItemModel::nextItem(QObject *item) {
        auto position = m_positions.value(item);
        auto it = std::find_if(m_items.find(position), m_items.end(), [=](const auto &v) {
            return v.second == item;
        });
        if (it == m_items.end()) {
            return nullptr;
        }
        if (++it == m_items.end()) {
            return nullptr;
        }
        return it->second;
    }
    QObject *TimeItemModel::previousItem(QObject *item) {
        auto position = m_positions.value(item);
        auto it = std::find_if(m_items.find(position), m_items.end(), [=](const auto &v) {
            return v.second == item;
        });
        if (it == m_items.end()) {
            return nullptr;
        }
        if (it == m_items.begin()) {
            return nullptr;
        }
        return (--it)->second;
    }
}