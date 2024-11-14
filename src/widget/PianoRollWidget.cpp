#include "PianoRollWidget.h"

#include <QVBoxLayout>

#include <ScopicFlow/private/PianoRollQuickItem_p.h>
#include <ScopicFlow/private/PianoRollBackgroundQuickItem_p.h>
#include <ScopicFlow/private/QuickWrapperHelper_p.h>

namespace sflow {
    PianoRollWidget::PianoRollWidget(QWidget *parent) : QWidget(parent) {
        static int _1 = qmlRegisterType<PianoRollQuickItem>("ScopicFlowPrivate", 1, 0, "PianoRoll");
        static int _2 = qmlRegisterType<PianoRollBackgroundQuickItem>("ScopicFlowPrivate", 1, 0, "PianoRollBackground");
        auto wrapper = QuickWrapperHelper::wrap("PianoRoll");
        auto layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(wrapper.first);
        setLayout(layout);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        d = static_cast<PianoRollQuickItem *>(wrapper.second);
    }
    PianoRollWidget::~PianoRollWidget() = default;

    TimeAlignmentViewModel *PianoRollWidget::timeAlignmentViewModel() const {
        return d->timeAlignmentViewModel();
    }
    void PianoRollWidget::setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel) {
        d->setTimeAlignmentViewModel(viewModel);
    }
    PlaybackViewModel *PianoRollWidget::playbackViewModel() const {
        return d->playbackViewModel();
    }
    void PianoRollWidget::setPlaybackViewModel(PlaybackViewModel *viewModel) {
        d->setPlaybackViewModel(viewModel);
    }
    ClavierViewModel *PianoRollWidget::clavierViewModel() const {
        return d->clavierViewModel();
    }
    void PianoRollWidget::setClavierViewModel(ClavierViewModel *viewModel) {
        d->setClavierViewModel(viewModel);
    }
    ScrollBehaviorViewModel *PianoRollWidget::scrollBehaviorViewModel() const {
        return d->scrollBehaviorViewModel();
    }
    void PianoRollWidget::setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel) {
        d->setScrollBehaviorViewModel(viewModel);
    }
    AnimationViewModel *PianoRollWidget::animationViewModel() const {
        return d->animationViewModel();
    }
    void PianoRollWidget::setAnimationViewModel(AnimationViewModel *viewModel) {
        d->setAnimationViewModel(viewModel);
    }
    PaletteViewModel *PianoRollWidget::paletteViewModel() const {
        return d->paletteViewModel();
    }
    void PianoRollWidget::setPaletteViewModel(PaletteViewModel *viewModel) {
        d->setPaletteViewModel(viewModel);
    }
} // sflow