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
    QColor TrackListWidget::backgroundColor() const {
        return d->palette()->backgroundColor();
    }
    void TrackListWidget::setBackgroundColor(const QColor &color) {
        d->palette()->setBackgroundColor(color);
    }
    QColor TrackListWidget::selectedBackgroundColor() const {
        return d->palette()->selectedBackgroundColor();
    }
    void TrackListWidget::setSelectedBackgroundColor(const QColor &color) {
        d->palette()->setSelectedBackgroundColor(color);
    }
    QColor TrackListWidget::foregroundColor() const {
        return d->palette()->foregroundColor();
    }
    void TrackListWidget::setForegroundColor(const QColor &color) {
        d->palette()->setForegroundColor(color);
    }
    QColor TrackListWidget::primaryColor() const {
        return d->palette()->primaryColor();
    }
    void TrackListWidget::setPrimaryColor(const QColor &color) {
        d->palette()->setPrimaryColor(color);
    }
    QColor TrackListWidget::borderColor() const {
        return d->palette()->borderColor();
    }
    void TrackListWidget::setBorderColor(const QColor &color) {
        d->palette()->setBorderColor(color);
    }
    QColor TrackListWidget::muteColor() const {
        return d->palette()->muteColor();
    }
    void TrackListWidget::setMuteColor(const QColor &color) {
        d->palette()->setMuteColor(color);
    }
    QColor TrackListWidget::soloColor() const {
        return d->palette()->soloColor();
    }
    void TrackListWidget::setSoloColor(const QColor &color) {
        d->palette()->setSoloColor(color);
    }
    QColor TrackListWidget::recordColor() const {
        return d->palette()->recordColor();
    }
    void TrackListWidget::setRecordColor(const QColor &color) {
        d->palette()->setRecordColor(color);
    }
    QColor TrackListWidget::levelLowColor() const {
        return d->palette()->levelLowColor();
    }
    void TrackListWidget::setLevelLowColor(const QColor &color) {
        d->palette()->setLevelLowColor(color);
    }
    QColor TrackListWidget::levelMiddleColor() const {
        return d->palette()->levelMiddleColor();
    }
    void TrackListWidget::setLevelMiddleColor(const QColor &color) {
        d->palette()->setLevelMiddleColor(color);
    }
    QColor TrackListWidget::levelHighColor() const {
        return d->palette()->levelHighColor();
    }
    void TrackListWidget::setLevelHighColor(const QColor &color) {
        d->palette()->setLevelHighColor(color);
    }
    QColor TrackListWidget::levelBackgroundColor() const {
        return d->palette()->levelBackgroundColor();
    }
    void TrackListWidget::setLevelBackgroundColor(const QColor &color) {
        d->palette()->setLevelBackgroundColor(color);
    }
    QColor TrackListWidget::levelBorderColor() const {
        return d->palette()->levelBorderColor();
    }
    void TrackListWidget::setLevelBorderColor(const QColor &color) {
        d->palette()->setLevelBorderColor(color);
    }
} // sflow