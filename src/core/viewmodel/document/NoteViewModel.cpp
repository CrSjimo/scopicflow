#include "NoteViewModel.h"
#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {
    NoteViewModel::NoteViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    NoteViewModel::~NoteViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(NoteViewModel, position, position, setPosition, resetPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(NoteViewModel, length, length, setLength, resetLength)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(NoteViewModel, key, key, setKey, resetKey)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(NoteViewModel, lyric, lyric, setLyric, resetLyric)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(NoteViewModel, selected, isSelected, setSelected, resetSelected)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(NoteViewModel, intermediate, isIntermediate, setIntermediate, resetIntermediate)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(NoteViewModel, overlapped, isOverlapped, setOverlapped, resetOverlapped)
}
