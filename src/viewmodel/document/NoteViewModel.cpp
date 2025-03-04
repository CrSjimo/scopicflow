#include "NoteViewModel.h"
#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    NoteViewModel::NoteViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        ViewModelHelper::initializeProperties(this);
        ViewModelHelper::connectValueChanged(this);
    }
    NoteViewModel::~NoteViewModel() = default;
    int NoteViewModel::position() const {
        return value("position").toInt();
    }
    void NoteViewModel::setPosition(int position) {
        insert("position", position);
    }
    int NoteViewModel::length() const {
        return value("length").toInt();
    }
    void NoteViewModel::setLength(int length) {
        insert("length", length);
    }
    int NoteViewModel::key() const {
        return value("key").toInt();
    }
    void NoteViewModel::setKey(int key) {
        insert("key", key);
    }
    QString NoteViewModel::lyric() const {
        return value("lyric").toString();
    }
    void NoteViewModel::setLyric(const QString &lyric) {
        insert("lyric", lyric);
    }
    bool NoteViewModel::selected() const {
        return value("selected").toBool();
    }
    void NoteViewModel::setSelected(bool selected) {
        insert("selected", selected);
    }
    bool NoteViewModel::intermediate() const {
        return value("intermediate").toBool();
    }
    void NoteViewModel::setIntermediate(bool intermediate) {
        insert("intermediate", intermediate);
    }

    bool NoteViewModel::overlapped() const {
        return value("overlapped").toBool();
    }
    void NoteViewModel::setOverlapped(bool overlapped) {
        insert("overlapped", overlapped);
    }
}