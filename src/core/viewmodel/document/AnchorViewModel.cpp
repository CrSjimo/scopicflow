#include "AnchorViewModel.h"

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {
    AnchorViewModel::AnchorViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    AnchorViewModel::~AnchorViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, position, position, setPosition, resetPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, anchorValue, anchorValue, setAnchorValue, resetAnchorValue)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, anchorType, anchorType, setAnchorType, resetAnchorType)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, styleFlag, styleFlag, setStyleFlag, resetStyleFlag)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, anchorNode, anchorNode, setAnchorNode, resetAnchorNode)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, free, isFree, setFree, resetFree)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(AnchorViewModel, selected, isSelected, setSelected, resetSelected)
}