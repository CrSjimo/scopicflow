#include "LabelSequenceViewModel.h"

namespace sflow {
    LabelSequenceViewModel::LabelSequenceViewModel(QObject *parent) : QObject(parent), m_currentItem(nullptr) {
    }
    LabelSequenceViewModel::~LabelSequenceViewModel() = default;

    LabelViewModel *LabelSequenceViewModel::currentItem() const {
        return m_currentItem;
    }
    void LabelSequenceViewModel::setCurrentItem(LabelViewModel *item) {
        m_currentItem = item;
    }
    int LabelSequenceViewModel::count() const {
        return m_labels.size();
    }
    void LabelSequenceViewModel::insertLabels(const QList<LabelViewModel *> &labels) {
    }
    void LabelSequenceViewModel::removeLabels(const QList<LabelViewModel *> &labels) {
    }
    QList<LabelViewModel *> LabelSequenceViewModel::labels() const {
        return m_labels.values();
    }
    QList<LabelViewModel *> LabelSequenceViewModel::selection() const {
        return m_selection.values();
    }
} // sflow