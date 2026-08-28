#include "RangeIndicatorInteractionController.h"

#include <QQuickItem>

#include <ScopicFlowCore/RangeIndicatorViewModel.h>

namespace sflow {

    RangeIndicatorInteractionController::RangeIndicatorInteractionController(QObject *parent)
        : QObject(parent) {
    }

    RangeIndicatorInteractionController::~RangeIndicatorInteractionController() = default;

}

#include "moc_RangeIndicatorInteractionController.cpp"
