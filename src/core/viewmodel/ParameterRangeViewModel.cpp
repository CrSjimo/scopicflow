#include "ParameterRangeViewModel.h"

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {
    ParameterRangeViewModel::ParameterRangeViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    ParameterRangeViewModel::~ParameterRangeViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ParameterRangeViewModel, topValue, topValue, setTopValue, resetTopValue)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ParameterRangeViewModel, bottomValue, bottomValue, setBottomValue, resetBottomValue)
}
