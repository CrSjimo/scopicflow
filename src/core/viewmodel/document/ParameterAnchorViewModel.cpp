#include "ParameterAnchorViewModel.h"

#include <cmath>

#include <ScopicFlowCore/AnchorParameterViewModel.h>

namespace sflow {

    ParameterAnchorViewModel::ParameterAnchorViewModel(QObject *parent) : QObject(parent) {
    }

    ParameterAnchorViewModel::~ParameterAnchorViewModel() = default;

    int ParameterAnchorViewModel::position() const {
        return m_position;
    }

    void ParameterAnchorViewModel::setPosition(int position) {
        if (position < 0 || m_position == position) {
            return;
        }
        if (m_anchorParameterViewModel) {
            m_anchorParameterViewModel->moveItem(this, position);
            return;
        }
        setPositionDirect(position);
    }

    void ParameterAnchorViewModel::setPositionDirect(int position) {
        if (m_position == position) {
            return;
        }
        m_position = position;
        Q_EMIT positionChanged();
    }

    double ParameterAnchorViewModel::value() const {
        return m_value;
    }

    void ParameterAnchorViewModel::setValue(double value) {
        if (!std::isfinite(value) || m_value == value) {
            return;
        }
        m_value = value;
        Q_EMIT valueChanged();
    }

    ParameterAnchorViewModel::InterpolationMode ParameterAnchorViewModel::interpolationMode() const {
        return m_interpolationMode;
    }

    void ParameterAnchorViewModel::setInterpolationMode(InterpolationMode interpolationMode) {
        if (interpolationMode < Hermite || interpolationMode > None || m_interpolationMode == interpolationMode) {
            return;
        }
        m_interpolationMode = interpolationMode;
        Q_EMIT interpolationModeChanged();
    }

    bool ParameterAnchorViewModel::isSelected() const {
        return m_selected;
    }

    void ParameterAnchorViewModel::setSelected(bool selected) {
        if (m_selected == selected) {
            return;
        }
        m_selected = selected;
        Q_EMIT selectedChanged();
    }

    AnchorParameterViewModel *ParameterAnchorViewModel::anchorParameterViewModel() const {
        return m_anchorParameterViewModel;
    }

    void ParameterAnchorViewModel::setAnchorParameterViewModel(AnchorParameterViewModel *viewModel) {
        if (m_anchorParameterViewModel == viewModel) {
            return;
        }
        m_anchorParameterViewModel = viewModel;
        Q_EMIT anchorParameterViewModelChanged();
    }

}

#include "moc_ParameterAnchorViewModel.cpp"
