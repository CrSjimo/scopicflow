#include "ParameterEditorInteractionController.h"

#include <cmath>

#include <QQuickItem>

#include <ScopicFlowCore/AnchorParameterViewModel.h>

namespace sflow {

    ParameterEditorInteractionController::ParameterEditorInteractionController(QObject *parent) : QObject(parent) {
    }

    ParameterEditorInteractionController::~ParameterEditorInteractionController() = default;

    bool ParameterEditorInteractionController::isClickSelectable() const {
        return m_clickSelectable;
    }

    void ParameterEditorInteractionController::setClickSelectable(bool clickSelectable) {
        if (m_clickSelectable == clickSelectable) {
            return;
        }
        m_clickSelectable = clickSelectable;
        Q_EMIT clickSelectableChanged();
    }

    ParameterEditorInteractionController::Interaction ParameterEditorInteractionController::primaryItemInteraction() const {
        return m_primaryItemInteraction;
    }

    void ParameterEditorInteractionController::setPrimaryItemInteraction(Interaction interaction) {
        if (m_primaryItemInteraction == interaction) {
            return;
        }
        m_primaryItemInteraction = interaction;
        Q_EMIT primaryItemInteractionChanged();
    }

    ParameterEditorInteractionController::Interaction ParameterEditorInteractionController::secondaryItemInteraction() const {
        return m_secondaryItemInteraction;
    }

    void ParameterEditorInteractionController::setSecondaryItemInteraction(Interaction interaction) {
        if (m_secondaryItemInteraction == interaction) {
            return;
        }
        m_secondaryItemInteraction = interaction;
        Q_EMIT secondaryItemInteractionChanged();
    }

    ParameterEditorInteractionController::Interaction ParameterEditorInteractionController::primarySceneInteraction() const {
        return m_primarySceneInteraction;
    }

    void ParameterEditorInteractionController::setPrimarySceneInteraction(Interaction interaction) {
        if (m_primarySceneInteraction == interaction) {
            return;
        }
        m_primarySceneInteraction = interaction;
        Q_EMIT primarySceneInteractionChanged();
    }

    ParameterEditorInteractionController::Interaction ParameterEditorInteractionController::secondarySceneInteraction() const {
        return m_secondarySceneInteraction;
    }

    void ParameterEditorInteractionController::setSecondarySceneInteraction(Interaction interaction) {
        if (m_secondarySceneInteraction == interaction) {
            return;
        }
        m_secondarySceneInteraction = interaction;
        Q_EMIT secondarySceneInteractionChanged();
    }

    ParameterEditorInteractionController::Interaction ParameterEditorInteractionController::primarySelectInteraction() const {
        return m_primarySelectInteraction;
    }

    void ParameterEditorInteractionController::setPrimarySelectInteraction(Interaction interaction) {
        if (m_primarySelectInteraction == interaction) {
            return;
        }
        m_primarySelectInteraction = interaction;
        Q_EMIT primarySelectInteractionChanged();
    }

    ParameterEditorInteractionController::Interaction ParameterEditorInteractionController::secondarySelectInteraction() const {
        return m_secondarySelectInteraction;
    }

    void ParameterEditorInteractionController::setSecondarySelectInteraction(Interaction interaction) {
        if (m_secondarySelectInteraction == interaction) {
            return;
        }
        m_secondarySelectInteraction = interaction;
        Q_EMIT secondarySelectInteractionChanged();
    }

    ParameterAnchorViewModel::InterpolationMode ParameterEditorInteractionController::newAnchorInterpolationMode() const {
        return m_newAnchorInterpolationMode;
    }

    void ParameterEditorInteractionController::setNewAnchorInterpolationMode(ParameterAnchorViewModel::InterpolationMode interpolationMode) {
        if (interpolationMode < ParameterAnchorViewModel::Hermite
            || interpolationMode > ParameterAnchorViewModel::None
            || m_newAnchorInterpolationMode == interpolationMode) {
            return;
        }
        m_newAnchorInterpolationMode = interpolationMode;
        Q_EMIT newAnchorInterpolationModeChanged();
    }

