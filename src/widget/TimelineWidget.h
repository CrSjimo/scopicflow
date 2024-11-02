#ifndef SCOPIC_FLOW_TIMELINEWIDGET_H
#define SCOPIC_FLOW_TIMELINEWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class TimeAlignmentViewModel;

    class TimelineQuickItem;

    class SCOPIC_FLOW_EXPORT TimelineWidget : public QWidget {
        Q_OBJECT
        Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor)
        Q_PROPERTY(QColor foregroundColor READ foregroundColor WRITE setForegroundColor)
        Q_PROPERTY(QColor positionIndicatorColor READ positionIndicatorColor WRITE setPositionIndicatorColor)
        Q_PROPERTY(QColor cursorIndicatorColor READ cursorIndicatorColor WRITE setCursorIndicatorColor)
    public:
        explicit TimelineWidget(QWidget *parent = nullptr);
        ~TimelineWidget() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel);

    signals:
        void positionIndicatorDoubleClicked();
        void contextMenuRequestedForTimeline(int tick);
        void contextMenuRequestedForPositionIndicator();

    private:
        TimelineQuickItem *d;

        QColor backgroundColor() const;
        void setBackgroundColor(const QColor &color);
        QColor foregroundColor() const;
        void setForegroundColor(const QColor &color);
        QColor positionIndicatorColor() const;
        void setPositionIndicatorColor(const QColor &color);
        QColor cursorIndicatorColor() const;
        void setCursorIndicatorColor(const QColor &color);
    };

}

#endif //SCOPIC_FLOW_TIMELINEWIDGET_H
