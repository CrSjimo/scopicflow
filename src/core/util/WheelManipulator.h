#ifndef SCOPIC_FLOW_WHEELMANIPULATOR_H
#define SCOPIC_FLOW_WHEELMANIPULATOR_H

#include <QObject>
#include <QPointF>
#include <QSizeF>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

namespace sflow {

    class WheelManipulatorPrivate;

    class SCOPIC_FLOW_CORE_EXPORT WheelManipulator : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(WheelManipulator)
    public:
        explicit WheelManipulator(QObject *parent = nullptr);
        ~WheelManipulator() override;

        Q_INVOKABLE void handleWheel(const QPointF &angleDelta, const QPointF &pixelDelta,
                                     const QPointF &center, const QSizeF &widgetSize,
                                     bool isAlternateAxis, bool isZoom, bool isPage);

    Q_SIGNALS:
        void moved(double x, double y, bool isPhysicalWheel);
        void zoomed(double ratioX, double ratioY, double x, double y, bool isPhysicalWheel);

    private:
        QScopedPointer<WheelManipulatorPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_WHEELMANIPULATOR_H
