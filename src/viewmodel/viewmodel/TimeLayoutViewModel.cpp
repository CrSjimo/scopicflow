#include "TimeLayoutViewModel.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    TimeLayoutViewModel::TimeLayoutViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    TimeLayoutViewModel::~TimeLayoutViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TimeLayoutViewModel, pixelDensity, 0.2, pixelDensity, setPixelDensity, resetPixelDensity)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TimeLayoutViewModel, maximumPixelDensity, 1.0, maximumPixelDensity, setMaximumPixelDensity, resetMaximumPixelDensity)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TimeLayoutViewModel, minimumPixelDensity, 0.00390625, minimumPixelDensity, setMinimumPixelDensity, resetMinimumPixelDensity)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TimeLayoutViewModel, cursorPosition, -1, cursorPosition, setCursorPosition, resetCursorPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(TimeLayoutViewModel, positionAlignment, 480, positionAlignment, setPositionAlignment, resetPositionAlignment)
}