#include "RangeIndicatorViewModel.h"

namespace sflow {

    RangeIndicatorViewModel::RangeIndicatorViewModel(QObject *parent)
        : QObject(parent) {
    }

    RangeIndicatorViewModel::~RangeIndicatorViewModel() = default;

    int RangeIndicatorViewModel::position() const {
        return m_position;
    }

    void RangeIndicatorViewModel::setPosition(int position) {
        if (m_position == position) {
            return;
        }
        m_position = position;
        Q_EMIT positionChanged();
    }

    int RangeIndicatorViewModel::length() const {
        return m_length;
    }

    void RangeIndicatorViewModel::setLength(int length) {
        if (m_length == length) {
            return;
        }
        m_length = length;
        Q_EMIT lengthChanged();
    }

    QString RangeIndicatorViewModel::content() const {
        return m_content;
    }

    void RangeIndicatorViewModel::setContent(const QString &content) {
        if (m_content == content) {
            return;
        }
        m_content = content;
        Q_EMIT contentChanged();
    }

    SVS::SVSCraft::ControlType RangeIndicatorViewModel::type() const {
        return m_type;
    }

    void RangeIndicatorViewModel::setType(SVS::SVSCraft::ControlType type) {
        if (m_type == type) {
            return;
        }
        m_type = type;
        Q_EMIT typeChanged();
    }

}

#include "moc_RangeIndicatorViewModel.cpp"
