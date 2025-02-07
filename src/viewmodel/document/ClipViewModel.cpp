#include "ClipViewModel.h"
#include "ViewModelHelper_p.h"

#include <limits>

namespace sflow {
    ClipViewModel::ClipViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        ViewModelHelper::initializeProperties(this);
        ViewModelHelper::connectValueChanged(this);
        setMaxLength(std::numeric_limits<int>::max());
    }
    ClipViewModel::~ClipViewModel() = default;

    int ClipViewModel::position() const {
        return value("position").toInt();
    }
    void ClipViewModel::setPosition(int position) {
        insert("position", position);
    }
    int ClipViewModel::length() const {
        return value("length").toInt();
    }
    void ClipViewModel::setLength(int length) {
        insert("length", length);
    }
    int ClipViewModel::clipStart() const {
        return value("clipStart").toInt();
    }
    void ClipViewModel::setClipStart(int clipStart) {
        insert("clipStart", clipStart);
    }

    int ClipViewModel::maxLength() const {
        return value("maxLength").toInt();
    }
    void ClipViewModel::setMaxLength(int maxLength) {
        insert("maxLength", maxLength);
    }
    int ClipViewModel::trackNumber() const {
        return value("trackNumber").toInt();
    }
    void ClipViewModel::setTrackNumber(int trackNumber) {
        insert("trackNumber", trackNumber);
    }
    QString ClipViewModel::name() const {
        return value("name").toString();
    }
    void ClipViewModel::setName(const QString &name) {
        insert("name", name);
    }
    bool ClipViewModel::selected() const {
        return value("selected").toBool();
    }
    void ClipViewModel::setSelected(bool selected) {
        insert("selected", selected);
    }
    bool ClipViewModel::intermediate() const {
        return value("intermediate").toBool();
    }
    void ClipViewModel::setIntermediate(bool intermediate) {
        insert("intermediate", intermediate);
    }
    bool ClipViewModel::overlapped() const {
        return value("overlapped").toBool();
    }
    void ClipViewModel::setOverlapped(bool overlapped) {
        insert("overlapped", overlapped);
    }
}