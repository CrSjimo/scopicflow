#include <QVBoxLayout>

#include <ScopicFlow/private/TrackListQuickItem_p.h>
#include <ScopicFlow/private/QuickWrapperHelper_p.h>

#include "TrackListWidget.h"

namespace sflow {
    TrackListWidget::TrackListWidget(QWidget *parent) : QWidget(parent) {
        static int _ = qmlRegisterType<TrackListQuickItem>("ScopicFlowPrivate", 1, 0, "TrackList");
        auto wrapper = QuickWrapperHelper::wrap("TrackList");
        auto layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(wrapper.first);
        setMinimumWidth(256);
        setLayout(layout);
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

        d = static_cast<TrackListQuickItem *>(wrapper.second);

        connect(d, &TrackListQuickItem::trackDoubleClicked, this, &TrackListWidget::trackDoubleClicked);
        connect(d, &TrackListQuickItem::contextMenuRequestedForTrack, this, &TrackListWidget::contextMenuRequestedForTrack);
        connect(d, &TrackListQuickItem::contextMenuRequestedForTrackDragging, this, &TrackListWidget::contextMenuRequestedForTrackDragging);
    }
    TrackListWidget::~TrackListWidget() = default;

    TrackListViewModel *TrackListWidget::trackListViewModel() const {
        return d->trackListViewModel();
    }
    void TrackListWidget::setTrackListViewModel(TrackListViewModel *trackListViewModel) {
        d->setTrackListViewModel(trackListViewModel);
    }
    ScrollBehaviorViewModel *TrackListWidget::scrollBehaviorViewModel() const {
        return d->scrollBehaviorViewModel();
    }
    void TrackListWidget::setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel) {
        d->setScrollBehaviorViewModel(scrollBehaviorViewModel);
    }
    AnimationViewModel *TrackListWidget::animationViewModel() const {
        return d->animationViewModel();
    }
    void TrackListWidget::setAnimationViewModel(AnimationViewModel *animationViewModel) {
        d->setAnimationViewModel(animationViewModel);
    }
} // sflow