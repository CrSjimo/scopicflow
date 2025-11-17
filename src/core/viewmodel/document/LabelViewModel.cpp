#include "LabelViewModel.h"

namespace sflow {

    LabelViewModel::LabelViewModel(QObject *parent) : QObject(parent),
        m_position(0), m_content(), m_selected(false) {
    }

    LabelViewModel::~LabelViewModel() = default;

    int LabelViewModel::position() const {
        return m_position;
    }

    void LabelViewModel::setPosition(int position) {
        if (m_position != position) {
            m_position = position;
            Q_EMIT positionChanged();
        }
    }

    QString LabelViewModel::content() const {
        return m_content;
    }

    void LabelViewModel::setContent(const QString &content) {
        if (m_content != content) {
            m_content = content;
            Q_EMIT contentChanged();
        }
    }

    bool LabelViewModel::isSelected() const {
        return m_selected;
    }

    void LabelViewModel::setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            Q_EMIT selectedChanged();
        }
    }

}