#include "LabelSequenceBehaviorViewModel.h"

#include <QColor>

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {
    LabelSequenceBehaviorViewModel::LabelSequenceBehaviorViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    LabelSequenceBehaviorViewModel::~LabelSequenceBehaviorViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(LabelSequenceBehaviorViewModel, editing, isEditing, setEditing, resetEditing)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(LabelSequenceBehaviorViewModel, color, color, setColor, resetColor)
}