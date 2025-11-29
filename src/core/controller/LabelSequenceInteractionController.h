#ifndef SCOPIC_FLOW_LABELSEQUENCEINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_LABELSEQUENCEINTERACTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class LabelViewModel;

    class SCOPIC_FLOW_CORE_EXPORT LabelSequenceInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(Interaction interaction READ interaction NOTIFY interactionChanged)
        Q_PROPERTY(ItemInteraction itemInteraction READ itemInteraction NOTIFY itemInteractionChanged)
    public:
        explicit LabelSequenceInteractionController(QObject *parent = nullptr);
        ~LabelSequenceInteractionController() override;

        enum InteractionFlag {
            Draw = 0x1,
            SelectByRubberBand = 0x2,
        };
        Q_ENUM(InteractionFlag)
        Q_DECLARE_FLAGS(Interaction, InteractionFlag)

        void setInteraction(Interaction interaction);
        Interaction interaction() const;

        enum ItemInteractionFlag {
            Move = 0x1,
            Select = 0x2,
        };
        Q_ENUM(ItemInteractionFlag)
        Q_DECLARE_FLAGS(ItemInteraction, ItemInteractionFlag)

        void setItemInteraction(ItemInteraction itemInteraction);
        ItemInteraction itemInteraction() const;

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
        void interactionChanged();
        void itemInteractionChanged();

        void interactionOperationStarted(QQuickItem *labelSequence, InteractionFlag type);
        void interactionOperationFinished(QQuickItem *labelSequence, InteractionFlag type);
        void itemInteractionOperationStarted(QQuickItem *labelSequence, LabelViewModel *item, ItemInteractionFlag type);
        void itemInteractionOperationFinished(QQuickItem *labelSequence, LabelViewModel *item, ItemInteractionFlag type);
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
        Interaction m_interaction;
        ItemInteraction m_itemInteraction;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(LabelSequenceInteractionController::Interaction)

}

#endif //SCOPIC_FLOW_LABELSEQUENCEINTERACTIONCONTROLLER_H