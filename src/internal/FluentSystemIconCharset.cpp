#include "FluentSystemIconCharset_p.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

namespace sflow {

    static QJsonObject data;

    FluentSystemIconCharset::FluentSystemIconCharset(QObject *parent) : QObject(parent) {
        if (data.isEmpty()) {
            QFile f(":/qt/qml/dev/sjimo/ScopicFlow/Internal/assets/FluentSystemIcons-Resizable.json");
            f.open(QIODevice::ReadOnly);
            data = QJsonDocument::fromJson(f.readAll()).object();
        }
    }
    FluentSystemIconCharset::~FluentSystemIconCharset() = default;
    QString FluentSystemIconCharset::get(const QString &name) const {
        auto charCode = static_cast<char32_t>(data.value("ic_fluent_" + name).toInt());
        if (!charCode)
            return "";
        return QString::fromUcs4(&charCode, 1);
    }
} // sflow
