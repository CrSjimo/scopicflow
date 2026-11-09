#ifndef SCOPIC_FLOW_WHEELINPUTAREA_P_H
#define SCOPIC_FLOW_WHEELINPUTAREA_P_H

#include <QPointF>
#include <QQuickItem>
#include <qqmlintegration.h>

class QWheelEvent;

namespace sflow {

    class WheelInputArea : public QQuickItem {
        Q_OBJECT
        QML_ELEMENT

    public:
        explicit WheelInputArea(QQuickItem *parent = nullptr);

    Q_SIGNALS:
        void wheel(const QPointF &angleDelta, const QPointF &pixelDelta, const QPointF &position, int modifiers);

    protected:
        void wheelEvent(QWheelEvent *event) override;
    };

}

#endif // SCOPIC_FLOW_WHEELINPUTAREA_P_H
