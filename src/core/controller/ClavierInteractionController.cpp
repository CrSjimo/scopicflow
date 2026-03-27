#include "ClavierInteractionController.h"

#include <QtGlobal>

#include <QQuickItem>

namespace sflow {
    ClavierInteractionController::ClavierInteractionController(QObject *parent)
        : QObject(parent), m_accidentalType(Flat), m_labelStrategy(LabelC), m_blackKeyLengthRatio(0.6) {
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

    void ClavierInteractionController::setBlackKeyLengthRatio(double ratio) {
        if (!qFuzzyCompare(m_blackKeyLengthRatio, ratio)) {
            m_blackKeyLengthRatio = ratio;
            emit blackKeyLengthRatioChanged();
        }
    }

    double ClavierInteractionController::blackKeyLengthRatio() const {
        return m_blackKeyLengthRatio;
    }
}

#include "moc_ClavierInteractionController.cpp"
