#ifndef SCOPIC_FLOW_LISTVIEWMODEL_P_H
#define SCOPIC_FLOW_LISTVIEWMODEL_P_H

#include <ScopicFlowCore/ListViewModel.h>

namespace sflow {

    class ListViewModelPrivate {
        Q_DECLARE_PUBLIC(ListViewModel)
    public:
        ListViewModel *q_ptr;

        QObjectList items;

    };
}

#endif //SCOPIC_FLOW_LISTVIEWMODEL_P_H
