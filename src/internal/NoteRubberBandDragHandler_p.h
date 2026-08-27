#ifndef SCOPIC_FLOW_NOTERUBBERBANDDRAGHANDLER_P_H
#define SCOPIC_FLOW_NOTERUBBERBANDDRAGHANDLER_P_H

#include <QMetaObject>
#include <QPointer>
#include <qqmlintegration.h>

#include <ScopicFlowInternal/private/RubberBandDragHandler_p.h>

namespace sflow {

    class NoteEditLayerQuickItem;

    class NoteRubberBandDragHandler : public RubberBandDragHandler {
        Q_OBJECT
        QML_NAMED_ELEMENT(NoteRubberBandDragHandler)
        Q_PROPERTY(NoteEditLayerQuickItem *noteItem READ noteItem WRITE setNoteItem NOTIFY noteItemChanged REQUIRED)

    public:
        explicit NoteRubberBandDragHandler(QObject *parent = nullptr);

        NoteEditLayerQuickItem *noteItem() const;
        void setNoteItem(NoteEditLayerQuickItem *noteItem);

        QRectF mapToSelector(const QRectF &rect) const override;

    Q_SIGNALS:
        void noteItemChanged();

    private:
        QPointer<NoteEditLayerQuickItem> m_noteItem;
        QMetaObject::Connection m_noteItemDestroyedConnection;
    };

}

#endif // SCOPIC_FLOW_NOTERUBBERBANDDRAGHANDLER_P_H
