#include "LabelSequenceQuickItem_p.h"
#include "LabelSequenceQuickItem_p_p.h"

namespace sflow {

    LabelSequenceQuickItem::LabelSequenceQuickItem(QQuickItem *parent) : d_ptr(new LabelSequenceQuickItemPrivate) {
        Q_D(LabelSequenceQuickItem);
        d->q_ptr = this;
    }
    LabelSequenceQuickItem::~LabelSequenceQuickItem() = default;
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
            connect(viewModel, &LabelSequenceViewModel::selectionChanged, this, &LabelSequenceQuickItem::selectionChanged);
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
    QList<LabelViewModel *> LabelSequenceQuickItem::selection() const {
        Q_D(const LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return {};
        return d->labelSequenceViewModel->selection();
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
    LabelViewModel *LabelSequenceQuickItem::nextItem(LabelViewModel *viewModel) const {
        Q_D(const LabelSequenceQuickItem);
        return d->labelSequenceViewModel->nextItem(viewModel);
    }
    LabelViewModel *LabelSequenceQuickItem::previousItem(LabelViewModel *viewModel) const {
        Q_D(const LabelSequenceQuickItem);
        return d->labelSequenceViewModel->previousItem(viewModel);
    }
    LabelViewModel *LabelSequenceQuickItem::insertLabelTo(int position, const QVariant &initialValue) {
        Q_D(LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return nullptr;
        auto label = new LabelViewModel;
        label->setPosition(position);
        label->setContent(initialValue);
        d->labelSequenceViewModel->insertLabels({label});
        d->labelSequenceViewModel->setCurrentItem(label);
        return label;
    }
    void LabelSequenceQuickItem::removeLabel(LabelViewModel *viewModel) {
        Q_D(LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return;
        d->labelSequenceViewModel->removeLabels({viewModel});
    }
    void LabelSequenceQuickItem::setSelectionIntermediate(bool intermediate) {
        Q_D(LabelSequenceQuickItem);
        if (!d->labelSequenceViewModel)
            return;
        for (auto label : d->labelSequenceViewModel->labels()) {
            label->setIntermediate(intermediate);
        }
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