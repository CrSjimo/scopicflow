#ifndef SCOPIC_FLOW_TIMELINEWIDGET_H
#define SCOPIC_FLOW_TIMELINEWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class TimeAlignmentViewModel;
    class PlaybackViewModel;
    class ScrollBehaviorViewModel;
    class AnimationViewModel;

    class TimelineQuickItem;

    class SCOPIC_FLOW_EXPORT TimelineWidget : public QWidget {
        Q_OBJECT
    public:
        explicit TimelineWidget(QWidget *parent = nullptr);
        ~TimelineWidget() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel);

        PlaybackViewModel *playbackViewModel() const;
        void setPlaybackViewModel(PlaybackViewModel *viewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *viewModel);

        int mapToTick(double x) const;
        double mapToX(int tick) const;

    signals:
        void positionIndicatorDoubleClicked();
        void contextMenuRequestedForTimeline(int tick);
        void contextMenuRequestedForPositionIndicator();

    private:
        TimelineQuickItem *d;
    };

}

#endif //SCOPIC_FLOW_TIMELINEWIDGET_H
