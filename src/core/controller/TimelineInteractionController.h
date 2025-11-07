#ifndef SCOPIC_FLOW_TIMELINEINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_TIMELINEINTERACTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

namespace sflow {

    class TimelineInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(Interaction interaction READ interaction NOTIFY interactionChanged)
    public:
        explicit TimelineInteractionController(QObject *parent = nullptr);
        ~TimelineInteractionController() override;

        enum InteractionFlag {
            MovePositionIndicator = 0x1,
            ZoomByRubberBand = 0x2,
        };
        Q_ENUM(InteractionFlag)
        Q_DECLARE_FLAGS(Interaction, InteractionFlag)

        void setInteraction(Interaction interaction);
        Interaction interaction() const;

    Q_SIGNALS:
        void interactionChanged();

        void interactionOperationStarted(QObject *timeline, InteractionFlag type);
        void interactionOperationFinished(QObject *timeline, InteractionFlag type);
        void hoverEntered(QObject *timeline, int position);
        void hoverMoved(QObject *timeline, int position);
        void hoverExited(QObject *timeline);
        void doubleClicked(QObject *timeline, int position);
        void contextMenuRequested(QObject *timeline);

    private:
        Interaction m_interaction;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(TimelineInteractionController::Interaction)

}

#endif //SCOPIC_FLOW_TIMELINEINTERACTIONCONTROLLER_H
