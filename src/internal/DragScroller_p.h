#ifndef SCOPIC_FLOW_DRAGSCROLLER_P_H
#define SCOPIC_FLOW_DRAGSCROLLER_P_H

#include <QJSValue>
#include <QObject>
#include <QScopedPointer>
#include <qqmlintegration.h>

class QTimer;

namespace sflow {

    class DragScrollerPrivate;

    class DragScroller : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(DragScroller)
        Q_PROPERTY(double distanceX READ distanceX WRITE setDistanceX NOTIFY distanceXChanged)
        Q_PROPERTY(double distanceY READ distanceY WRITE setDistanceY NOTIFY distanceYChanged)
        Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
        Q_PROPERTY(QTimer *tickingTimer READ tickingTimer CONSTANT)

    public:
        explicit DragScroller(QObject *parent = nullptr);
        ~DragScroller() override;

        double distanceX() const;
        void setDistanceX(double distanceX);

        double distanceY() const;
        void setDistanceY(double distanceY);

        bool isRunning() const;
        void setRunning(bool running);

        QTimer *tickingTimer() const;

        void determine(double x, double width, double y, double height, const std::function<void(bool, bool)> &callback);
        Q_INVOKABLE void determine(double x, double width, double y, double height, const QJSValue &callback);

    Q_SIGNALS:
        void distanceXChanged();
        void distanceYChanged();
        void runningChanged();

        void moved(double deltaX, double deltaY);

    private:
        QScopedPointer<DragScrollerPrivate> d_ptr;
    };

}

#endif // SCOPIC_FLOW_DRAGSCROLLER_P_H
