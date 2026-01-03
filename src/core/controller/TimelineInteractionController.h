#ifndef SCOPIC_FLOW_TIMELINEINTERACTIONCONTROLLER_H
#define SCOPIC_FLOW_TIMELINEINTERACTIONCONTROLLER_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class SCOPIC_FLOW_CORE_EXPORT TimelineInteractionController : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_PROPERTY(Interaction interaction READ interaction NOTIFY interactionChanged)
    public:
        explicit TimelineInteractionController(QObject *parent = nullptr);
        ~TimelineInteractionController() override;

        enum InteractionFlag {
            MovePositionIndicator = 0x1,
            ZoomByRubberBand = 0x2,
            AdjustLoopRange = 0x4,
        };
        Q_ENUM(InteractionFlag)
        Q_DECLARE_FLAGS(Interaction, InteractionFlag)

        void setInteraction(Interaction interaction);
        Interaction interaction() const;

        enum LoopAdjustmentOperation {
            AdjustRange,
            AdjustStart,
            AdjustEnd,
        };
        Q_ENUM(LoopAdjustmentOperation)

    Q_SIGNALS:
        void interactionChanged();

        void rubberBandDraggingStarted(QQuickItem *timeline);
        void rubberBandDraggingFinished(QQuickItem *timeline);

        void positionIndicatorMovingStarted(QQuickItem *timeline);
        void positionIndicatorMovingFinished(QQuickItem *timeline);

        void loopRangeAdjustingStarted(QQuickItem *timeline, LoopAdjustmentOperation operation);
        void loopRangeAdjustingFinished(QQuickItem *timeline, LoopAdjustmentOperation operation);

        void hoverEntered(QQuickItem *timeline, int position);
        void hoverMoved(QQuickItem *timeline, int position);
        void hoverExited(QQuickItem *timeline);

        void loopHoverEntered(QQuickItem *timeline, LoopAdjustmentOperation operation);
        void loopHoverExited(QQuickItem *timeline, LoopAdjustmentOperation operation);

        void doubleClicked(QQuickItem *timeline, int position);
        void contextMenuRequested(QQuickItem *timeline, int position);

    private:
        Interaction m_interaction;
    };

    Q_DECLARE_OPERATORS_FOR_FLAGS(TimelineInteractionController::Interaction)

}

#endif //SCOPIC_FLOW_TIMELINEINTERACTIONCONTROLLER_H
