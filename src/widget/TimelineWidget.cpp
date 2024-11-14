#include "TimelineWidget.h"

#include <QVBoxLayout>

#include <ScopicFlow/private/TimelineQuickItem_p.h>

#include "QuickWrapperHelper_p.h"

namespace sflow {
    TimelineWidget::TimelineWidget(QWidget *parent) : QWidget(parent) {
        static int _ = qmlRegisterType<TimelineQuickItem>("ScopicFlowPrivate", 1, 0, "Timeline");
        auto wrapper = QuickWrapperHelper::wrap("Timeline");
        auto layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(wrapper.first);
        setMinimumHeight(20);
        setLayout(layout);

        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        d = static_cast<TimelineQuickItem *>(wrapper.second);
        connect(d, &TimelineQuickItem::positionIndicatorDoubleClicked, this, &TimelineWidget::positionIndicatorDoubleClicked);
        connect(d, &TimelineQuickItem::contextMenuRequestedForTimeline, this, &TimelineWidget::contextMenuRequestedForTimeline);
        connect(d, &TimelineQuickItem::contextMenuRequestedForPositionIndicator, this, &TimelineWidget::contextMenuRequestedForPositionIndicator);
    }
    TimelineWidget::~TimelineWidget() = default;

    TimeAlignmentViewModel *TimelineWidget::timeAlignmentViewModel() const {
        return d->timeAlignmentViewModel();
    }
    void TimelineWidget::setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel) {
        d->setTimeAlignmentViewModel(viewModel);
    }
    PlaybackViewModel *TimelineWidget::playbackViewModel() const {
        return d->playbackViewModel();
    }
    void TimelineWidget::setPlaybackViewModel(PlaybackViewModel *viewModel) {
        d->setPlaybackViewModel(viewModel);
    }
    ScrollBehaviorViewModel *TimelineWidget::scrollBehaviorViewModel() const {
        return d->scrollBehaviorViewModel();
    }
    void TimelineWidget::setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel) {
        d->setScrollBehaviorViewModel(viewModel);
    }
    AnimationViewModel *TimelineWidget::animationViewModel() const {
        return d->animationViewModel();
    }
    void TimelineWidget::setAnimationViewModel(AnimationViewModel *viewModel) {
        d->setAnimationViewModel(viewModel);
    }
    PaletteViewModel *TimelineWidget::paletteViewModel() const {
        return d->paletteViewModel();
    }
    void TimelineWidget::setPaletteViewModel(PaletteViewModel *viewModel) {
        d->setPaletteViewModel(viewModel);
    }
    int TimelineWidget::mapToTick(double x) const {
        return d->mapToTick(x);
    }
    double TimelineWidget::mapToX(int tick) const {
        return d->mapToX(tick);
    }
} // sflow