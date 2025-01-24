#ifndef SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H
#define SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H

#include <qqmlregistration.h>

#include <ScopicFlow/LabelViewModel.h>

namespace sflow {
    struct LabelViewModelForeign {
        Q_GADGET
        QML_NAMED_ELEMENT(LabelViewModel)
        QML_FOREIGN(LabelViewModel)
    };
}

#endif //SCOPIC_FLOW_CREATABLEVIEWMODELS_P_H