#ifndef SCOPIC_FLOW_PHONEMESEQUENCEINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_PHONEMESEQUENCEINTERACTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class PhonemeViewModel;

    class SCOPIC_FLOW_CORE_EXPORT PhonemeSequenceInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(Interaction primaryItemInteraction READ primaryItemInteraction WRITE setPrimaryItemInteraction NOTIFY primaryItemInteractionChanged)
        Q_PROPERTY(Interaction secondaryItemInteraction READ secondaryItemInteraction WRITE setSecondaryItemInteraction NOTIFY secondaryItemInteractionChanged)

    public:
        enum Interaction {
            None,
            Move,
        };
        Q_ENUM(Interaction)

        explicit PhonemeSequenceInteractionController(QObject *parent = nullptr);
        ~PhonemeSequenceInteractionController() override;

        Interaction primaryItemInteraction() const;
        void setPrimaryItemInteraction(Interaction interaction);

        Interaction secondaryItemInteraction() const;
        void setSecondaryItemInteraction(Interaction interaction);

        enum InPlaceEditOperation {
            StartEditing,
            CommitEditing,
            AbortEditing,
        };
        Q_ENUM(InPlaceEditOperation)

    Q_SIGNALS:
        void primaryItemInteractionChanged();
        void secondaryItemInteractionChanged();

        void movingStarted(QQuickItem *phonemeSequence, PhonemeViewModel *item);
        void movingCommitted(QQuickItem *phonemeSequence, PhonemeViewModel *item);
        void movingAborted(QQuickItem *phonemeSequence, PhonemeViewModel *item);

        void hoverEntered(QQuickItem *phonemeSequence, int position);
        void hoverMoved(QQuickItem *phonemeSequence, int position);
        void hoverExited(QQuickItem *phonemeSequence);

        void itemHoverEntered(QQuickItem *phonemeSequence, PhonemeViewModel *item);
        void itemHoverExited(QQuickItem *phonemeSequence, PhonemeViewModel *item);

        void doubleClicked(QQuickItem *phonemeSequence, int position);
        void itemDoubleClicked(QQuickItem *phonemeSequence, PhonemeViewModel *item);

        void contextMenuRequested(QQuickItem *phonemeSequence, int position);
        void itemContextMenuRequested(QQuickItem *phonemeSequence, PhonemeViewModel *item);

        void inPlaceEditOperationTriggered(QQuickItem *phonemeSequence, PhonemeViewModel *item,
                                           InPlaceEditOperation operation);

    private:
        Interaction m_primaryItemInteraction = Move;
        Interaction m_secondaryItemInteraction = None;
    };

}

#endif // SCOPIC_FLOW_PHONEMESEQUENCEINTERACTIONCONTROLLER_H
