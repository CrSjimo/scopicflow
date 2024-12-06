#ifndef SCOPIC_FLOW_NOTECOLLECTIONVIEWMODEL_P_H
#define SCOPIC_FLOW_NOTECOLLECTIONVIEWMODEL_P_H

#include <ScopicFlow/NoteCollectionViewModel.h>

#include <map>

#include <interval-tree/interval_tree.hpp>

#include <QSet>

namespace sflow {

    struct NoteInterval : public lib_interval_tree::interval<qint64> {
        inline NoteInterval(NoteViewModel * content, int position, int length) : lib_interval_tree::interval<qint64>(position, position + length - 1), m_content(content) {
        }

        inline NoteViewModel *content() const {
            return m_content;
        }

        inline qint64 position() const {
            return low();
        }

        inline qint64 length() const {
            return high() - low() + 1;
        }

        NoteViewModel *m_content;
    };

    class NoteCollectionViewModelPrivate {
        Q_DECLARE_PUBLIC(NoteCollectionViewModel)
    public:
        NoteCollectionViewModel *q_ptr;
        std::multimap<int, NoteViewModel *> positionNoteMap;
        QHash<NoteViewModel *, QPair<int, int>> noteIntervalMap;
        QSet<NoteViewModel *> selection;
        NoteViewModel *currentItem = nullptr;
        lib_interval_tree::interval_tree<NoteInterval> noteIntervalTree;
    };
}

#endif //SCOPIC_FLOW_NOTECOLLECTIONVIEWMODEL_P_H
