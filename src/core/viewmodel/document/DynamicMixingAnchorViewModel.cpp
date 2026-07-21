#include "DynamicMixingAnchorViewModel.h"

#include <cmath>

#include <ScopicFlowCore/DynamicMixingViewModel.h>

namespace sflow {

    namespace {

        bool isValidRatio(const QList<double> &ratio) {
            double sum = 0.0;
            for (const double value : ratio) {
                if (!std::isfinite(value) || value < 0.0 || value > 1.0) {
                    return false;
                }
                sum += value;
                if (sum > 1.0) {
                    return false;
                }
            }
            return true;
        }

    }

    DynamicMixingAnchorViewModel::DynamicMixingAnchorViewModel(QObject *parent) : QObject(parent) {
    }

    DynamicMixingAnchorViewModel::~DynamicMixingAnchorViewModel() = default;

    int DynamicMixingAnchorViewModel::position() const {
        return m_position;
    }

    void DynamicMixingAnchorViewModel::setPosition(int position) {
        if (position < 0 || m_position == position) {
            return;
        }
        if (m_dynamicMixingViewModel) {
            m_dynamicMixingViewModel->moveItem(this, position);
            return;
        }
        setPositionDirect(position);
    }

    void DynamicMixingAnchorViewModel::setPositionDirect(int position) {
        if (m_position == position) {
            return;
        }
        m_position = position;
        Q_EMIT positionChanged();
    }

    QList<double> DynamicMixingAnchorViewModel::ratio() const {
        return m_ratio;
    }

    void DynamicMixingAnchorViewModel::setRatio(const QList<double> &ratio) {
        if (m_ratio == ratio || !isValidRatio(ratio)) {
            return;
        }
        m_ratio = ratio;
        Q_EMIT ratioChanged();
    }

    bool DynamicMixingAnchorViewModel::isSelected() const {
        return m_selected;
    }

    void DynamicMixingAnchorViewModel::setSelected(bool selected) {
        if (m_selected == selected) {
            return;
        }
        m_selected = selected;
        Q_EMIT selectedChanged();
    }

    DynamicMixingViewModel *DynamicMixingAnchorViewModel::dynamicMixingViewModel() const {
        return m_dynamicMixingViewModel;
    }

    void DynamicMixingAnchorViewModel::setDynamicMixingViewModel(DynamicMixingViewModel *viewModel) {
        if (m_dynamicMixingViewModel == viewModel) {
            return;
        }
        m_dynamicMixingViewModel = viewModel;
        Q_EMIT dynamicMixingViewModelChanged();
    }

}

#include "moc_DynamicMixingAnchorViewModel.cpp"
