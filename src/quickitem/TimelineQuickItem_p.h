#ifndef SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
#define SCOPIC_FLOW_TIMELINEQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class TimeAlignmentViewModel;

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

    class TimelineQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT TimelineQuickItem : public QQuickItem {
        Q_OBJECT
        Q_PROPERTY(TimelinePalette *palette READ palette WRITE setPalette NOTIFY paletteChanged)
        Q_PROPERTY(double primaryIndicatorX READ primaryIndicatorX WRITE setPrimaryIndicatorX NOTIFY primaryIndicatorXChanged)
        Q_PROPERTY(double secondaryIndicatorX READ secondaryIndicatorX NOTIFY secondaryIndicatorXChanged)
        Q_PROPERTY(double cursorIndicatorX READ cursorIndicatorX NOTIFY cursorIndicatorXChanged)
        Q_DECLARE_PRIVATE(TimelineQuickItem)
    public:
        explicit TimelineQuickItem(QQuickItem *parent = nullptr);
        ~TimelineQuickItem() override;

        TimelinePalette *palette() const;
        void setPalette(TimelinePalette *palette);

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *timeAlignmentViewModel);

        double primaryIndicatorX() const;
        void setPrimaryIndicatorX(double primaryIndicatorX);

        double secondaryIndicatorX() const;

        double cursorIndicatorX() const;

        Q_INVOKABLE void handleContextMenuRequest(double x);

    signals:
        void paletteChanged(TimelinePalette *palette);
        void timeViewModelChanged();
        void timeAlignmentViewModelChanged();
        void primaryIndicatorXChanged(double x);
        void secondaryIndicatorXChanged(double x);
        void cursorIndicatorXChanged(double x);

        void positionIndicatorDoubleClicked();

        void contextMenuRequestedForTimeline(int tick);
        void contextMenuRequestedForPositionIndicator();

    protected:
        QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

    private:
        QScopedPointer<TimelineQuickItemPrivate> d_ptr;
    };
}

#endif //SCOPIC_FLOW_TIMELINEQUICKITEM_P_H
