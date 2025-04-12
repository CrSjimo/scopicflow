#include "TimeViewModel.h"

#include <SVSCraftCore/MusicTimeline.h>

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {

    TimeViewModel::TimeViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }

    TimeViewModel::~TimeViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TimeViewModel, start, start, setStart, resetStart)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TimeViewModel, end, 48000, end, setEnd, resetEnd)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TimeViewModel, timeline, timeline, setTimeline, resetTimeline)
}