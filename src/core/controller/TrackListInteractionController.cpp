#include "TrackListInteractionController.h"

#include <QQuickItem>

namespace sflow {
    TrackListInteractionController::TrackListInteractionController(QObject *parent)
        : QObject(parent), m_clickSelectable(true),
          m_primaryItemInteraction(DragMove), m_secondaryItemInteraction(DragCopy),
          m_primarySceneInteraction(RubberBandSelect), m_secondarySceneInteraction(RubberBandSelect),
          m_primarySelectInteraction(RubberBandSelect), m_secondarySelectInteraction(RubberBandSelect),
          m_itemAction(EditMute | EditSolo | EditRecord | EditName | EditGain | EditPan | AdjustHeight) {
    }

    TrackListInteractionController::~TrackListInteractionController() = default;

    bool TrackListInteractionController::isClickSelectable() const {
        return m_clickSelectable;
    }
    void TrackListInteractionController::setClickSelectable(bool clickSelectable) {
        if (m_clickSelectable != clickSelectable) {
            m_clickSelectable = clickSelectable;
            emit clickSelectableChanged();
        }
    }
    TrackListInteractionController::Interaction TrackListInteractionController::primaryItemInteraction() const {
        return m_primaryItemInteraction;
    }
    void TrackListInteractionController::setPrimaryItemInteraction(Interaction itemInteraction) {
        if (m_primaryItemInteraction != itemInteraction) {
            m_primaryItemInteraction = itemInteraction;
            emit primaryItemInteractionChanged();
        }
    }
    TrackListInteractionController::Interaction TrackListInteractionController::secondaryItemInteraction() const {
        return m_secondaryItemInteraction;
    }
    void TrackListInteractionController::setSecondaryItemInteraction(Interaction itemInteraction) {
        if (m_secondaryItemInteraction != itemInteraction) {
            m_secondaryItemInteraction = itemInteraction;
            emit secondaryItemInteractionChanged();
        }
    }
    TrackListInteractionController::Interaction TrackListInteractionController::primarySceneInteraction() const {
        return m_primarySceneInteraction;
    }
    void TrackListInteractionController::setPrimarySceneInteraction(Interaction sceneInteraction) {
        if (m_primarySceneInteraction != sceneInteraction) {
            m_primarySceneInteraction = sceneInteraction;
            emit primarySceneInteractionChanged();
        }
    }
    TrackListInteractionController::Interaction TrackListInteractionController::secondarySceneInteraction() const {
        return m_secondarySceneInteraction;
    }
    void TrackListInteractionController::setSecondarySceneInteraction(Interaction sceneInteraction) {
        if (m_secondarySceneInteraction != sceneInteraction) {
            m_secondarySceneInteraction = sceneInteraction;
            emit secondarySceneInteractionChanged();
        }
    }
    TrackListInteractionController::Interaction TrackListInteractionController::primarySelectInteraction() const {
        return m_primarySelectInteraction;
    }
    void TrackListInteractionController::setPrimarySelectInteraction(Interaction sceneInteraction) {
        if (m_primarySelectInteraction != sceneInteraction) {
            m_primarySelectInteraction = sceneInteraction;
            emit primarySelectInteractionChanged();
        }
    }
    TrackListInteractionController::Interaction TrackListInteractionController::secondarySelectInteraction() const {
        return m_secondarySelectInteraction;
    }
    void TrackListInteractionController::setSecondarySelectInteraction(Interaction sceneInteraction) {
        if (m_secondarySelectInteraction != sceneInteraction) {
            m_secondarySelectInteraction = sceneInteraction;
            emit secondarySelectInteractionChanged();
        }
    }
    TrackListInteractionController::ItemAction TrackListInteractionController::itemAction() const {
        return m_itemAction;
    }
    void TrackListInteractionController::setItemAction(ItemAction itemAction) {
        if (m_itemAction != itemAction) {
            m_itemAction = itemAction;
            emit itemActionChanged();
        }
    }

}

#include "moc_TrackListInteractionController.cpp"
