#include "LabelSequenceWidget.h"

#include <QVBoxLayout>

#include <ScopicFlow/private/LabelSequenceQuickItem_p.h>
#include <ScopicFlow/private/QuickWrapperHelper_p.h>

namespace sflow {
    LabelSequenceWidget::LabelSequenceWidget(QWidget *parent) : QWidget(parent) {
        static int _ = qmlRegisterType<LabelSequenceQuickItem>("ScopicFlowPrivate", 1, 0, "LabelSequence");
        auto wrapper = QuickWrapperHelper::wrap("LabelSequence");
        auto layout = new QVBoxLayout;
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(wrapper.first);
        setMinimumHeight(16);
        setLayout(layout);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);

        d = static_cast<LabelSequenceQuickItem *>(wrapper.second);
    }
    LabelSequenceWidget::~LabelSequenceWidget() = default;

    TimeAlignmentViewModel *LabelSequenceWidget::timeAlignmentViewModel() const {
        return d->timeAlignmentViewModel();
    }
    void LabelSequenceWidget::setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel) {
        d->setTimeAlignmentViewModel(viewModel);
    }
    PlaybackViewModel *LabelSequenceWidget::playbackViewModel() const {
        return d->playbackViewModel();
    }
    void LabelSequenceWidget::setPlaybackViewModel(PlaybackViewModel *viewModel) {
        d->setPlaybackViewModel(viewModel);
    }
    ScrollBehaviorViewModel *LabelSequenceWidget::scrollBehaviorViewModel() const {
        return d->scrollBehaviorViewModel();
    }
    void LabelSequenceWidget::setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel) {
        d->setScrollBehaviorViewModel(viewModel);
    }
    AnimationViewModel *LabelSequenceWidget::animationViewModel() const {
        return d->animationViewModel();
    }
    void LabelSequenceWidget::setAnimationViewModel(AnimationViewModel *viewModel) {
        d->setAnimationViewModel(viewModel);
    }
    PaletteViewModel *LabelSequenceWidget::paletteViewModel() const {
        return d->paletteViewModel();
    }
    void LabelSequenceWidget::setPaletteViewModel(PaletteViewModel *viewModel) {
        d->setPaletteViewModel(viewModel);
    }
    LabelSequenceViewModel *LabelSequenceWidget::labelSequenceViewModel() const {
        return d->labelSequenceViewModel();
    }
    void LabelSequenceWidget::setLabelSequenceViewModel(LabelSequenceViewModel *viewModel) {
        d->setLabelSequenceViewModel(viewModel);
    }
} // sflow