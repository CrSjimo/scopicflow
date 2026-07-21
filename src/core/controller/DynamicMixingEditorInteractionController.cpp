#include "DynamicMixingEditorInteractionController.h"

#include <cmath>

#include <QQuickItem>

#include <ScopicFlowCore/DynamicMixingAnchorViewModel.h>
#include <ScopicFlowCore/DynamicMixingViewModel.h>

namespace sflow {

    namespace {

        bool isValidRatio(const QList<double> &ratio) {
            double sum = 0.0;
            for (const double value : ratio) {
                if (!std::isfinite(value) || value < 0.0 || value > 1.0) {
                    return false;
                }
                sum += value;
                if (sum > 1.0) {
                    return false;
                }
            }
            return true;
        }

    }

    DynamicMixingEditorInteractionController::DynamicMixingEditorInteractionController(QObject *parent)
        : QObject(parent) {
    }

    DynamicMixingEditorInteractionController::~DynamicMixingEditorInteractionController() = default;

    bool DynamicMixingEditorInteractionController::isClickSelectable() const {
        return m_clickSelectable;
    }

    void DynamicMixingEditorInteractionController::setClickSelectable(bool clickSelectable) {
        if (m_clickSelectable == clickSelectable) {
            return;
        }
        m_clickSelectable = clickSelectable;
        Q_EMIT clickSelectableChanged();
    }

    DynamicMixingEditorInteractionController::Interaction DynamicMixingEditorInteractionController::primaryItemInteraction() const {
        return m_primaryItemInteraction;
    }

    void DynamicMixingEditorInteractionController::setPrimaryItemInteraction(Interaction interaction) {
        if (m_primaryItemInteraction == interaction) {
            return;
        }
        m_primaryItemInteraction = interaction;
        Q_EMIT primaryItemInteractionChanged();
    }

    DynamicMixingEditorInteractionController::Interaction DynamicMixingEditorInteractionController::secondaryItemInteraction() const {
        return m_secondaryItemInteraction;
    }

    void DynamicMixingEditorInteractionController::setSecondaryItemInteraction(Interaction interaction) {
        if (m_secondaryItemInteraction == interaction) {
            return;
        }
        m_secondaryItemInteraction = interaction;
        Q_EMIT secondaryItemInteractionChanged();
    }

    DynamicMixingEditorInteractionController::Interaction DynamicMixingEditorInteractionController::primarySceneInteraction() const {
        return m_primarySceneInteraction;
    }

    void DynamicMixingEditorInteractionController::setPrimarySceneInteraction(Interaction interaction) {
        if (m_primarySceneInteraction == interaction) {
            return;
        }
        m_primarySceneInteraction = interaction;
        Q_EMIT primarySceneInteractionChanged();
    }

    DynamicMixingEditorInteractionController::Interaction DynamicMixingEditorInteractionController::secondarySceneInteraction() const {
        return m_secondarySceneInteraction;
    }

    void DynamicMixingEditorInteractionController::setSecondarySceneInteraction(Interaction interaction) {
        if (m_secondarySceneInteraction == interaction) {
            return;
        }
        m_secondarySceneInteraction = interaction;
        Q_EMIT secondarySceneInteractionChanged();
    }

    DynamicMixingEditorInteractionController::Interaction DynamicMixingEditorInteractionController::primarySelectInteraction() const {
        return m_primarySelectInteraction;
    }

    void DynamicMixingEditorInteractionController::setPrimarySelectInteraction(Interaction interaction) {
        if (m_primarySelectInteraction == interaction) {
            return;
        }
        m_primarySelectInteraction = interaction;
        Q_EMIT primarySelectInteractionChanged();
    }

    DynamicMixingEditorInteractionController::Interaction DynamicMixingEditorInteractionController::secondarySelectInteraction() const {
        return m_secondarySelectInteraction;
    }

    void DynamicMixingEditorInteractionController::setSecondarySelectInteraction(Interaction interaction) {
        if (m_secondarySelectInteraction == interaction) {
            return;
        }
        m_secondarySelectInteraction = interaction;
        Q_EMIT secondarySelectInteractionChanged();
    }

    DynamicMixingAnchorViewModel *DynamicMixingEditorInteractionController::createAnchor(
        int position,
        const QList<double> &ratio) {
        if (position < 0 || !isValidRatio(ratio)) {
            return nullptr;
        }
        auto *item = new DynamicMixingAnchorViewModel;
        item->setPosition(position);
        item->setRatio(ratio);
        return item;
    }

    bool DynamicMixingEditorInteractionController::insertAnchor(DynamicMixingViewModel *viewModel,
                                                                 DynamicMixingAnchorViewModel *item) {
        return viewModel && item && viewModel->insertItem(item);
    }

    DynamicMixingAnchorViewModel *DynamicMixingEditorInteractionController::createAndInsertAnchor(
        DynamicMixingViewModel *viewModel,
        int position,
        const QList<double> &ratio) {
        auto *item = createAnchor(position, ratio);
        if (!item) {
            return nullptr;
        }
        if (!insertAnchor(viewModel, item)) {
            if (!item->parent()) {
                delete item;
            }
            return nullptr;
        }
        if (!item->parent()) {
            item->setParent(viewModel);
        }
        return item;
    }

    bool DynamicMixingEditorInteractionController::removeAnchor(DynamicMixingViewModel *viewModel,
                                                                 DynamicMixingAnchorViewModel *item) {
        return viewModel && item && viewModel->removeItem(item);
    }

}

#include "moc_DynamicMixingEditorInteractionController.cpp"
