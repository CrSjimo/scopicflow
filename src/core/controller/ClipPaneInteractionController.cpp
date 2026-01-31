#include "ClipPaneInteractionController.h"

#include <QQuickItem>
#include <ScopicFlowCore/ClipViewModel.h>

namespace sflow {
    ClipPaneInteractionController::ClipPaneInteractionController(QObject *parent)
        : QObject(parent), m_clickSelectable(true),
          m_primaryItemInteraction(Move), m_secondaryItemInteraction(CopyAndMove),
          m_primarySceneInteraction(RubberBandSelect), m_secondarySceneInteraction(TimeRangeSelect),
          m_primarySelectInteraction(RubberBandSelect), m_secondarySelectInteraction(TimeRangeSelect) {
    }

    ClipPaneInteractionController::~ClipPaneInteractionController() = default;

    bool ClipPaneInteractionController::isClickSelectable() const {
        return m_clickSelectable;
    }
    void ClipPaneInteractionController::setClickSelectable(bool clickSelectable) {
        if (m_clickSelectable != clickSelectable) {
            m_clickSelectable = clickSelectable;
            emit clickSelectableChanged();
        }
    }
    ClipPaneInteractionController::Interaction ClipPaneInteractionController::primaryItemInteraction() const {
        return m_primaryItemInteraction;
    }
    void ClipPaneInteractionController::setPrimaryItemInteraction(Interaction itemInteraction) {
        if (m_primaryItemInteraction != itemInteraction) {
            m_primaryItemInteraction = itemInteraction;
            emit primaryItemInteractionChanged();
        }
    }
    ClipPaneInteractionController::Interaction ClipPaneInteractionController::secondaryItemInteraction() const {
        return m_secondaryItemInteraction;
    }
    void ClipPaneInteractionController::setSecondaryItemInteraction(Interaction itemInteraction) {
        if (m_secondaryItemInteraction != itemInteraction) {
            m_secondaryItemInteraction = itemInteraction;
            emit secondaryItemInteractionChanged();
        }
    }
    ClipPaneInteractionController::Interaction ClipPaneInteractionController::primarySceneInteraction() const {
        return m_primarySceneInteraction;
    }
    void ClipPaneInteractionController::setPrimarySceneInteraction(Interaction sceneInteraction) {
        if (m_primarySceneInteraction != sceneInteraction) {
            m_primarySceneInteraction = sceneInteraction;
            emit primarySceneInteractionChanged();
        }
    }
    ClipPaneInteractionController::Interaction ClipPaneInteractionController::secondarySceneInteraction() const {
        return m_secondarySceneInteraction;
    }
    void ClipPaneInteractionController::setSecondarySceneInteraction(Interaction sceneInteraction) {
        if (m_secondarySceneInteraction != sceneInteraction) {
            m_secondarySceneInteraction = sceneInteraction;
            emit secondarySceneInteractionChanged();
        }
    }
    ClipPaneInteractionController::Interaction ClipPaneInteractionController::primarySelectInteraction() const {
        return m_primarySelectInteraction;
    }
    void ClipPaneInteractionController::setPrimarySelectInteraction(Interaction sceneInteraction) {
        if (m_primarySelectInteraction != sceneInteraction) {
            m_primarySelectInteraction = sceneInteraction;
            emit primarySelectInteractionChanged();
        }
    }
    ClipPaneInteractionController::Interaction ClipPaneInteractionController::secondarySelectInteraction() const {
        return m_secondarySelectInteraction;
    }
    void ClipPaneInteractionController::setSecondarySelectInteraction(Interaction sceneInteraction) {
        if (m_secondarySelectInteraction != sceneInteraction) {
            m_secondarySelectInteraction = sceneInteraction;
            emit secondarySelectInteractionChanged();
        }
    }

}

#include "moc_ClipPaneInteractionController.cpp"
