#ifndef SCOPIC_FLOW_TRACKLISTWIDGET_H
#define SCOPIC_FLOW_TRACKLISTWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class TrackListViewModel;
    class ScrollBehaviorViewModel;
    class AnimationViewModel;

    class TrackListQuickItem;

    class SCOPIC_FLOW_EXPORT TrackListWidget : public QWidget {
        Q_OBJECT
    public:
        explicit TrackListWidget(QWidget *parent = nullptr);
        ~TrackListWidget() override;

        TrackListViewModel *trackListViewModel() const;
        void setTrackListViewModel(TrackListViewModel *trackListViewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *animationViewModel);

    signals:
        void trackDoubleClicked(int index);
        void contextMenuRequestedForTrack(int index);
        void contextMenuRequestedForTrackDragging(int index, int target);

    private:
        TrackListQuickItem *d;
    };

}

#endif //SCOPIC_FLOW_TRACKLISTWIDGET_H
