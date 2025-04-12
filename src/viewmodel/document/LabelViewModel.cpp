#include "LabelViewModel.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    LabelViewModel::LabelViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    LabelViewModel::~LabelViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(LabelViewModel, position, position, setPosition, resetPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(LabelViewModel, content, content, setContent, resetContent)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(LabelViewModel, selected, selected, setSelected, resetSelected)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(LabelViewModel, intermediate, intermediate, setIntermediate, resetIntermediate)
}