#ifndef SCOPIC_FLOW_LABELSEQUENCEINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_LABELSEQUENCEINTERACTIONCONTROLLER_H

#include <qqmlintegration.h>

#include <QObject>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class LabelViewModel;

    class SCOPIC_FLOW_CORE_EXPORT LabelSequenceInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(bool clickSelectable READ isClickSelectable WRITE setClickSelectable NOTIFY clickSelectableChanged)
        Q_PROPERTY(Interaction primaryItemInteraction READ primaryItemInteraction WRITE setPrimaryItemInteraction NOTIFY primaryItemInteractionChanged)
        Q_PROPERTY(Interaction secondaryItemInteraction READ secondaryItemInteraction WRITE setSecondaryItemInteraction NOTIFY secondaryItemInteractionChanged)
        Q_PROPERTY(Interaction primarySceneInteraction READ primarySceneInteraction WRITE setPrimarySceneInteraction NOTIFY primarySceneInteractionChanged)
        Q_PROPERTY(Interaction secondarySceneInteraction READ secondarySceneInteraction WRITE setSecondarySceneInteraction NOTIFY secondarySceneInteractionChanged)
        Q_PROPERTY(Interaction primarySelectInteraction READ primarySelectInteraction WRITE setPrimarySelectInteraction NOTIFY primarySelectInteractionChanged)
        Q_PROPERTY(Interaction secondarySelectInteraction READ secondarySelectInteraction WRITE setSecondarySelectInteraction NOTIFY secondarySelectInteractionChanged)
    public:
        explicit LabelSequenceInteractionController(QObject *parent = nullptr);
        ~LabelSequenceInteractionController() override;

        bool isClickSelectable() const;
        void setClickSelectable(bool clickSelectable);

        enum Interaction {
            None,
            Move,
            CopyAndMove,
            RubberBandSelect = 0x10000,
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

        enum MoveFlag {
            MF_Move,
            MF_CopyAndMove,
        };
        Q_ENUM(MoveFlag)

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

    Q_SIGNALS:
        void clickSelectableChanged();
        void primaryItemInteractionChanged();
        void secondaryItemInteractionChanged();
        void primarySceneInteractionChanged();
        void secondarySceneInteractionChanged();
        void primarySelectInteractionChanged();
        void secondarySelectInteractionChanged();

        void rubberBandDraggingStarted(QQuickItem *labelSequence);
        void rubberBandDraggingCommitted(QQuickItem *labelSequence);
        void rubberBandDraggingAborted(QQuickItem *labelSequence);

        void movingStarted(QQuickItem *labelSequence, LabelViewModel *item, MoveFlag flag);
        void movingCommitted(QQuickItem *labelSequence, LabelViewModel *item);
        void movingAborted(QQuickItem *labelSequence, LabelViewModel *item);

        void hoverEntered(QQuickItem *labelSequence, int position);
        void hoverMoved(QQuickItem *labelSequence, int position);
        void hoverExited(QQuickItem *labelSequence);

        void itemHoverEntered(QQuickItem *labelSequence, LabelViewModel *item);
        void itemHoverExited(QQuickItem *labelSequence, LabelViewModel *item);

        void doubleClicked(QQuickItem *labelSequence, int position);
        void itemDoubleClicked(QQuickItem *labelSequence, LabelViewModel *item);

        void contextMenuRequested(QQuickItem *labelSequence, int position);
        void itemContextMenuRequested(QQuickItem *labelSequence, LabelViewModel *item);

        void inPlaceEditOperationTriggered(QQuickItem *labelSequence, LabelViewModel *item, InPlaceEditOperation operation);

    private:
        bool m_clickSelectable;
        Interaction m_primaryItemInteraction;
        Interaction m_secondaryItemInteraction;
        Interaction m_primarySceneInteraction;
        Interaction m_secondarySceneInteraction;
        Interaction m_primarySelectInteraction;
        Interaction m_secondarySelectInteraction;
    };

}

#endif //SCOPIC_FLOW_LABELSEQUENCEINTERACTIONCONTROLLER_H