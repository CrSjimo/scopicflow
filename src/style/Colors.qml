pragma Singleton
import QtQml
import QtQuick

QtObject {
    function autoForeground(background, foreground1 = Qt.rgba(0, 0, 0, 1), foreground2 = Qt.rgba(1,1,1,1)) {
        let toLinear = (c) => {
            return c <= 0.04045 ? c / 12.92 : Math.pow((c + 0.055) / 1.055, 2.4)
        }
        let toLum = (c) => {
            return 0.2126 * toLinear(c.r) + 0.7152 * toLinear(c.g) + 0.0722 * toLinear(c.b)
        }
        let bLum = toLum(background);
        let f1Lum = toLum(foreground1);
        let f2Lum = toLum(foreground2);

        let contrast1 = (Math.max(bLum, f1Lum) + 0.05) / (Math.min(bLum, f1Lum) + 0.05)
        let contrast2 = (Math.max(bLum, f2Lum) + 0.05) / (Math.min(bLum, f2Lum) + 0.05)
        return contrast1 > contrast2 ? foreground1 : foreground2
    }
}