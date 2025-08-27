#include "ClavierViewModel.h"

#include <ScopicFlowCore/private/ViewModelHelper_p.h>

namespace sflow {
    ClavierViewModel::ClavierViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        SFLOW_INITIALIZE_VIEWMODEL();
    }
    ClavierViewModel::~ClavierViewModel() = default;

    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClavierViewModel, start, 42, start, setStart, resetStart)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClavierViewModel, pixelDensity, 24, pixelDensity, setPixelDensity, resetPixelDensity)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClavierViewModel, maximumPixelDensity, 64, maximumPixelDensity, setMaximumPixelDensity, resetMaximumPixelDensity)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClavierViewModel, minimumPixelDensity, 16, minimumPixelDensity, setMinimumPixelDensity, resetMinimumPixelDensity)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClavierViewModel, cursorPosition, -1, cursorPosition, setCursorPosition, resetCursorPosition)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClavierViewModel, accidentalType, SVS::MusicPitch::Flat, accidentalType, setAccidentalType, resetAccidentalType)
    SFLOW_VIEWMODEL_IMPLEMENT_PROPERTY_DEFAULT_VALUE(ClavierViewModel, labelStrategy, ScopicFlow::LS_C, labelStrategy, setLabelStrategy, resetLabelStrategy)

}