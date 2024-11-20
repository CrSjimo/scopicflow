#include "PianoRollQuickItem_p.h"
#include "PianoRollQuickItem_p_p.h"

namespace sflow {

    namespace {
        auto _ = qmlRegisterType<PianoRollQuickItem>("dev.sjimo.ScopicFlow.Private.Internal", 1, 0, "PianoRoll");
    }

    PianoRollQuickItem::PianoRollQuickItem(QQuickItem *parent) : QQuickItem(parent), d_ptr(new PianoRollQuickItemPrivate) {
        Q_D(PianoRollQuickItem);
        d->q_ptr = this;
    }
    PianoRollQuickItem::~PianoRollQuickItem() = default;

    TimeAlignmentViewModel *PianoRollQuickItem::timeAlignmentViewModel() const {
        Q_D(const PianoRollQuickItem);
        return d->timeAlignmentViewModel;
    }
    void PianoRollQuickItem::setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel) {
        Q_D(PianoRollQuickItem);
        if (d->timeAlignmentViewModel != viewModel) {
            d->timeAlignmentViewModel = viewModel;
            emit timeAlignmentViewModelChanged(viewModel);
        }
    }
    PlaybackViewModel *PianoRollQuickItem::playbackViewModel() const {
        Q_D(const PianoRollQuickItem);
        return d->playbackViewModel;
    }
    void PianoRollQuickItem::setPlaybackViewModel(PlaybackViewModel *viewModel) {
        Q_D(PianoRollQuickItem);
        if (d->playbackViewModel != viewModel) {
            d->playbackViewModel = viewModel;
            emit playbackViewModelChanged(viewModel);
        }
    }
    ClavierViewModel *PianoRollQuickItem::clavierViewModel() const {
        Q_D(const PianoRollQuickItem);
        return d->clavierViewModel;
    }
    void PianoRollQuickItem::setClavierViewModel(ClavierViewModel *viewModel) {
        Q_D(PianoRollQuickItem);
        if (d->clavierViewModel != viewModel) {
            d->clavierViewModel = viewModel;
            emit clavierViewModelChanged(viewModel);
        }
    }
    ScrollBehaviorViewModel *PianoRollQuickItem::scrollBehaviorViewModel() const {
        Q_D(const PianoRollQuickItem);
        return d->scrollBehaviorViewModel;
    }
    void PianoRollQuickItem::setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel) {
        Q_D(PianoRollQuickItem);
        if (d->scrollBehaviorViewModel != viewModel) {
            d->scrollBehaviorViewModel = viewModel;
            emit scrollBehaviorViewModelChanged(viewModel);
        }
    }
    AnimationViewModel *PianoRollQuickItem::animationViewModel() const {
        Q_D(const PianoRollQuickItem);
        return d->animationViewModel;
    }
    void PianoRollQuickItem::setAnimationViewModel(AnimationViewModel *viewModel) {
        Q_D(PianoRollQuickItem);
        if (d->animationViewModel != viewModel) {
            d->animationViewModel = viewModel;
            emit animationViewModelChanged(viewModel);
        }
    }
    PaletteViewModel *PianoRollQuickItem::paletteViewModel() const {
        Q_D(const PianoRollQuickItem);
        return d->paletteViewModel;
    }
    void PianoRollQuickItem::setPaletteViewModel(PaletteViewModel *viewModel) {
        Q_D(PianoRollQuickItem);
        if (d->paletteViewModel != viewModel) {
            d->paletteViewModel = viewModel;
            emit paletteViewModelChanged(viewModel);
        }
    }
    
}