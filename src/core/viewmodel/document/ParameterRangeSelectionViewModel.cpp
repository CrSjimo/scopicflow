#include "ParameterRangeSelectionViewModel.h"

#include <algorithm>

namespace sflow {

    ParameterRangeSelectionViewModel::ParameterRangeSelectionViewModel(QObject *parent) : QObject(parent) {
    }

    ParameterRangeSelectionViewModel::~ParameterRangeSelectionViewModel() = default;

    bool ParameterRangeSelectionViewModel::hasSelection() const {
        return m_hasSelection;
    }

    int ParameterRangeSelectionViewModel::start() const {
        return m_start;
    }

    int ParameterRangeSelectionViewModel::end() const {
        return m_end;
    }

    void ParameterRangeSelectionViewModel::setRange(int start, int end) {
        start = std::max(0, start);
        end = std::max(0, end);
        if (start > end) {
            std::swap(start, end);
        }
        const bool hasSelection = start < end;
        if (m_start == start && m_end == end && m_hasSelection == hasSelection) {
            return;
        }
        const bool selectionStateChanged = m_hasSelection != hasSelection;
        m_start = start;
        m_end = end;
        m_hasSelection = hasSelection;
        Q_EMIT rangeChanged();
        if (selectionStateChanged) {
            Q_EMIT hasSelectionChanged(m_hasSelection);
        }
    }

    void ParameterRangeSelectionViewModel::clear() {
        setRange(0, 0);
    }

}

#include "moc_ParameterRangeSelectionViewModel.cpp"
