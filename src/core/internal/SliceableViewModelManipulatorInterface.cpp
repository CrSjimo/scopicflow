#include "SliceableViewModelManipulatorInterface_p.h"

namespace sflow {
    SliceableViewModelManipulatorInterface::SliceableViewModelManipulatorInterface(QObject *parent) : QObject(parent) {
    }
    SliceableViewModelManipulatorInterface::~SliceableViewModelManipulatorInterface() = default;

    int SliceableViewModelManipulatorInterface::itemLength(QObject *item) const {
        return 1;
    }
}
