#ifndef SCOPIC_FLOW_STANDARDSELECTIONCONTROLLER_P_H
#define SCOPIC_FLOW_STANDARDSELECTIONCONTROLLER_P_H

#include <ScopicFlowCore/StandardSelectionController.h>

#include <QSet>

namespace sflow {

    class StandardSelectionControllerPrivate {
        Q_DECLARE_PUBLIC(StandardSelectionController)
    public:
        StandardSelectionController *q_ptr;

        QString selectedProperty;
        QObject *viewModel;
        QSet<QObject *> selection;
        QObject *currentItem{};

        void addToSelection(QObject *item);
        void removeFromSelection(QObject *item);
        void clearSelection();

    };

}

#endif //SCOPIC_FLOW_STANDARDSELECTIONCONTROLLER_P_H
