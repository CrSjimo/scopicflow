#ifndef SCOPIC_FLOW_TIMELINE_H
#define SCOPIC_FLOW_TIMELINE_H

#include <QQuickItem>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class TimeViewModel;

    class SCOPIC_FLOW_EXPORT TimelinePalette : public QObject {
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged)
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor NOTIFY foregroundColorChanged)
        Q_PROPERTY(QColor positionIndicatorColor READ positionIndicatorColor WRITE setPositionIndicatorColor NOTIFY positionIndicatorColorChanged)
        Q_PROPERTY(QColor cursorIndicatorColor READ cursorIndicatorColor WRITE setCursorIndicatorColor NOTIFY cursorIndicatorColorChanged)

    public:
        explicit TimelinePalette(QObject *parent = nullptr);
        ~TimelinePalette() override;

        QColor backgroundColor() const;
        void setBackgroundColor(const QColor &color);

        QColor foregroundColor() const;
        void setForegroundColor(const QColor &color);

        QColor positionIndicatorColor() const;
        void setPositionIndicatorColor(const QColor &color);

        QColor cursorIndicatorColor() const;
        void setCursorIndicatorColor(const QColor &color);

    signals:
        void backgroundColorChanged(const QColor &color);
        void foregroundColorChanged(const QColor &color);
        void positionIndicatorColorChanged(const QColor &color);
        void cursorIndicatorColorChanged(const QColor &color);

    private:
        QColor m_backgroundColor;
        QColor m_foregroundColor;
        QColor m_positionIndicatorColor;
        QColor m_cursorIndicatorColor;
    };

    class TimelinePrivate;

    class SCOPIC_FLOW_EXPORT Timeline : public QQuickItem {
        Q_OBJECT
        Q_PROPERTY(TimelinePalette *palette READ palette WRITE setPalette NOTIFY paletteChanged)
        Q_PROPERTY(double zeroTickX READ zeroTickX NOTIFY zeroTickXChanged)
        Q_PROPERTY(double primaryIndicatorX READ primaryIndicatorX WRITE setPrimaryIndicatorX NOTIFY
                       primaryIndicatorXChanged)
        Q_DECLARE_PRIVATE(Timeline)
    public:
        explicit Timeline(QQuickItem *parent = nullptr);
        ~Timeline() override;

        TimelinePalette *palette() const;
        void setPalette(TimelinePalette *palette);

        TimeViewModel *timeViewModel() const;
        void setTimeViewModel(TimeViewModel *timeViewModel);

        double zeroTickX() const;

        double primaryIndicatorX() const;
        void setPrimaryIndicatorX(double primaryIndicatorX);



    signals:
        void paletteChanged(TimelinePalette *palette);
        void timeViewModelChanged();
        void zeroTickXChanged(double x);
        void primaryIndicatorXChanged(double x);

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<TimelinePrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_TIMELINE_H
