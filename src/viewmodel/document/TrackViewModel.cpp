#include "TrackViewModel.h"

#include <limits>

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    TrackViewModel::TrackViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        ViewModelHelper::initializeProperties(this);
        setLeftLevel(std::numeric_limits<double>::lowest());
        setRightLevel(std::numeric_limits<double>::lowest());
        setRowHeight(80);
        ViewModelHelper::connectValueChanged(this);
    }
    TrackViewModel::~TrackViewModel() = default;

    QString TrackViewModel::name() const {
        return value("name").toString();
    }
    void TrackViewModel::setName(const QString &value) {
        insert("name", value);
    }
    bool TrackViewModel::mute() const {
        return value("mute").toBool();
    }
    void TrackViewModel::setMute(bool mute) {
        insert("mute", mute);
    }
    bool TrackViewModel::solo() const {
        return value("solo").toBool();
    }
    void TrackViewModel::setSolo(bool solo) {
        insert("solo", solo);
    }
    bool TrackViewModel::record() const {
        return value("record").toBool();
    }
    void TrackViewModel::setRecord(bool record) {
        insert("record", record);
    }
    double TrackViewModel::gain() const {
        return value("gain").toDouble();
    }
    void TrackViewModel::setGain(double gain) {
        insert("gain", gain);
    }
    double TrackViewModel::pan() const {
        return value("pan").toDouble();
    }
    void TrackViewModel::setPan(double pan) {
        insert("pan", pan);
    }
    bool TrackViewModel::intermediate() const {
        return value("intermediate").toBool();
    }
    void TrackViewModel::setIntermediate(bool intermediate) {
        insert("intermediate", intermediate);
    }
    double TrackViewModel::leftLevel() const {
        return value("leftLevel").toDouble();
    }
    void TrackViewModel::setLeftLevel(double leftLevel) {
        insert("leftLevel", leftLevel);
    }
    double TrackViewModel::rightLevel() const {
        return value("rightValue").toDouble();
    }
    void TrackViewModel::setRightLevel(double rightLevel) {
        insert("rightLevel", rightLevel);
    }
    bool TrackViewModel::selected() const {
        return value("selected").toBool();
    }
    void TrackViewModel::setSelected(bool selected) {
        insert("selected", selected);
    }
    double TrackViewModel::rowHeight() const {
        return value("rowHeight").toDouble();
    }
    void TrackViewModel::setRowHeight(double rowHeight) {
        insert("rowHeight", rowHeight);
    }
}