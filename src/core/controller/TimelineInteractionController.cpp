#include "TimelineInteractionController.h"

namespace sflow {
    TimelineInteractionController::TimelineInteractionController(QObject *parent)
        : QObject(parent),
          m_interaction(MovePositionIndicator) {

    }

    TimelineInteractionController::~TimelineInteractionController() = default;

    void TimelineInteractionController::setInteraction(Interaction interaction) {
        if (m_interaction != interaction) {
            m_interaction = interaction;
            emit interactionChanged();
        }
    }

    TimelineInteractionController::Interaction TimelineInteractionController::interaction() const {
        return m_interaction;
    }
}