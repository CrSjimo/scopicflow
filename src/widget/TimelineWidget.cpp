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
    WheelModifierViewModel *TimelineWidget::wheelModifierViewModel() const {
        return d->wheelModifierViewModel();
    }
    void TimelineWidget::setWheelModifierViewModel(WheelModifierViewModel *viewModel) {
        d->setWheelModifierViewModel(viewModel);
    }
    int TimelineWidget::mapToTick(double x) const {
        return d->mapToTick(x);
    }
    double TimelineWidget::mapToX(int tick) const {
        return d->mapToX(tick);
    }

    QColor TimelineWidget::backgroundColor() const {
        return d->palette()->backgroundColor();
    }
    void TimelineWidget::setBackgroundColor(const QColor &color) {
        d->palette()->setBackgroundColor(color);
    }
    QColor TimelineWidget::foregroundColor() const {
        return d->palette()->foregroundColor();
    }
    void TimelineWidget::setForegroundColor(const QColor &color) {
        d->palette()->setForegroundColor(color);
    }
    QColor TimelineWidget::positionIndicatorColor() const {
        return d->palette()->positionIndicatorColor();
    }
    void TimelineWidget::setPositionIndicatorColor(const QColor &color) {
        d->palette()->setPositionIndicatorColor(color);
    }
    QColor TimelineWidget::cursorIndicatorColor() const {
        return d->palette()->cursorIndicatorColor();
    }
    void TimelineWidget::setCursorIndicatorColor(const QColor &color) {
        d->palette()->setCursorIndicatorColor(color);
    }
} // sflow