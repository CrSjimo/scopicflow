#include "PhonemeSequenceInteractionController.h"

#include <QQuickItem>

#include <ScopicFlowCore/PhonemeViewModel.h>

namespace sflow {

    PhonemeSequenceInteractionController::PhonemeSequenceInteractionController(QObject *parent)
        : QObject(parent) {
    }

    PhonemeSequenceInteractionController::~PhonemeSequenceInteractionController() = default;

    PhonemeSequenceInteractionController::Interaction PhonemeSequenceInteractionController::primaryItemInteraction() const {
        return m_primaryItemInteraction;
    }

    void PhonemeSequenceInteractionController::setPrimaryItemInteraction(Interaction interaction) {
        if (m_primaryItemInteraction == interaction) {
            return;
        }
        m_primaryItemInteraction = interaction;
        Q_EMIT primaryItemInteractionChanged();
    }

    PhonemeSequenceInteractionController::Interaction PhonemeSequenceInteractionController::secondaryItemInteraction() const {
        return m_secondaryItemInteraction;
    }

    void PhonemeSequenceInteractionController::setSecondaryItemInteraction(Interaction interaction) {
        if (m_secondaryItemInteraction == interaction) {
            return;
        }
        m_secondaryItemInteraction = interaction;
        Q_EMIT secondaryItemInteractionChanged();
    }

}

#include "moc_PhonemeSequenceInteractionController.cpp"
