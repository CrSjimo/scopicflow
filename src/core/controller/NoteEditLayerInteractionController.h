#ifndef SCOPIC_FLOW_NOTEEDITLAYERINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_NOTEEDITLAYERINTERACTIONCONTROLLER_H

#include <qqmlintegration.h>

#include <QObject>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class NoteViewModel;
    class RangeSequenceViewModel;

    class SCOPIC_FLOW_CORE_EXPORT NoteEditLayerInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(bool clickSelectable READ isClickSelectable WRITE setClickSelectable NOTIFY clickSelectableChanged)
        Q_PROPERTY(Interaction primaryItemInteraction READ primaryItemInteraction WRITE setPrimaryItemInteraction NOTIFY primaryItemInteractionChanged)
        Q_PROPERTY(Interaction secondaryItemInteraction READ secondaryItemInteraction WRITE setSecondaryItemInteraction NOTIFY secondaryItemInteractionChanged)
        Q_PROPERTY(Interaction primarySceneInteraction READ primarySceneInteraction WRITE setPrimarySceneInteraction NOTIFY primarySceneInteractionChanged)
        Q_PROPERTY(Interaction secondarySceneInteraction READ secondarySceneInteraction WRITE setSecondarySceneInteraction NOTIFY secondarySceneInteractionChanged)
        Q_PROPERTY(Interaction primarySelectInteraction READ primarySelectInteraction WRITE setPrimarySelectInteraction NOTIFY primarySelectInteractionChanged)
        Q_PROPERTY(Interaction secondarySelectInteraction READ secondarySelectInteraction WRITE setSecondarySelectInteraction NOTIFY secondarySelectInteractionChanged)
        Q_PROPERTY(AdditionalTextPosition additionalTextPosition READ additionalTextPosition WRITE setAdditionalTextPosition NOTIFY additionalTextPositionChanged)
        Q_PROPERTY(int shortNoteThreshold READ shortNoteThreshold WRITE setShortNoteThreshold NOTIFY shortNoteThresholdChanged)
        Q_PROPERTY(bool warnOfOverlappingNotes READ warnOfOverlappingNotes WRITE setWarnOfOverlappingNotes NOTIFY warnOfOverlappingNotesChanged)
    public:
        explicit NoteEditLayerInteractionController(QObject *parent = nullptr);
        ~NoteEditLayerInteractionController() override;

        bool isClickSelectable() const;
        void setClickSelectable(bool clickSelectable);

        enum Interaction {
            None,
            Move,
            CopyAndMove,
            Split,
            RubberBandSelect = 0x10000,
            TimeRangeSelect,
            Draw,
        };
        Q_ENUM(Interaction)
        Interaction primaryItemInteraction() const;
        void setPrimaryItemInteraction(Interaction itemInteraction);
        Interaction secondaryItemInteraction() const;
        void setSecondaryItemInteraction(Interaction itemInteraction);
        Interaction primarySceneInteraction() const;
        void setPrimarySceneInteraction(Interaction sceneInteraction);
        Interaction secondarySceneInteraction() const;
        void setSecondarySceneInteraction(Interaction sceneInteraction);
        Interaction primarySelectInteraction() const;
        void setPrimarySelectInteraction(Interaction sceneInteraction);
        Interaction secondarySelectInteraction() const;
        void setSecondarySelectInteraction(Interaction sceneInteraction);

        enum AdditionalTextPosition {
            AdditionalTextPosition_Up,
            AdditionalTextPosition_Down,
        };
        Q_ENUM(AdditionalTextPosition)
        AdditionalTextPosition additionalTextPosition() const;
        void setAdditionalTextPosition(AdditionalTextPosition position);

        int shortNoteThreshold() const;
        void setShortNoteThreshold(int threshold);

        bool warnOfOverlappingNotes() const;
        void setWarnOfOverlappingNotes(bool warnOfOverlappingNotes);

        enum MoveFlag {
            MF_Move,
            MF_CopyAndMove,
        };
        Q_ENUM(MoveFlag)

        Q_INVOKABLE virtual NoteViewModel *createAndInsertNoteOnDrawing(RangeSequenceViewModel *noteSequenceViewModel, int position, int trackIndex);

        enum AdjustLengthEdge {
            LeftEdge,
            RightEdge,
        };
        Q_ENUM(AdjustLengthEdge)

        enum InPlaceEditOperation {
            StartEditing,
            CommitEditing,
            AbortEditing,
            MovePrevious,
            MoveNext,
            MoveHome,
            MoveEnd,
        };
        Q_ENUM(InPlaceEditOperation)

        enum RippleDeleteOption {
            RippleDelete_Previous,
            RippleDelete_Next,
        };
        Q_ENUM(RippleDeleteOption)

    Q_SIGNALS:
        void clickSelectableChanged();
        void primaryItemInteractionChanged();
        void secondaryItemInteractionChanged();
        void primarySceneInteractionChanged();
        void secondarySceneInteractionChanged();
        void primarySelectInteractionChanged();
        void secondarySelectInteractionChanged();
        void additionalTextPositionChanged();
        void shortNoteThresholdChanged();
        void warnOfOverlappingNotesChanged();

        void rubberBandDraggingStarted(QQuickItem *noteArea);
        void rubberBandDraggingCommitted(QQuickItem *noteArea);
        void rubberBandDraggingAborted(QQuickItem *noteArea);

        void movingStarted(QQuickItem *noteArea, NoteViewModel *item, MoveFlag flag);
        void movingCommitted(QQuickItem *noteArea, NoteViewModel *item);
        void movingAborted(QQuickItem *noteArea, NoteViewModel *item);

        void adjustLengthStarted(QQuickItem *noteArea, NoteViewModel *item, AdjustLengthEdge edge);
        void adjustLengthCommitted(QQuickItem *noteArea, NoteViewModel *item, AdjustLengthEdge edge);
        void adjustLengthAborted(QQuickItem *noteArea, NoteViewModel *item, AdjustLengthEdge edge);

        void drawingStarted(QQuickItem *noteArea);
        void drawingCommitted(QQuickItem *noteArea);
        void drawingAborted(QQuickItem *noteArea);

        void hoverEntered(QQuickItem *noteArea, int position, int trackIndex);
        void hoverMoved(QQuickItem *noteArea, int position, int trackIndex);
        void hoverExited(QQuickItem *noteArea);

        void itemHoverEntered(QQuickItem *noteArea, NoteViewModel *item);
        void itemHoverExited(QQuickItem *noteArea, NoteViewModel *item);

        void doubleClicked(QQuickItem *noteArea, int position, int key);
        void itemDoubleClicked(QQuickItem *noteArea, NoteViewModel *item);
        void itemAdditionalTextDoubleClicked(QQuickItem *noteArea, NoteViewModel *item);

        void contextMenuRequested(QQuickItem *noteArea, int position, int trackIndex);
        void itemContextMenuRequested(QQuickItem *noteArea, NoteViewModel *item);

        void splitStarted(QQuickItem *noteArea, int position);
        void splitCommitted(QQuickItem *noteArea, int position);
        void splitAborted(QQuickItem *noteArea);

        void lyricInPlaceEditOperationTriggered(QQuickItem *noteArea, NoteViewModel *item, InPlaceEditOperation operation);
        void additionalTextInPlaceEditOperationTriggered(QQuickItem *noteArea, NoteViewModel *item, InPlaceEditOperation operation);

        void rippleDeleteRequested(QQuickItem *noteArea, NoteViewModel *item, RippleDeleteOption option);

    private:
        bool m_clickSelectable;
        Interaction m_primaryItemInteraction;
        Interaction m_secondaryItemInteraction;
        Interaction m_primarySceneInteraction;
        Interaction m_secondarySceneInteraction;
        Interaction m_primarySelectInteraction;
        Interaction m_secondarySelectInteraction;
        AdditionalTextPosition m_additionalTextPosition;
        int m_shortNoteThreshold;
        bool m_warnOfOverlappingNotes;
    };

}

#endif //SCOPIC_FLOW_NOTEEDITLAYERINTERACTIONCONTROLLER_H
