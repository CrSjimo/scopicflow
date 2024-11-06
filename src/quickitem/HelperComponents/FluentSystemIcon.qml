import QtQuick
import './FluentSystemIconCharset.mjs' as Charset

Text {
    required property string icon
    FontLoader { id: fluentSystemIcons; source: "qrc:/ScopicFlow/assets/FluentSystemIcons-Resizable.ttf" }
    font.family: fluentSystemIcons.font.family
    text: Charset.get(icon)
}