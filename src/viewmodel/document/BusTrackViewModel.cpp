#include "BusTrackViewModel.h"

#include <limits>

#include <QColor>

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    BusTrackViewModel::BusTrackViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    BusTrackViewModel::~BusTrackViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, name, name, setName, resetName)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, mute, isMute, setMute, resetMute)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, route, isRoute, setRoute, resetRoute)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, gain, gain, setGain, resetGain)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, pan, pan, setPan, resetPan)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, intermediate, isIntermediate, setIntermediate, resetIntermediate)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(BusTrackViewModel, leftLevel, std::numeric_limits<double>::lowest(), leftLevel, setLeftLevel, resetLeftLevel)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(BusTrackViewModel, rightLevel, std::numeric_limits<double>::lowest(), rightLevel, setRightLevel, resetRightLevel)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, selected, isSelected, setSelected, resetSelected)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(BusTrackViewModel, color, color, setColor, resetColor)
}