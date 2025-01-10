#include "TrackListLayoutViewModel.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    TrackListLayoutViewModel::TrackListLayoutViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        ViewModelHelper::initializeProperties(this);
    }
    TrackListLayoutViewModel::~TrackListLayoutViewModel() = default;
    double TrackListLayoutViewModel::viewportOffset() const {
        return value("viewportOffset").toDouble();
    }
    void TrackListLayoutViewModel::setViewportOffset(double value) {
        insert("viewportOffset", value);
    }
}