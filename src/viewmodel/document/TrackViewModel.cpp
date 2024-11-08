#include "TrackViewModel.h"

namespace sflow {
    TrackViewModel::TrackViewModel(QObject *parent) : QObject(parent),
    m_mute(false), m_solo(false), m_record(false),
    m_gain(0), m_pan(0), m_intermediate(false),
    m_leftLevel(-qInf()), m_rightLevel(-qInf()),
    m_selected(false),
    m_rowHeight(80){
    }
    TrackViewModel::~TrackViewModel() = default;

    QString TrackViewModel::name() const {
        return m_name;
    }
    void TrackViewModel::setName(const QString &value) {
        if (m_name != value) {
            m_name = value;
            emit nameChanged(value);
        }
    }
    bool TrackViewModel::mute() const {
        return m_mute;
    }
    void TrackViewModel::setMute(bool mute) {
        if (m_mute != mute) {
            m_mute = mute;
            emit muteChanged(mute);
        }
    }
    bool TrackViewModel::solo() const {
        return m_solo;
    }
    void TrackViewModel::setSolo(bool solo) {
        if (m_solo != solo) {
            m_solo = solo;
            emit soloChanged(solo);
        }
    }
    bool TrackViewModel::record() const {
        return m_record;
    }
    void TrackViewModel::setRecord(bool record) {
        if (m_record != record) {
            m_record = record;
            emit recordChanged(record);
        }
    }
    double TrackViewModel::gain() const {
        return m_gain;
    }
    void TrackViewModel::setGain(double gain) {
        if (m_gain != gain) {
            m_gain = gain;
            emit gainChanged(gain);
        }
    }
    double TrackViewModel::pan() const {
        return m_pan;
    }
    void TrackViewModel::setPan(double pan) {
        if (m_pan != pan) {
            m_pan = pan;
            emit panChanged(pan);
        }
    }
    bool TrackViewModel::intermediate() const {
        return m_intermediate;
    }
    void TrackViewModel::setIntermediate(bool intermediate) {
        if (m_intermediate != intermediate) {
            m_intermediate = intermediate;
            emit intermediateChanged(intermediate);
        }
    }
    double TrackViewModel::leftLevel() const {
        return m_leftLevel;
    }
    void TrackViewModel::setLeftLevel(double leftLevel) {
        if (m_leftLevel != leftLevel) {
            m_leftLevel = leftLevel;
            emit leftLevelChanged(leftLevel);
        }
    }
    double TrackViewModel::rightLevel() const {
        return m_rightLevel;
    }
    void TrackViewModel::setRightLevel(double rightLevel) {
        if (m_rightLevel != rightLevel) {
            m_rightLevel = rightLevel;
            emit rightLevelChanged(rightLevel);
        }
    }
    bool TrackViewModel::selected() const {
        return m_selected;
    }
    void TrackViewModel::setSelected(bool selected) {
        if (m_selected != selected) {
            m_selected = selected;
            emit selectedChanged(selected);
        }
    }
    double TrackViewModel::rowHeight() const {
        return m_rowHeight;
    }
    void TrackViewModel::setRowHeight(double rowHeight) {
        if (m_rowHeight != rowHeight) {
            m_rowHeight = rowHeight;
            emit rowHeightChanged(rowHeight);
        }
    }
}