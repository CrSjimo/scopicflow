#include "SliceableViewModelQmlHandle_p.h"

namespace sflow {
    SliceableViewModelQmlHandle::SliceableViewModelQmlHandle(QObject *parent) : QObject(parent) {
    }
    SliceableViewModelQmlHandle::~SliceableViewModelQmlHandle() = default;

    int SliceableViewModelQmlHandle::itemLength(QObject *item) const {
        return 1;
    }
}
