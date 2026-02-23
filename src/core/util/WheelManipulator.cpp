#include "WheelManipulator.h"
#include "WheelManipulator_p.h"

#include <QtMath>

namespace sflow {

    WheelManipulator::WheelManipulator(QObject *parent) : QObject(parent), d_ptr(new WheelManipulatorPrivate) {
        Q_D(WheelManipulator);
        d->q_ptr = this;
    }

    WheelManipulator::~WheelManipulator() = default;

    void WheelManipulator::handleWheel(const QPointF &angleDelta, const QPointF &pixelDelta,
                                        const QPointF &center, const QSizeF &widgetSize,
                                        bool isAlternateAxis, bool isZoom, bool isPage) {
        double deltaPixelX = isAlternateAxis ? pixelDelta.y() : pixelDelta.x();
        double deltaPixelY = isAlternateAxis ? pixelDelta.x() : pixelDelta.y();

        double deltaX = (isAlternateAxis ? angleDelta.y() : angleDelta.x()) / 120.0;
        double deltaY = (isAlternateAxis ? angleDelta.x() : angleDelta.y()) / 120.0;

        // Determine if this is a physical wheel (detent-based)
        bool wheelHint = (!qFuzzyIsNull(deltaPixelX) || qFuzzyCompare(deltaX, qFloor(deltaX))) &&
                         (!qFuzzyIsNull(deltaPixelY) || qFuzzyCompare(deltaY, qFloor(deltaY)));

        if (isZoom) {
            double ratioX = qPow(1.0 + (isPage ? 2.5 : 0.25) * qAbs(deltaX), qreal(qFuzzyIsNull(deltaX) ? 0 : (deltaX > 0 ? 1 : -1)));
            double ratioY = qPow(1.0 + (isPage ? 3.0 : 0.3) * qAbs(deltaY), qreal(qFuzzyIsNull(deltaY) ? 0 : (deltaY > 0 ? 1 : -1)));
            emit zoomed(ratioX, ratioY, center.x(), center.y(), wheelHint);
        } else {
            if (qFuzzyIsNull(deltaPixelX)) {
                deltaPixelX = isPage ? (qFuzzyIsNull(deltaX) ? 0 : (deltaX > 0 ? 1 : -1)) * widgetSize.width()
                                     : 0.125 * deltaX * widgetSize.width();
            }
            if (qFuzzyIsNull(deltaPixelY)) {
                deltaPixelY = isPage ? (qFuzzyIsNull(deltaY) ? 0 : (deltaY > 0 ? 1 : -1)) * widgetSize.height()
                                     : 0.2 * deltaY * widgetSize.height();
            }
            emit moved(-deltaPixelX, -deltaPixelY, wheelHint);
        }
    }

}
