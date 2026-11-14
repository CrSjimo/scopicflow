#include "NoteViewModel.h"

namespace sflow {

    NoteViewModel::NoteViewModel(QObject *parent) : QObject(parent),
        m_position(0), m_length(0), m_key(0), m_additionalTextHighlighted(false), m_selected(false), m_overlapped(false) {
    }

    NoteViewModel::~NoteViewModel() = default;

    QString NoteViewModel::lyric() const {
        return m_lyric;
    }

    void NoteViewModel::setLyric(const QString &lyric) {
        if (m_lyric != lyric) {
            m_lyric = lyric;
            Q_EMIT lyricChanged();
        }
    }

    int NoteViewModel::position() const {
        return m_position;
    }

    void NoteViewModel::setPosition(int position) {
        if (m_position != position) {
            m_position = position;
            Q_EMIT positionChanged();
        }
    }

    int NoteViewModel::length() const {
        return m_length;
    }

    void NoteViewModel::setLength(int length) {
        if (m_length != length) {
            m_length = length;
            Q_EMIT lengthChanged();
        }
    }

    int NoteViewModel::key() const {
        return m_key;
    }

    void NoteViewModel::setKey(int key) {
        if (m_key != key) {
            m_key = key;
            Q_EMIT keyChanged();
        }
    }

    QString NoteViewModel::additionalText() const {
        return m_additionalText;
    }

    void NoteViewModel::setAdditionalText(const QString &additionalText) {
        if (m_additionalText != additionalText) {
            m_additionalText = additionalText;
            Q_EMIT additionalTextChanged();
        }
    }

    bool NoteViewModel::isAdditionalTextHighlighted() const {
        return m_additionalTextHighlighted;
    }

    void NoteViewModel::setAdditionalTextHighlighted(bool additionalTextHighlighted) {
        if (m_additionalTextHighlighted != additionalTextHighlighted) {
            m_additionalTextHighlighted = additionalTextHighlighted;
            Q_EMIT additionalTextHighlightedChanged();
        }
    }

    bool NoteViewModel::isSelected() const {
        return m_selected;
    }

    void NoteViewModel::setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            Q_EMIT selectedChanged();
        }
    }

    bool NoteViewModel::isOverlapped() const {
        return m_overlapped;
    }

    void NoteViewModel::setOverlapped(bool overlapped) {
        if (m_overlapped != overlapped) {
            m_overlapped = overlapped;
            Q_EMIT overlappedChanged();
        }
    }

}
