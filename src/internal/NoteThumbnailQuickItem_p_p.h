#ifndef SCOPIC_FLOW_NOTETHUMBNAILQUICKITEM_P_P_H
#define SCOPIC_FLOW_NOTETHUMBNAILQUICKITEM_P_P_H

#include <QPointer>
#include <QMap>

#include <ScopicFlowInternal/private/NoteThumbnailQuickItem_p.h>

#include <unordered_map>
#include <vector>

class QSGGeometryNode;

namespace sflow {

    class NoteViewModel;

    class NoteThumbnailQuickItemPrivate {
        Q_DECLARE_PUBLIC(NoteThumbnailQuickItem)
    public:
        NoteThumbnailQuickItem *q_ptr = nullptr;

        QColor color;
        QPointer<RangeSequenceViewModel> noteSequenceViewModel;

        void resetNoteSequenceViewModel(RangeSequenceViewModel *viewModel);
        void detachAllItems();
        void detachItem(NoteViewModel *itemModel);

        void onItemInserted(NoteViewModel *itemModel);
        void onItemUpdated(NoteViewModel *itemModel);
        void onItemRemoved(NoteViewModel *itemModel);
        void onItemKeyChanged(NoteViewModel *itemModel, int oldKey, int newKey);

        QHash<NoteViewModel *, QSGGeometryNode *> itemModelToNode;
        QSet<NoteViewModel *> pendingUpdateItems;
        QSet<NoteViewModel *> pendingRemoveItems;

        QColor nodeColor;

        QMap<int, int> keyCountMap;
        QHash<NoteViewModel *, int> itemToKeyMap;

        void removeKeyEntry(int key);
        void addKeyEntry(int key);
        int calculateMinKey() const;
        int calculateMaxKey() const;

    };

}

#endif //SCOPIC_FLOW_NOTETHUMBNAILQUICKITEM_P_P_H
