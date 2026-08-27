#ifndef SCOPIC_FLOW_POINTERINTERACTIONTYPES_P_H
#define SCOPIC_FLOW_POINTERINTERACTIONTYPES_P_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QVariant>
#include <qqmlintegration.h>

namespace sflow {

    class PointerInputEvent {
        Q_GADGET
        QML_VALUE_TYPE(pointerInputEvent)
        QML_STRUCTURED_VALUE
        Q_PROPERTY(int pointerId MEMBER pointerId)
        Q_PROPERTY(DeviceType deviceType MEMBER deviceType)
        Q_PROPERTY(Button button MEMBER button)
        Q_PROPERTY(QPointF position MEMBER position)
        Q_PROPERTY(QRectF surfaceRect MEMBER surfaceRect)
        Q_PROPERTY(Qt::KeyboardModifiers modifiers MEMBER modifiers)

    public:
        enum DeviceType {
            Mouse,
            Touch,
            Pen,
        };
        Q_ENUM(DeviceType)

        enum Button {
            NoButton,
            PrimaryButton,
            SecondaryButton,
            AuxiliaryButton,
        };
        Q_ENUM(Button)

        int pointerId = 0;
        DeviceType deviceType = Mouse;
        Button button = NoButton;
        QPointF position;
        QRectF surfaceRect;
        Qt::KeyboardModifiers modifiers = Qt::NoModifier;
    };

    class PointerHit {
        Q_GADGET
        QML_VALUE_TYPE(pointerHit)
        QML_STRUCTURED_VALUE
        Q_PROPERTY(bool valid MEMBER valid)
        Q_PROPERTY(QObject *target MEMBER target)
        Q_PROPERTY(QRectF targetRect MEMBER targetRect)
        Q_PROPERTY(int hoverRegion MEMBER hoverRegion)
        Q_PROPERTY(QVariant payload MEMBER payload)

    public:
        bool valid = false;
        QObject *target = nullptr;
        QRectF targetRect;
        int hoverRegion = 0;
        QVariant payload;
    };

}

Q_DECLARE_METATYPE(sflow::PointerInputEvent)
Q_DECLARE_METATYPE(sflow::PointerHit)

#endif // SCOPIC_FLOW_POINTERINTERACTIONTYPES_P_H
