#include "TrackListQuickItem_p.h"
#include "TrackListQuickItem_p_p.h"

#include <ScopicFlow/TrackViewModel.h>

namespace sflow {

    namespace {
        auto _ = qmlRegisterType<TrackListQuickItem>("dev.sjimo.ScopicFlow.Private.Internal", 1, 0, "TrackList");
    }

    void TrackListQuickItemPrivate::calculateTotalHeight() {
        totalHeight = 0.0;
        handlePositionList.resize(trackListViewModel->count());
        for (int i = 0; i < trackListViewModel->count(); i++) {
            auto track = trackListViewModel->trackAt(i);
            totalHeight += track->rowHeight();
            handlePositionList[i] = totalHeight;
        }
    }
    TrackListQuickItem::TrackListQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new TrackListQuickItemPrivate) {
        Q_D(TrackListQuickItem);
        d->q_ptr = this;
    }
    TrackListQuickItem::~TrackListQuickItem() {
    }
    TrackListViewModel *TrackListQuickItem::trackListViewModel() const {
        Q_D(const TrackListQuickItem);
        return d->trackListViewModel;
    }
    void TrackListQuickItem::setTrackListViewModel(TrackListViewModel *trackListViewModel) {
        Q_D(TrackListQuickItem);
        if (d->trackListViewModel == trackListViewModel)
            return;
        if (d->trackListViewModel) {
            disconnect(d->trackListViewModel, nullptr, this, nullptr);
        }
        d->trackListViewModel = trackListViewModel;
        d->calculateTotalHeight();
        connect(d->trackListViewModel, &TrackListViewModel::tracksInserted, this, [=] {
            d->calculateTotalHeight();
            emit layoutRequired();
        });
        connect(d->trackListViewModel, &TrackListViewModel::tracksRemoved, this, [=] {
            d->calculateTotalHeight();
            emit layoutRequired();
        });
        connect(d->trackListViewModel, &TrackListViewModel::tracksRotated, this, &TrackListQuickItem::layoutRequired);
        emit trackListViewModelChanged(trackListViewModel);
    }
    ScrollBehaviorViewModel *TrackListQuickItem::scrollBehaviorViewModel() const {
        Q_D(const TrackListQuickItem);
        return d->scrollBehaviorViewModel;
    }
    void TrackListQuickItem::setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel) {
        Q_D(TrackListQuickItem);
        if (d->scrollBehaviorViewModel == scrollBehaviorViewModel)
            return;
        d->scrollBehaviorViewModel = scrollBehaviorViewModel;
        emit scrollBehaviorViewModelChanged(scrollBehaviorViewModel);
    }
    AnimationViewModel *TrackListQuickItem::animationViewModel() const {
        Q_D(const TrackListQuickItem);
        return d->animationViewModel;
    }
    void TrackListQuickItem::setAnimationViewModel(AnimationViewModel *animationViewModel) {
        Q_D(TrackListQuickItem);
        if (d->animationViewModel == animationViewModel)
            return;
        d->animationViewModel = animationViewModel;
        emit animationViewModelChanged(animationViewModel);
    }
    PaletteViewModel *TrackListQuickItem::paletteViewModel() const {
        Q_D(const TrackListQuickItem);
        return d->paletteViewModel;
    }
    void TrackListQuickItem::setPaletteViewModel(PaletteViewModel *paletteViewModel) {
        Q_D(TrackListQuickItem);
        if (d->paletteViewModel == paletteViewModel)
            return;
        d->paletteViewModel = paletteViewModel;
        emit paletteViewModelChanged(paletteViewModel);
    }

    QObject *TrackListQuickItem::trackAt(int index) const {
        Q_D(const TrackListQuickItem);
        if (!d->trackListViewModel)
            return nullptr;
        return d->trackListViewModel->trackAt(index);
    }
    double TrackListQuickItem::handlePositionAt(int index) const {
        Q_D(const TrackListQuickItem);
        return d->handlePositionList[index];
    }
    void TrackListQuickItem::handleTrackMoved(int index, int target) {
        Q_D(TrackListQuickItem);
        if (!d->trackListViewModel)
            return;
        auto currentItem = d->trackListViewModel->trackAt(d->trackListViewModel->currentIndex());
        if (!d->trackListViewModel->trackAt(index)->selected()) {
            if (target > index) {
                d->trackListViewModel->rotateTracks(index, target - index, index + 1);
            } else {
                d->trackListViewModel->rotateTracks(target, index + 1 - target, index);
            }
        } else {
            d->trackListViewModel->setIntermediate(true);
            int continuousSelectionStart = -1;
            int nextTarget = target;
            int i;
            for (i = target; i < d->trackListViewModel->count(); i++) {
                if (d->trackListViewModel->trackAt(i)->selected() && continuousSelectionStart == -1) {
                    continuousSelectionStart = i;
                } else if (!d->trackListViewModel->trackAt(i)->selected()) {
                    if (continuousSelectionStart != -1) {
                        // if (target > i) {
                        //     d->trackListViewModel->rotateTracks(continuousSelectionStart, target - continuousSelectionStart, i);
                        // }
                        d->trackListViewModel->rotateTracks(nextTarget, i - nextTarget, continuousSelectionStart);
                        nextTarget = nextTarget + i - continuousSelectionStart;
                        continuousSelectionStart = -1;
                    }
                }
            }
            if (continuousSelectionStart != -1) {
                d->trackListViewModel->rotateTracks(nextTarget, i - nextTarget, continuousSelectionStart);
            }
            continuousSelectionStart = -1;
            nextTarget = target;
            for (i = target - 1; i >= 0; i--) {
                if (d->trackListViewModel->trackAt(i)->selected() && continuousSelectionStart == -1) {
                    continuousSelectionStart = i;
                } else if (!d->trackListViewModel->trackAt(i)->selected()) {
                    if (continuousSelectionStart != -1) {
                        d->trackListViewModel->rotateTracks(i + 1, nextTarget - i - 1, continuousSelectionStart + 1);
                        nextTarget = nextTarget + i - continuousSelectionStart;
                        continuousSelectionStart = -1;
                    }
                }
            }
            if (continuousSelectionStart != -1) {
                d->trackListViewModel->rotateTracks(i + 1, nextTarget - i - 1, continuousSelectionStart + 1);
            }
            d->trackListViewModel->setIntermediate(false);
        }
        for (int i = 0; i < trackListViewModel()->count(); i++) {
            if (trackListViewModel()->trackAt(i) == currentItem) {
                trackListViewModel()->setCurrentIndex(i);
                break;
            }
        }
    }
} // sflow