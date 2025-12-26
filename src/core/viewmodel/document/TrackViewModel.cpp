#include "TrackViewModel.h"

#include <limits>

namespace sflow {

    TrackViewModel::TrackViewModel(QObject *parent)
        : QObject(parent), m_mute(false), m_solo(false), m_record(false), m_gain(0.0), m_pan(0.0),
          m_leftLevel(std::numeric_limits<double>::lowest()), m_rightLevel(std::numeric_limits<double>::lowest()),
          m_rowHeight(80.0), m_color(Qt::transparent), m_muteEnabled(true), m_soloEnabled(true),
          m_recordEnabled(true), m_selected(false) {
    }

    TrackViewModel::~TrackViewModel() = default;

    QString TrackViewModel::name() const {
        return m_name;
    }

    void TrackViewModel::setName(const QString &name) {
        if (m_name != name) {
            m_name = name;
            Q_EMIT nameChanged();
        }
    }

    bool TrackViewModel::isMute() const {
        return m_mute;
    }

    void TrackViewModel::setMute(bool mute) {
        if (m_mute != mute) {
            m_mute = mute;
            Q_EMIT muteChanged();
        }
    }

    bool TrackViewModel::isSolo() const {
        return m_solo;
    }

    void TrackViewModel::setSolo(bool solo) {
        if (m_solo != solo) {
            m_solo = solo;
            Q_EMIT soloChanged();
        }
    }

    bool TrackViewModel::isRecord() const {
        return m_record;
    }

    void TrackViewModel::setRecord(bool record) {
        if (m_record != record) {
            m_record = record;
            Q_EMIT recordChanged();
        }
    }

    double TrackViewModel::gain() const {
        return m_gain;
    }

    void TrackViewModel::setGain(double gain) {
        if (m_gain != gain) {
            m_gain = gain;
            Q_EMIT gainChanged();
        }
    }

    double TrackViewModel::pan() const {
        return m_pan;
    }

    void TrackViewModel::setPan(double pan) {
        if (m_pan != pan) {
            m_pan = pan;
            Q_EMIT panChanged();
        }
    }

    double TrackViewModel::leftLevel() const {
        return m_leftLevel;
    }

    void TrackViewModel::setLeftLevel(double leftLevel) {
        if (m_leftLevel != leftLevel) {
            m_leftLevel = leftLevel;
            Q_EMIT leftLevelChanged();
        }
    }

    double TrackViewModel::rightLevel() const {
        return m_rightLevel;
    }

    void TrackViewModel::setRightLevel(double rightLevel) {
        if (m_rightLevel != rightLevel) {
            m_rightLevel = rightLevel;
            Q_EMIT rightLevelChanged();
        }
    }

    double TrackViewModel::rowHeight() const {
        return m_rowHeight;
    }

    void TrackViewModel::setRowHeight(double rowHeight) {
        if (m_rowHeight != rowHeight) {
            m_rowHeight = rowHeight;
            Q_EMIT rowHeightChanged();
        }
    }

    QColor TrackViewModel::color() const {
        return m_color;
    }

    void TrackViewModel::setColor(const QColor &color) {
        if (m_color != color) {
            m_color = color;
            Q_EMIT colorChanged();
        }
    }

    bool TrackViewModel::isMuteEnabled() const {
        return m_muteEnabled;
    }

    void TrackViewModel::setMuteEnabled(bool enabled) {
        if (m_muteEnabled != enabled) {
            m_muteEnabled = enabled;
            Q_EMIT muteEnabledChanged();
        }
    }

    bool TrackViewModel::isSoloEnabled() const {
        return m_soloEnabled;
    }

    void TrackViewModel::setSoloEnabled(bool enabled) {
        if (m_soloEnabled != enabled) {
            m_soloEnabled = enabled;
            Q_EMIT soloEnabledChanged();
        }
    }

    bool TrackViewModel::isRecordEnabled() const {
        return m_recordEnabled;
    }

    void TrackViewModel::setRecordEnabled(bool enabled) {
        if (m_recordEnabled != enabled) {
            m_recordEnabled = enabled;
            Q_EMIT recordEnabledChanged();
        }
    }

    bool TrackViewModel::isSelected() const {
        return m_selected;
    }

    void TrackViewModel::setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            Q_EMIT selectedChanged();
        }
    }

}
