#include "PhonemeViewModel.h"

#include <cmath>

#include <ScopicFlowCore/NoteViewModel.h>

namespace sflow {

    PhonemeViewModel::PhonemeViewModel(QObject *parent) : QObject(parent) {
    }

    PhonemeViewModel::~PhonemeViewModel() = default;

    double PhonemeViewModel::position() const {
        return m_position;
    }

    void PhonemeViewModel::setPosition(double position) {
        if (!std::isfinite(position) || m_position == position) {
            return;
        }
        m_position = position;
        Q_EMIT positionChanged();
        Q_EMIT sequencePositionChanged();
    }

    QString PhonemeViewModel::content() const {
        return m_content;
    }

    void PhonemeViewModel::setContent(const QString &content) {
        if (m_content == content) {
            return;
        }
        m_content = content;
        Q_EMIT contentChanged();
    }

    PhonemeViewModel *PhonemeViewModel::nextPhoneme() const {
        return m_nextPhoneme;
    }

    void PhonemeViewModel::setNextPhoneme(PhonemeViewModel *nextPhoneme) {
        if (m_nextPhoneme == nextPhoneme) {
            return;
        }
        QObject::disconnect(m_nextPhonemeDestroyedConnection);
        m_nextPhoneme = nextPhoneme;
        if (m_nextPhoneme) {
            m_nextPhonemeDestroyedConnection = connect(m_nextPhoneme, &QObject::destroyed, this, [this] {
                m_nextPhoneme = nullptr;
                m_nextPhonemeDestroyedConnection = {};
                Q_EMIT nextPhonemeChanged();
            });
        } else {
            m_nextPhonemeDestroyedConnection = {};
        }
        Q_EMIT nextPhonemeChanged();
    }

    NoteViewModel *PhonemeViewModel::associatedNote() const {
        return m_associatedNote;
    }

    void PhonemeViewModel::setAssociatedNote(NoteViewModel *associatedNote) {
        if (m_associatedNote == associatedNote) {
            return;
        }
        QObject::disconnect(m_associatedNotePositionConnection);
        QObject::disconnect(m_associatedNoteDestroyedConnection);
        m_associatedNote = associatedNote;
        if (m_associatedNote) {
            m_associatedNotePositionConnection = connect(m_associatedNote, &NoteViewModel::positionChanged,
                                                         this, &PhonemeViewModel::sequencePositionChanged);
            m_associatedNoteDestroyedConnection = connect(m_associatedNote, &QObject::destroyed, this, [this] {
                m_associatedNote = nullptr;
                m_associatedNotePositionConnection = {};
                m_associatedNoteDestroyedConnection = {};
                Q_EMIT associatedNoteChanged();
                Q_EMIT sequencePositionChanged();
            });
        } else {
            m_associatedNotePositionConnection = {};
            m_associatedNoteDestroyedConnection = {};
        }
        Q_EMIT associatedNoteChanged();
        Q_EMIT sequencePositionChanged();
    }

    bool PhonemeViewModel::isEdited() const {
        return m_edited;
    }

    void PhonemeViewModel::setEdited(bool edited) {
        if (m_edited == edited) {
            return;
        }
        m_edited = edited;
        Q_EMIT editedChanged();
    }

    double PhonemeViewModel::sequencePosition() const {
        return m_position + (m_associatedNote ? m_associatedNote->position() : 0);
    }

}

#include "moc_PhonemeViewModel.cpp"
