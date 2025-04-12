#include "TrackListLayoutViewModel.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    TrackListLayoutViewModel::TrackListLayoutViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    TrackListLayoutViewModel::~TrackListLayoutViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(TrackListLayoutViewModel, viewportOffset, viewportOffset, setViewportOffset, resetViewportOffset)

}