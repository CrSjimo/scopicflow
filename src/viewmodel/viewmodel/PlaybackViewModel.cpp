#include "PlaybackViewModel.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    PlaybackViewModel::PlaybackViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    PlaybackViewModel::~PlaybackViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PlaybackViewModel, primaryPosition, primaryPosition, setPrimaryPosition, resetPrimaryPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY(PlaybackViewModel, secondaryPosition, secondaryPosition, setSecondaryPosition, resetSecondaryPosition)

}