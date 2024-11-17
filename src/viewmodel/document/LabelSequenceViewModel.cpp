#include "LabelSequenceViewModel.h"

#include <algorithm>
#include <iterator>

#include <ScopicFlow/LabelViewModel.h>

namespace sflow {
    LabelSequenceViewModel::LabelSequenceViewModel(QObject *parent) : QObject(parent), m_currentItem(nullptr) {
    }
    LabelSequenceViewModel::~LabelSequenceViewModel() = default;

    LabelViewModel *LabelSequenceViewModel::currentItem() const {
        return m_currentItem;
    }
    void LabelSequenceViewModel::setCurrentItem(LabelViewModel *item) {
        if (m_currentItem != item) {
            m_currentItem = item;
            emit currentItemChanged(item);
        }
    }
    int LabelSequenceViewModel::count() const {
        return m_labels.size();
    }
    void LabelSequenceViewModel::insertLabels(const QList<LabelViewModel *> &labels) {
        bool hasSelectedFlag = false;
        for (auto label : labels) {
            m_labels.insert({label->position(), label});
            m_positions.insert(label, label->position());
            if (label->selected()) {
                hasSelectedFlag = true;
                m_selection.insert(label);
            }
            connect(label, &LabelViewModel::positionChanged, this, [=] {
                auto oldPosition = m_positions.value(label);
                m_labels.erase(std::find_if(m_labels.find(oldPosition), m_labels.end(), [=](auto it) {
                    return it.second == label;
                }));
                m_labels.insert({label->position(), label});
                m_positions.insert(label, label->position());
            });
            connect(label, &LabelViewModel::selectedChanged, this, [=] {
                if (label->selected()) {
                    m_selection.insert(label);
                } else {
                    m_selection.remove(label);
                }
                emit selectionChanged();
            });
        }
        if (!labels.isEmpty())
            emit countChanged(m_labels.size());
        if (hasSelectedFlag)
            emit selectionChanged();
    }
    void LabelSequenceViewModel::removeLabels(const QList<LabelViewModel *> &labels) {
        bool hasSelectedFlag = false;
        bool hasCurrentFlag = false;
        for (auto label : labels) {
            m_labels.erase(std::find_if(m_labels.find(label->position()), m_labels.end(), [=](auto it) {
                return it.second == label;
            }));
            m_positions.remove(label);
            if (label->selected()) {
                hasSelectedFlag = true;
                m_selection.remove(label);
            }
            if (label == m_currentItem) {
                hasCurrentFlag = true;
                m_currentItem = nullptr;
            }
            delete label;
        }
        if (!labels.isEmpty())
            emit countChanged(m_labels.size());
        if (hasSelectedFlag)
            emit selectionChanged();
        if (hasCurrentFlag)
            emit currentItemChanged(m_currentItem);
    }
    QList<LabelViewModel *> LabelSequenceViewModel::labels() const {
        QList<LabelViewModel *> ret;
        std::transform(m_labels.begin(), m_labels.end(), std::back_inserter(ret), [=](auto it) {
            return it.second;
        });
        return ret;
    }
    QList<LabelViewModel *> LabelSequenceViewModel::selection() const {
        return m_selection.values();
    }
    LabelViewModel *LabelSequenceViewModel::nextItem(LabelViewModel *item) const {
        auto it = std::find_if(m_labels.find(item->position()), m_labels.end(), [=](auto it) {
            return it.second == item;
        });
        if (it == m_labels.end()) {
            return nullptr;
        }
        if (++it == m_labels.end()) {
            return nullptr;
        }
        return it->second;
    }
    LabelViewModel *LabelSequenceViewModel::previousItem(LabelViewModel *item) const {
        auto it = std::find_if(m_labels.find(item->position()), m_labels.end(), [=](auto it) {
            return it.second == item;
        });
        if (it == m_labels.end()) {
            return nullptr;
        }
        if (it == m_labels.begin()) {
            return nullptr;
        }
        return (--it)->second;
    }
} // sflow