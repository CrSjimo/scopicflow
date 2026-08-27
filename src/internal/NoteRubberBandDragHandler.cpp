#include "NoteRubberBandDragHandler_p.h"

#include <ScopicFlowInternal/private/NoteEditLayerQuickItem_p.h>
#include <ScopicFlowInternal/private/RubberBandLayerQuickItem_p.h>

namespace sflow {

    NoteRubberBandDragHandler::NoteRubberBandDragHandler(QObject *parent) : RubberBandDragHandler(parent) {
    }

    NoteEditLayerQuickItem *NoteRubberBandDragHandler::noteItem() const {
        return m_noteItem;
    }

    void NoteRubberBandDragHandler::setNoteItem(NoteEditLayerQuickItem *noteItem) {
        if (m_noteItem == noteItem)
            return;
        disconnect(m_noteItemDestroyedConnection);
        m_noteItem = noteItem;
        if (m_noteItem) {
            m_noteItemDestroyedConnection = connect(m_noteItem, &QObject::destroyed, this, [this] {
                m_noteItem = nullptr;
                m_noteItemDestroyedConnection = {};
                Q_EMIT noteItemChanged();
            });
        } else {
            m_noteItemDestroyedConnection = {};
        }
        Q_EMIT noteItemChanged();
    }

    QRectF NoteRubberBandDragHandler::mapToSelector(const QRectF &rect) const {
        if (!m_noteItem || !rubberBandLayer())
            return {};
        return m_noteItem->mapToTickKeyRect(rubberBandLayer()->mapRectToItem(m_noteItem, rect));
    }

}

#include "moc_NoteRubberBandDragHandler_p.cpp"
