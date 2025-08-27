#include "PianoRollNoteAreaBehaviorViewModel.h"

#include <QColor>

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {
    PianoRollNoteAreaBehaviorViewModel::PianoRollNoteAreaBehaviorViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    PianoRollNoteAreaBehaviorViewModel::~PianoRollNoteAreaBehaviorViewModel() = default;
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PianoRollNoteAreaBehaviorViewModel, editing, isEditing, setEditing, resetEditing)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PianoRollNoteAreaBehaviorViewModel, unitedExtend, isUnitedExtend, setUnitedExtend, resetUnitedExtend)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PianoRollNoteAreaBehaviorViewModel, color, color, setColor, resetColor)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PianoRollNoteAreaBehaviorViewModel, compactDisplay, isCompactDisplay, setCompactDisplay, resetCompactDisplay)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PianoRollNoteAreaBehaviorViewModel, offset, offset, setOffset, resetOffset)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PianoRollNoteAreaBehaviorViewModel, lengthHint, lengthHint, setLengthHint, resetLengthHint)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(PianoRollNoteAreaBehaviorViewModel, mouseBehavior, ScopicFlow::MB_Pointer, mouseBehavior, setMouseBehavior, resetMouseBehavior)
}