#include "TrackViewModel.h"

#include <limits>

#include <QColor>

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    TrackViewModel::TrackViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    TrackViewModel::~TrackViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, name, name, setName, resetName)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, mute, mute, setMute, resetMute)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, solo, solo, setSolo, resetSolo)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, record, record, setRecord, resetRecord)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, gain, gain, setGain, resetGain)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, pan, pan, setPan, resetPan)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, intermediate, intermediate, setIntermediate, resetIntermediate)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TrackViewModel, leftLevel, std::numeric_limits<double>::lowest(), leftLevel, setLeftLevel, resetLeftLevel)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TrackViewModel, rightLevel, std::numeric_limits<double>::lowest(), rightLevel, setRightLevel, resetRightLevel)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, selected, selected, setSelected, resetSelected)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TrackViewModel, rowHeight, 80, rowHeight, setRowHeight, resetRowHeight)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackViewModel, color, color, setColor, resetColor)
}