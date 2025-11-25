#include "LabelSequenceInteractionController.h"

#include <QQuickItem>
#include <ScopicFlowCore/LabelViewModel.h>

namespace sflow {
    LabelSequenceInteractionController::LabelSequenceInteractionController(QObject *parent)
        : QObject(parent),
          m_interaction(SelectByRubberBand), m_itemInteraction(Move) {

    }

    LabelSequenceInteractionController::~LabelSequenceInteractionController() = default;

    void LabelSequenceInteractionController::setInteraction(Interaction interaction) {
        if (m_interaction != interaction) {
            m_interaction = interaction;
            emit interactionChanged();
        }
    }

    LabelSequenceInteractionController::Interaction LabelSequenceInteractionController::interaction() const {
        return m_interaction;
    }

    void LabelSequenceInteractionController::setItemInteraction(ItemInteraction itemInteraction) {
        if (m_itemInteraction != itemInteraction) {
            m_itemInteraction = itemInteraction;
            emit itemInteractionChanged();
        }
    }

    LabelSequenceInteractionController::ItemInteraction LabelSequenceInteractionController::itemInteraction() const {
        return m_itemInteraction;
    }
}

#include "moc_LabelSequenceInteractionController.cpp"