#include "AnchorViewModel.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    AnchorViewModel::AnchorViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    AnchorViewModel::~AnchorViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, position, position, setPosition, resetPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, anchorValue, anchorValue, setAnchorValue, resetAnchorValue)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, anchorType, anchorType, setAnchorType, resetAnchorType)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, styleFlag, styleFlag, setStyleFlag, resetStyleFlag)
}