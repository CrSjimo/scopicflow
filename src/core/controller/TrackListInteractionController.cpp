#include "TrackListInteractionController.h"

#include <QQuickItem>

namespace sflow {
    TrackListInteractionController::TrackListInteractionController(QObject *parent)
        : QObject(parent),
        m_interaction(SelectByRubberBand),
        m_itemInteraction(DragMove | Select | EditMute | EditSolo | EditRecord | EditName | EditGain | EditPan | AdjustHeight) {

    }

    TrackListInteractionController::~TrackListInteractionController() = default;

    void TrackListInteractionController::setInteraction(Interaction interaction) {
        if (m_interaction != interaction) {
            m_interaction = interaction;
            emit interactionChanged();
        }
    }

    TrackListInteractionController::Interaction TrackListInteractionController::interaction() const {
        return m_interaction;
    }

    void TrackListInteractionController::setItemInteraction(ItemInteraction itemInteraction) {
        if (m_itemInteraction != itemInteraction) {
            m_itemInteraction = itemInteraction;
            emit itemInteractionChanged();
        }
    }

    TrackListInteractionController::ItemInteraction TrackListInteractionController::itemInteraction() const {
        return m_itemInteraction;
    }
}

#include "moc_TrackListInteractionController.cpp"
