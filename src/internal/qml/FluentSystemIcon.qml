import QtQuick

import dev.sjimo.ScopicFlow.Internal

Text {
    font.family: fluentSystemIcons.font.family
    text: FluentSystemIconCharset.get(icon)

    required property string icon

    FontLoader {
        id: fluentSystemIcons
        source: "../assets/FluentSystemIcons-Resizable.ttf"
    }
}