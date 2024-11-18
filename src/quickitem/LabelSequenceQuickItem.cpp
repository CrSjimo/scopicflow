#include "LabelSequenceQuickItem_p.h"
#include "LabelSequenceQuickItem_p_p.h"

namespace sflow {

    LabelSequenceQuickItem::LabelSequenceQuickItem(QQuickItem *parent) : d_ptr(new LabelSequenceQuickItemPrivate) {
        Q_D(LabelSequenceQuickItem);
        d->q_ptr = this;
    }
    LabelSequenceQuickItem::~LabelSequenceQuickItem() = default;
    TimeAlignmentViewModel *LabelSequenceQuickItem::timeAlignmentViewModel() const {
        Q_D(const LabelSequenceQuickItem);
        return d->timeAlignmentViewModel;
    }
    void LabelSequenceQuickItem::setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (d->timeAlignmentViewModel == viewModel)
            return;
        d->timeAlignmentViewModel = viewModel;
        emit timeAlignmentViewModelChanged(viewModel);
    }
    PlaybackViewModel *LabelSequenceQuickItem::playbackViewModel() const {
        Q_D(const LabelSequenceQuickItem);
        return d->playbackViewModel;
    }
    void LabelSequenceQuickItem::setPlaybackViewModel(PlaybackViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (d->playbackViewModel == viewModel)
            return;
        d->playbackViewModel = viewModel;
        emit playbackViewModelChanged(viewModel);
    }
    ScrollBehaviorViewModel *LabelSequenceQuickItem::scrollBehaviorViewModel() const {
        Q_D(const LabelSequenceQuickItem);
        return d->scrollBehaviorViewModel;
    }
    void LabelSequenceQuickItem::setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (d->scrollBehaviorViewModel == viewModel)
            return;
        d->scrollBehaviorViewModel = viewModel;
        emit scrollBehaviorViewModelChanged(viewModel);
    }
    AnimationViewModel *LabelSequenceQuickItem::animationViewModel() const {
        Q_D(const LabelSequenceQuickItem);
        return d->animationViewModel;
    }
    void LabelSequenceQuickItem::setAnimationViewModel(AnimationViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (d->animationViewModel == viewModel)
            return;
        d->animationViewModel = viewModel;
        emit animationViewModelChanged(viewModel);
    }
    PaletteViewModel *LabelSequenceQuickItem::paletteViewModel() const {
        Q_D(const LabelSequenceQuickItem);
        return d->paletteViewModel;
    }
    void LabelSequenceQuickItem::setPaletteViewModel(PaletteViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (d->paletteViewModel == viewModel)
            return;
        d->paletteViewModel = viewModel;
        emit paletteViewModelChanged(viewModel);
    }
    LabelSequenceViewModel *LabelSequenceQuickItem::labelSequenceViewModel() const {
        Q_D(const LabelSequenceQuickItem);
        return d->labelSequenceViewModel;
    }
    void LabelSequenceQuickItem::setLabelSequenceViewModel(LabelSequenceViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (d->labelSequenceViewModel == viewModel)
            return;
        if (d->labelSequenceViewModel) {
            disconnect(d->labelSequenceViewModel, nullptr, this, nullptr);
        }
        d->labelSequenceViewModel = viewModel;
        if (viewModel) {
            connect(viewModel, &LabelSequenceViewModel::countChanged, this, &LabelSequenceQuickItem::modelChanged);
            connect(viewModel, &LabelSequenceViewModel::currentItemChanged, this, &LabelSequenceQuickItem::currentItemChanged);
        }
        emit modelChanged();
        emit currentItemChanged(viewModel ? viewModel->currentItem() : nullptr);
    }
    QList<LabelViewModel *> LabelSequenceQuickItem::model() const {
        Q_D(const LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return {};
        return d->labelSequenceViewModel->labels();
    }
    int LabelSequenceQuickItem::deselectAll() {
        Q_D(LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return 0;
        auto list = d->labelSequenceViewModel->selection();
        for (auto label : list) {
            label->setSelected(false);
        }
        return list.size();
    }
    void LabelSequenceQuickItem::extendSelection(LabelViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return;
        if (!d->labelSequenceViewModel->currentItem()) {
            viewModel->setSelected(true);
        } else {
            if (viewModel->position() > d->labelSequenceViewModel->currentItem()->position()) {
                for (auto label = viewModel; label;) {
                    label->setSelected(true);
                    if (label == d->labelSequenceViewModel->currentItem())
                        break;
                    label = d->labelSequenceViewModel->previousItem(label);
                }
            } else if (viewModel->position() <
                       d->labelSequenceViewModel->currentItem()->position()) {
                for (auto label = viewModel; label;) {
                    label->setSelected(true);
                    if (label == d->labelSequenceViewModel->currentItem())
                        break;
                    label = d->labelSequenceViewModel->nextItem(label);
                }
            } else {
                viewModel->setSelected(true);
            }
        }
    }
    void LabelSequenceQuickItemPrivate::moveSelectionTo(int position, LabelViewModel *viewModel) {
        if (position  != viewModel->position()) {
            int deltaPosition = position  - viewModel->position();
            for (auto label : labelSequenceViewModel->selection()) {
                if (label->position() + deltaPosition < 0)
                    return;
                if (label->position() + deltaPosition > timeAlignmentViewModel->end())
                    timeAlignmentViewModel->setEnd(label->position() + deltaPosition);
            }
            for (auto label : labelSequenceViewModel->selection()) {
                label->setPosition(label->position() + deltaPosition);
            }
        }
    }
    void LabelSequenceQuickItem::moveSelectedLabelsTo(double x, LabelViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (!d->timeAlignmentViewModel || !d->labelSequenceViewModel)
            return;
        auto deltaTick = x / d->timeAlignmentViewModel->pixelDensity();
        int tick = static_cast<int>(std::round(d->timeAlignmentViewModel->start() + deltaTick));
        int align = d->timeAlignmentViewModel->positionAlignment();
        int alignedTick = (tick + align / 2) / align * align;
        d->moveSelectionTo(alignedTick, viewModel);
    }
    void LabelSequenceQuickItem::moveSelectedLabelOnDragScrolling(bool isBackward, LabelViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        double x = isBackward ? 0 : width();
        auto deltaTick = x / d->timeAlignmentViewModel->pixelDensity();
        int tick = static_cast<int>(std::round(d->timeAlignmentViewModel->start() + deltaTick));
        int align = d->timeAlignmentViewModel->positionAlignment();
        int alignedTick;
        if (isBackward) {
            alignedTick = (tick + align - 1) / align * align;
        } else {
            alignedTick = tick / align * align;
        }
        d->moveSelectionTo(alignedTick, viewModel);
    }
    LabelViewModel *LabelSequenceQuickItem::nextItem(LabelViewModel *viewModel) const {
        Q_D(const LabelSequenceQuickItem);
        return d->labelSequenceViewModel->nextItem(viewModel);
    }
    LabelViewModel *LabelSequenceQuickItem::previousItem(LabelViewModel *viewModel) const {
        Q_D(const LabelSequenceQuickItem);
        return d->labelSequenceViewModel->previousItem(viewModel);
    }
    LabelViewModel *LabelSequenceQuickItem::currentItem() const {
        Q_D(const LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return nullptr;
        return d->labelSequenceViewModel->currentItem();
    }
    void LabelSequenceQuickItem::setCurrentItem(LabelViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return;
        d->labelSequenceViewModel->setCurrentItem(viewModel);
    }

}