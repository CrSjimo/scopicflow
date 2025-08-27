#include "ClipPaneBehaviorViewModel.h"

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {
    ClipPaneBehaviorViewModel::ClipPaneBehaviorViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    ClipPaneBehaviorViewModel::~ClipPaneBehaviorViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipPaneBehaviorViewModel, lengthHint, lengthHint, setLengthHint, resetLengthHint)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClipPaneBehaviorViewModel, mouseBehavior, ScopicFlow::MB_Pointer, mouseBehavior, setMouseBehavior, resetMouseBehavior)
}