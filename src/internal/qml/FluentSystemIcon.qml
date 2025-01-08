import QtQuick
import './FluentSystemIconCharset.mjs' as Charset

Text {
    font.family: fluentSystemIcons.font.family
    text: Charset.get(icon)

    required property string icon

    FontLoader {
        id: fluentSystemIcons
        source: "../assets/FluentSystemIcons-Resizable.ttf"
    }
}