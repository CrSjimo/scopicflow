import QtQuick

import dev.sjimo.ScopicFlow
import dev.sjimo.ScopicFlow.Internal

Text {
    required property string icon

    font.family: fluentSystemIcons.font.family
    text: FluentSystemIconCharset.get(icon)

    FontLoader {
        id: fluentSystemIcons

        source: "../assets/FluentSystemIcons-Resizable.ttf"
    }
}
