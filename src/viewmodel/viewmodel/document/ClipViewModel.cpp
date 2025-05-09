#include "ClipViewModel.h"
#include <ScopicFlow/private/ViewModelHelper_p.h>

#include <limits>

namespace sflow {
    ClipViewModel::ClipViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    ClipViewModel::~ClipViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, position, position, setPosition, resetPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, length, length, setLength, resetLength)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, clipStart, clipStart, setClipStart, resetClipStart)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClipViewModel, maxLength, std::numeric_limits<int>::max(), maxLength, setMaxLength, resetMaxLength)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, trackNumber, trackNumber, setTrackNumber, resetTrackNumber)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, name, name, setName, resetName)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, selected, isSelected, setSelected, resetSelected)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, intermediate, isIntermediate, setIntermediate, resetIntermediate)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(ClipViewModel, overlapped, isOverlapped, setOverlapped, resetOverlapped)
}