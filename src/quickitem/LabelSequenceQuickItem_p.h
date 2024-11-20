#ifndef SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_H
#define SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/PlaybackViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>
#include <ScopicFlow/LabelSequenceViewModel.h>
#include <ScopicFlow/LabelViewModel.h>

namespace sflow {

    class LabelSequenceQuickItemPrivate;

    class LabelSequenceQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(LabelSequenceQuickItem)
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel WRITE setTimeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(PlaybackViewModel *playbackViewModel READ playbackViewModel WRITE setPlaybackViewModel NOTIFY playbackViewModelChanged)
        Q_PROPERTY(ScrollBehaviorViewModel *scrollBehaviorViewModel READ scrollBehaviorViewModel WRITE setScrollBehaviorViewModel NOTIFY scrollBehaviorViewModelChanged)
        Q_PROPERTY(AnimationViewModel *animationViewModel READ animationViewModel WRITE setAnimationViewModel NOTIFY animationViewModelChanged)
        Q_PROPERTY(PaletteViewModel *paletteViewModel READ paletteViewModel WRITE setPaletteViewModel NOTIFY paletteViewModelChanged)
        Q_PROPERTY(LabelSequenceViewModel *labelSequenceViewModel READ labelSequenceViewModel WRITE setLabelSequenceViewModel NOTIFY labelSequenceViewModelChanged)
        Q_PROPERTY(QList<LabelViewModel *> model READ model NOTIFY modelChanged)
        Q_PROPERTY(LabelViewModel *currentItem READ currentItem WRITE setCurrentItem NOTIFY currentItemChanged)
    public:
        explicit LabelSequenceQuickItem(QQuickItem *parent = nullptr);
        ~LabelSequenceQuickItem() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel);

        PlaybackViewModel *playbackViewModel() const;
        void setPlaybackViewModel(PlaybackViewModel *viewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *viewModel);

        PaletteViewModel *paletteViewModel() const;
        void setPaletteViewModel(PaletteViewModel *viewModel);

        LabelSequenceViewModel *labelSequenceViewModel() const;
        void setLabelSequenceViewModel(LabelSequenceViewModel *viewModel);

        QList<LabelViewModel *> model() const;

        Q_INVOKABLE int deselectAll();
        Q_INVOKABLE void extendSelection(LabelViewModel *viewModel);
        Q_INVOKABLE void moveSelectedLabelsTo(double x, LabelViewModel *viewModel);
        Q_INVOKABLE void moveSelectedLabelOnDragScrolling(bool isBackward, LabelViewModel *viewModel);
        Q_INVOKABLE LabelViewModel *nextItem(LabelViewModel *viewModel) const;
        Q_INVOKABLE LabelViewModel *previousItem(LabelViewModel *viewModel) const;
        Q_INVOKABLE LabelViewModel *insertLabelTo(double x, const QVariant &initialValue);
        Q_INVOKABLE void removeLabel(LabelViewModel *viewModel);
        Q_INVOKABLE void setSelectionIntermediate(bool intermediate);

        LabelViewModel *currentItem() const;
        void setCurrentItem(LabelViewModel *viewModel);

    signals:
        void timeAlignmentViewModelChanged(TimeAlignmentViewModel *viewModel);
        void playbackViewModelChanged(PlaybackViewModel *viewModel);
        void scrollBehaviorViewModelChanged(ScrollBehaviorViewModel *viewModel);
        void animationViewModelChanged(AnimationViewModel *viewModel);
        void paletteViewModelChanged(PaletteViewModel *viewModel);
        void labelSequenceViewModelChanged(LabelSequenceViewModel *viewModel);
        void modelChanged();
        void currentItemChanged(LabelViewModel *viewModel);

        void contextMenuRequested(int tick);
        void contextMenuRequestedForLabel(LabelViewModel *label);


    private:
        QScopedPointer<LabelSequenceQuickItemPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_LABELSEQUENCEQUICKITEM_P_H
