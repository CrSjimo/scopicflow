#include "LabelViewModel.h"

namespace sflow {
    LabelViewModel::LabelViewModel(QObject *parent) : QObject(parent), m_position(0), m_selected(false), m_intermediate(false) {
    }
    LabelViewModel::~LabelViewModel() = default;
    int LabelViewModel::position() const {
        return m_position;
    }
    void LabelViewModel::setPosition(int position) {
        if (m_position != position) {
            m_position = position;
            emit positionChanged(position);
        }
    }
    QVariant LabelViewModel::content() const {
        return m_content;
    }
    void LabelViewModel::setContent(const QVariant &content) {
        if (m_content != content) {
            m_content = content;
            emit contentChanged(content);
        }
    }
    bool LabelViewModel::selected() const {
        return m_selected;
    }
    void LabelViewModel::setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            emit selectedChanged(selected);
        }
    }
    bool LabelViewModel::intermediate() const {
        return m_intermediate;
    }
    void LabelViewModel::setIntermediate(bool intermediate) {
        if (m_intermediate != intermediate) {
            m_intermediate = intermediate;
            emit intermediateChanged(intermediate);
        }
    }
}