#include "ClipViewModel.h"

#include <limits>

#include <ScopicFlowCore/RangeSequenceViewModel.h>

namespace sflow {

    ClipViewModel::ClipViewModel(QObject *parent) : QObject(parent),
        m_associatedNoteSequence(nullptr),
        m_position(0), m_length(0), m_clipStart(0), m_maxLength(std::numeric_limits<int>::max()),
        m_trackIndex(0), m_mute(false), m_selected(false), m_overlapped(false), m_editing(false) {
    }

    ClipViewModel::~ClipViewModel() = default;

    QString ClipViewModel::name() const {
        return m_name;
    }

    void ClipViewModel::setName(const QString &name) {
        if (m_name != name) {
            m_name = name;
            Q_EMIT nameChanged();
        }
    }

    QUrl ClipViewModel::iconSource() const {
        return m_iconSource;
    }

    void ClipViewModel::setIconSource(const QUrl &iconSource) {
        if (m_iconSource != iconSource) {
            m_iconSource = iconSource;
            Q_EMIT iconSourceChanged();
        }
    }

    RangeSequenceViewModel *ClipViewModel::associatedNoteSequence() const {
        return m_associatedNoteSequence;
    }

    void ClipViewModel::setAssociatedNoteSequence(RangeSequenceViewModel *associatedNoteSequence) {
        if (m_associatedNoteSequence != associatedNoteSequence) {
            m_associatedNoteSequence = associatedNoteSequence;
            Q_EMIT associatedNoteSequenceChanged();
        }
    }

    int ClipViewModel::position() const {
        return m_position;
    }

    void ClipViewModel::setPosition(int position) {
        if (m_position != position) {
            m_position = position;
            Q_EMIT positionChanged();
        }
    }

    int ClipViewModel::length() const {
        return m_length;
    }

    void ClipViewModel::setLength(int length) {
        if (m_length != length) {
            m_length = length;
            Q_EMIT lengthChanged();
        }
    }

    int ClipViewModel::clipStart() const {
        return m_clipStart;
    }

    void ClipViewModel::setClipStart(int clipStart) {
        if (m_clipStart != clipStart) {
            m_clipStart = clipStart;
            Q_EMIT clipStartChanged();
        }
    }

    int ClipViewModel::maxLength() const {
        return m_maxLength;
    }

    void ClipViewModel::setMaxLength(int maxLength) {
        if (m_maxLength != maxLength) {
            m_maxLength = maxLength;
            Q_EMIT maxLengthChanged();
        }
    }

    int ClipViewModel::trackIndex() const {
        return m_trackIndex;
    }

    void ClipViewModel::setTrackIndex(int trackIndex) {
        if (m_trackIndex != trackIndex) {
            m_trackIndex = trackIndex;
            Q_EMIT trackIndexChanged();
        }
    }

    bool ClipViewModel::isMute() const {
        return m_mute;
    }

    void ClipViewModel::setMute(bool mute) {
        if (m_mute != mute) {
            m_mute = mute;
            Q_EMIT muteChanged();
        }
    }

    bool ClipViewModel::isSelected() const {
        return m_selected;
    }

    void ClipViewModel::setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            Q_EMIT selectedChanged();
        }
    }

    bool ClipViewModel::isOverlapped() const {
        return m_overlapped;
    }

    void ClipViewModel::setOverlapped(bool overlapped) {
        if (m_overlapped != overlapped) {
            m_overlapped = overlapped;
            Q_EMIT overlappedChanged();
        }
    }

    bool ClipViewModel::isEditing() const {
        return m_editing;
    }

    void ClipViewModel::setEditing(bool editing) {
        if (m_editing != editing) {
            m_editing = editing;
            Q_EMIT editingChanged();
        }
    }

}

#include "moc_ClipViewModel.cpp"