    ParameterEditorInteractionController::FillMode ParameterEditorInteractionController::fillMode() const {
        return m_fillMode;
    }

    void ParameterEditorInteractionController::setFillMode(FillMode fillMode) {
        if (fillMode < NoFill || fillMode > BaselineFill || m_fillMode == fillMode) {
            return;
        }
        m_fillMode = fillMode;
        Q_EMIT fillModeChanged();
    }

    double ParameterEditorInteractionController::fillBaseline() const {
        return m_fillBaseline;
    }

    void ParameterEditorInteractionController::setFillBaseline(double fillBaseline) {
        if (!std::isfinite(fillBaseline) || m_fillBaseline == fillBaseline) {
            return;
        }
        m_fillBaseline = fillBaseline;
        Q_EMIT fillBaselineChanged();
    }

    bool ParameterEditorInteractionController::isReferenceVisible() const {
        return m_referenceVisible;
    }

    void ParameterEditorInteractionController::setReferenceVisible(bool referenceVisible) {
        if (m_referenceVisible == referenceVisible) {
            return;
        }
        m_referenceVisible = referenceVisible;
        Q_EMIT referenceVisibleChanged();
    }

    double ParameterEditorInteractionController::referenceBaseline() const {
        return m_referenceBaseline;
    }

    void ParameterEditorInteractionController::setReferenceBaseline(double referenceBaseline) {
        if (!std::isfinite(referenceBaseline) || m_referenceBaseline == referenceBaseline) {
            return;
        }
        m_referenceBaseline = referenceBaseline;
        Q_EMIT referenceBaselineChanged();
    }

    bool ParameterEditorInteractionController::isDefaultValueEnabled() const {
        return m_defaultValueEnabled;
    }

    void ParameterEditorInteractionController::setDefaultValueEnabled(bool defaultValueEnabled) {
        if (m_defaultValueEnabled == defaultValueEnabled) {
            return;
        }
        m_defaultValueEnabled = defaultValueEnabled;
        Q_EMIT defaultValueEnabledChanged();
    }

    double ParameterEditorInteractionController::defaultValue() const {
        return m_defaultValue;
    }

    void ParameterEditorInteractionController::setDefaultValue(double defaultValue) {
        if (!std::isfinite(defaultValue) || m_defaultValue == defaultValue) {
            return;
        }
        m_defaultValue = defaultValue;
        Q_EMIT defaultValueChanged();
    }

    ParameterEditorInteractionController::CurveDisplayMode ParameterEditorInteractionController::originalAndDefaultCurveDisplayMode() const {
        return m_originalAndDefaultCurveDisplayMode;
    }

    void ParameterEditorInteractionController::setOriginalAndDefaultCurveDisplayMode(CurveDisplayMode mode) {
        if (mode < CurveHidden || mode > CurveSolid || m_originalAndDefaultCurveDisplayMode == mode) {
            return;
        }
        m_originalAndDefaultCurveDisplayMode = mode;
        Q_EMIT originalAndDefaultCurveDisplayModeChanged();
    }

    ParameterAnchorViewModel *ParameterEditorInteractionController::createAnchor(int position, double value) {
        if (position < 0 || !std::isfinite(value)) {
            return nullptr;
        }
        auto *item = new ParameterAnchorViewModel;
        item->setPosition(position);
        item->setValue(value);
        item->setInterpolationMode(m_newAnchorInterpolationMode);
        return item;
    }

    bool ParameterEditorInteractionController::insertAnchor(AnchorParameterViewModel *viewModel,
                                                              ParameterAnchorViewModel *item) {
        return viewModel && item && viewModel->insertItem(item);
    }

    ParameterAnchorViewModel *ParameterEditorInteractionController::createAndInsertAnchor(AnchorParameterViewModel *viewModel,
                                                                                           int position,
                                                                                           double value) {
        auto *item = createAnchor(position, value);
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

    bool ParameterEditorInteractionController::removeAnchor(AnchorParameterViewModel *viewModel,
                                                             ParameterAnchorViewModel *item) {
        return viewModel && item && viewModel->removeItem(item);
    }

}

#include "moc_ParameterEditorInteractionController.cpp"
