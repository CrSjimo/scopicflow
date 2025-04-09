#include "BusTrackViewModel.h"

#include <limits>

#include <QColor>

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    BusTrackViewModel::BusTrackViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        ViewModelHelper::initializeProperties(this);
        setLeftLevel(std::numeric_limits<double>::lowest());
        setRightLevel(std::numeric_limits<double>::lowest());
        ViewModelHelper::connectValueChanged(this);
    }
    BusTrackViewModel::~BusTrackViewModel() = default;
    QString BusTrackViewModel::name() const {
        return value("name").toString();
    }
    void BusTrackViewModel::setName(const QString &name) {
        insert("name", name);
    }
    bool BusTrackViewModel::mute() const {
        return value("mute").toBool();
    }
    void BusTrackViewModel::setMute(bool mute) {
        insert("mute", mute);
    }
    QVariant BusTrackViewModel::route() const {
        return value("route");
    }
    void BusTrackViewModel::setRoute(const QVariant &route) {
        insert("route", route);
    }
    double BusTrackViewModel::gain() const {
        return value("gain").toDouble();
    }
    void BusTrackViewModel::setGain(double gain) {
        insert("gain", gain);
    }
    double BusTrackViewModel::pan() const {
        return value("pan").toDouble();
    }
    void BusTrackViewModel::setPan(double pan) {
        insert("pan", pan);
    }
    bool BusTrackViewModel::intermediate() const {
        return value("intermediate").toBool();
    }
    void BusTrackViewModel::setIntermediate(bool intermediate) {
        insert("intermediate", intermediate);
    }
    double BusTrackViewModel::leftLevel() const {
        return value("leftLevel").toDouble();
    }
    void BusTrackViewModel::setLeftLevel(double leftLevel) {
        insert("leftLevel", leftLevel);
    }
    double BusTrackViewModel::rightLevel() const {
        return value("rightLevel").toDouble();
    }
    void BusTrackViewModel::setRightLevel(double rightLevel) {
        insert("rightLevel", rightLevel);
    }
    bool BusTrackViewModel::selected() const {
        return value("selected").toBool();
    }
    void BusTrackViewModel::setSelected(bool selected) {
        insert("selected", selected);
    }
    QColor BusTrackViewModel::color() const {
        return value("color").value<QColor>();
    }
    void BusTrackViewModel::setColor(const QColor &color) {
        insert("color", color);
    }
}