#include "WheelInputArea_p.h"

#include <QGuiApplication>
#include <QWheelEvent>

namespace sflow {

    WheelInputArea::WheelInputArea(QQuickItem *parent) : QQuickItem(parent) {
    }

    void WheelInputArea::wheelEvent(QWheelEvent *event) {
        auto angleDelta = event->angleDelta();
        auto pixelDelta = event->pixelDelta();
        const auto modifiers = event->modifiers();
        const auto platform = QGuiApplication::platformName();
        const bool qtAxisSwapped = modifiers.testFlag(Qt::AltModifier) &&
            (platform == QStringLiteral("windows") || platform == QStringLiteral("xcb") || platform.startsWith(QStringLiteral("wayland")));
        // The Cocoa backend marks precise scrolling events as synthesized by the system.
        const bool cocoaAxisSwapped = platform == QStringLiteral("cocoa") && modifiers.testFlag(Qt::ShiftModifier) && event->source() == Qt::MouseEventNotSynthesized;
        if (qtAxisSwapped || cocoaAxisSwapped) {
            angleDelta = angleDelta.transposed();
            pixelDelta = pixelDelta.transposed();
        }
        event->accept();
        Q_EMIT wheel(angleDelta, pixelDelta, event->position(), modifiers.toInt());
    }

}

#include "moc_WheelInputArea_p.cpp"
