#include "ClavierInteractionController.h"

#include <QQuickItem>

namespace sflow {
    ClavierInteractionController::ClavierInteractionController(QObject *parent)
        : QObject(parent), m_accidentalType(Flat), m_labelStrategy(LabelC), m_displayStyle(Realistic) {
    }

    ClavierInteractionController::~ClavierInteractionController() = default;

    void ClavierInteractionController::setAccidentalType(AccidentalType type) {
        if (m_accidentalType != type) {
            m_accidentalType = type;
            emit accidentalTypeChanged();
        }
    }

    ClavierInteractionController::AccidentalType ClavierInteractionController::accidentalType() const {
        return m_accidentalType;
    }

    void ClavierInteractionController::setLabelStrategy(LabelStrategy strategy) {
        if (m_labelStrategy != strategy) {
            m_labelStrategy = strategy;
            emit labelStrategyChanged();
        }
    }

    ClavierInteractionController::LabelStrategy ClavierInteractionController::labelStrategy() const {
        return m_labelStrategy;
    }

    void ClavierInteractionController::setDisplayStyle(DisplayStyle style) {
        if (m_displayStyle != style) {
            m_displayStyle = style;
            emit displayStyleChanged();
        }
    }

    ClavierInteractionController::DisplayStyle ClavierInteractionController::displayStyle() const {
        return m_displayStyle;
    }
}

#include "moc_ClavierInteractionController.cpp"
