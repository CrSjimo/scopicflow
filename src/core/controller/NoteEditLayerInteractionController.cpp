#include "NoteEditLayerInteractionController.h"

#include <QQuickItem>
#include <ScopicFlowCore/NoteViewModel.h>
#include <ScopicFlowCore/RangeSequenceViewModel.h>

namespace sflow {
    NoteEditLayerInteractionController::NoteEditLayerInteractionController(QObject *parent)
        : QObject(parent), m_clickSelectable(true),
          m_primaryItemInteraction(Move), m_secondaryItemInteraction(CopyAndMove),
          m_primarySceneInteraction(RubberBandSelect), m_secondarySceneInteraction(TimeRangeSelect),
          m_primarySelectInteraction(RubberBandSelect), m_secondarySelectInteraction(TimeRangeSelect),
          m_additionalTextPosition(AdditionalTextPosition_Up) {
    }

    NoteEditLayerInteractionController::~NoteEditLayerInteractionController() = default;

    bool NoteEditLayerInteractionController::isClickSelectable() const {
        return m_clickSelectable;
    }
    NoteEditLayerInteractionController::AdditionalTextPosition NoteEditLayerInteractionController::additionalTextPosition() const {
        return m_additionalTextPosition;
    }
    void NoteEditLayerInteractionController::setAdditionalTextPosition(AdditionalTextPosition position) {
        if (m_additionalTextPosition != position) {
            m_additionalTextPosition = position;
            emit additionalTextPositionChanged();
        }
    }
    void NoteEditLayerInteractionController::setClickSelectable(bool clickSelectable) {
        if (m_clickSelectable != clickSelectable) {
            m_clickSelectable = clickSelectable;
            emit clickSelectableChanged();
        }
    }
    NoteEditLayerInteractionController::Interaction NoteEditLayerInteractionController::primaryItemInteraction() const {
        return m_primaryItemInteraction;
    }
    void NoteEditLayerInteractionController::setPrimaryItemInteraction(Interaction itemInteraction) {
        if (m_primaryItemInteraction != itemInteraction) {
            m_primaryItemInteraction = itemInteraction;
            emit primaryItemInteractionChanged();
        }
    }
    NoteEditLayerInteractionController::Interaction NoteEditLayerInteractionController::secondaryItemInteraction() const {
        return m_secondaryItemInteraction;
    }
    void NoteEditLayerInteractionController::setSecondaryItemInteraction(Interaction itemInteraction) {
        if (m_secondaryItemInteraction != itemInteraction) {
            m_secondaryItemInteraction = itemInteraction;
            emit secondaryItemInteractionChanged();
        }
    }
    NoteEditLayerInteractionController::Interaction NoteEditLayerInteractionController::primarySceneInteraction() const {
        return m_primarySceneInteraction;
    }
    void NoteEditLayerInteractionController::setPrimarySceneInteraction(Interaction sceneInteraction) {
        if (m_primarySceneInteraction != sceneInteraction) {
            m_primarySceneInteraction = sceneInteraction;
            emit primarySceneInteractionChanged();
        }
    }
    NoteEditLayerInteractionController::Interaction NoteEditLayerInteractionController::secondarySceneInteraction() const {
        return m_secondarySceneInteraction;
    }
    void NoteEditLayerInteractionController::setSecondarySceneInteraction(Interaction sceneInteraction) {
        if (m_secondarySceneInteraction != sceneInteraction) {
            m_secondarySceneInteraction = sceneInteraction;
            emit secondarySceneInteractionChanged();
        }
    }
    NoteEditLayerInteractionController::Interaction NoteEditLayerInteractionController::primarySelectInteraction() const {
        return m_primarySelectInteraction;
    }
    void NoteEditLayerInteractionController::setPrimarySelectInteraction(Interaction sceneInteraction) {
        if (m_primarySelectInteraction != sceneInteraction) {
            m_primarySelectInteraction = sceneInteraction;
            emit primarySelectInteractionChanged();
        }
    }
    NoteEditLayerInteractionController::Interaction NoteEditLayerInteractionController::secondarySelectInteraction() const {
        return m_secondarySelectInteraction;
    }
    void NoteEditLayerInteractionController::setSecondarySelectInteraction(Interaction sceneInteraction) {
        if (m_secondarySelectInteraction != sceneInteraction) {
            m_secondarySelectInteraction = sceneInteraction;
            emit secondarySelectInteractionChanged();
        }
    }
    NoteViewModel *NoteEditLayerInteractionController::createAndInsertNoteOnDrawing(RangeSequenceViewModel *noteSequenceViewModel, int position, int trackIndex) {
        auto noteViewModel = new NoteViewModel(noteSequenceViewModel);
        noteViewModel->setPosition(position);
        noteViewModel->setKey(trackIndex);
        noteSequenceViewModel->insertItem(noteViewModel);
        return noteViewModel;
    }

}

#include "moc_NoteEditLayerInteractionController.cpp"
