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
} // sflow