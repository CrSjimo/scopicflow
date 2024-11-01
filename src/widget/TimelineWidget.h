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
    public:
        explicit TimelineWidget(QWidget *parent = nullptr);
        ~TimelineWidget() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel);

    signals:
        void positionIndicatorDoubleClicked();

    private:
        TimelineQuickItem *d;
    };

}

#endif //SCOPIC_FLOW_TIMELINEWIDGET_H
