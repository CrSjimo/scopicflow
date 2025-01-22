#ifndef SCOPIC_FLOW_SEQUENCESLICERLOADER_P_P_H
#define SCOPIC_FLOW_SEQUENCESLICERLOADER_P_P_H

#include <ScopicFlowInternal/private/SequenceSlicerLoader_p.h>

#include <QHash>

#include <ScopicFlow/private/SliceableViewModelQmlHandle_p.h>

namespace sflow {

    class SequenceSlicerLoaderPrivate {
        Q_DECLARE_PUBLIC(SequenceSlicerLoader)
    public:
        SequenceSlicerLoader *q_ptr;
        QQmlComponent *delegate = nullptr;
        SliceableViewModelQmlHandle *handle = nullptr;
        QPair<int, int> range;

        QHash<QObject *, QQuickItem *> items;

        void handleRangeChanged();
        void handleDelegateChanged();
        void handleHandleChanged();
        void handleItemInserted(QObject *item);
        void handleItemRemoved(QObject *item);
        void handleItemUpdated(QObject *item);
        QQuickItem *createView(QObject *itemModel);
    };
}

#endif //SCOPIC_FLOW_SEQUENCESLICERLOADER_P_P_H
