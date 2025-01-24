#include "LabelViewModel.h"

#include <ScopicFlow/private/ViewModelHelper_p.h>

namespace sflow {
    LabelViewModel::LabelViewModel(QObject *parent) : QQmlPropertyMap(this, parent) {
        ViewModelHelper::initializeProperties(this);
        ViewModelHelper::connectValueChanged(this);
    }
    LabelViewModel::~LabelViewModel() = default;
    int LabelViewModel::position() const {
        return value("position").toInt();
    }
    void LabelViewModel::setPosition(int position) {
        insert("position", position);
    }
    QString LabelViewModel::content() const {
        return value("content").toString();
    }
    void LabelViewModel::setContent(const QString &content) {
        insert("content", content);
    }
    bool LabelViewModel::selected() const {
        return value("selected").toBool();
    }
    void LabelViewModel::setSelected(bool selected) {
        insert("selected", selected);
    }
    bool LabelViewModel::intermediate() const {
        return value("intermediate").toBool();
    }
    void LabelViewModel::setIntermediate(bool intermediate) {
        insert("intermediate", intermediate);
    }
}