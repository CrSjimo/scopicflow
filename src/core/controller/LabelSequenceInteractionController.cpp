#include "LabelSequenceInteractionController.h"

#include <QQuickItem>
#include <ScopicFlowCore/LabelViewModel.h>

namespace sflow {
    LabelSequenceInteractionController::LabelSequenceInteractionController(QObject *parent)
        : QObject(parent), m_clickSelectable(true),
          m_primaryItemInteraction(Move), m_secondaryItemInteraction(CopyAndMove),
          m_primarySceneInteraction(RubberBandSelect), m_secondarySceneInteraction(RubberBandSelect),
          m_primarySelectInteraction(RubberBandSelect), m_secondarySelectInteraction(RubberBandSelect) {
    }

    LabelSequenceInteractionController::~LabelSequenceInteractionController() = default;

    bool LabelSequenceInteractionController::isClickSelectable() const {
        return m_clickSelectable;
    }
    void LabelSequenceInteractionController::setClickSelectable(bool clickSelectable) {
        if (m_clickSelectable != clickSelectable) {
            m_clickSelectable = clickSelectable;
            emit clickSelectableChanged();
        }
    }
    LabelSequenceInteractionController::Interaction LabelSequenceInteractionController::primaryItemInteraction() const {
        return m_primaryItemInteraction;
    }
    void LabelSequenceInteractionController::setPrimaryItemInteraction(Interaction itemInteraction) {
        if (m_primaryItemInteraction != itemInteraction) {
            m_primaryItemInteraction = itemInteraction;
            emit primaryItemInteractionChanged();
        }
    }
    LabelSequenceInteractionController::Interaction LabelSequenceInteractionController::secondaryItemInteraction() const {
        return m_secondaryItemInteraction;
    }
    void LabelSequenceInteractionController::setSecondaryItemInteraction(Interaction itemInteraction) {
        if (m_secondaryItemInteraction != itemInteraction) {
            m_secondaryItemInteraction = itemInteraction;
            emit secondaryItemInteractionChanged();
        }
    }
    LabelSequenceInteractionController::Interaction LabelSequenceInteractionController::primarySceneInteraction() const {
        return m_primarySceneInteraction;
    }
    void LabelSequenceInteractionController::setPrimarySceneInteraction(Interaction sceneInteraction) {
        if (m_primarySceneInteraction != sceneInteraction) {
            m_primarySceneInteraction = sceneInteraction;
            emit primarySceneInteractionChanged();
        }
    }
    LabelSequenceInteractionController::Interaction LabelSequenceInteractionController::secondarySceneInteraction() const {
        return m_secondarySceneInteraction;
    }
    void LabelSequenceInteractionController::setSecondarySceneInteraction(Interaction sceneInteraction) {
        if (m_secondarySceneInteraction != sceneInteraction) {
            m_secondarySceneInteraction = sceneInteraction;
            emit secondarySceneInteractionChanged();
        }
    }
    LabelSequenceInteractionController::Interaction LabelSequenceInteractionController::primarySelectInteraction() const {
        return m_primarySelectInteraction;
    }
    void LabelSequenceInteractionController::setPrimarySelectInteraction(Interaction sceneInteraction) {
        if (m_primarySelectInteraction != sceneInteraction) {
            m_primarySelectInteraction = sceneInteraction;
            emit primarySelectInteractionChanged();
        }
    }
    LabelSequenceInteractionController::Interaction LabelSequenceInteractionController::secondarySelectInteraction() const {
        return m_secondarySelectInteraction;
    }
    void LabelSequenceInteractionController::setSecondarySelectInteraction(Interaction sceneInteraction) {
        if (m_secondarySelectInteraction != sceneInteraction) {
            m_secondarySelectInteraction = sceneInteraction;
            emit secondarySelectInteractionChanged();
        }
    }

}

#include "moc_LabelSequenceInteractionController.cpp"