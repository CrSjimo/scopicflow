#ifndef SCOPIC_FLOW_LABELSEQUENCEWIDGET_H
#define SCOPIC_FLOW_LABELSEQUENCEWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class TimeAlignmentViewModel;
    class PlaybackViewModel;
    class ScrollBehaviorViewModel;
    class AnimationViewModel;
    class PaletteViewModel;
    class LabelSequenceViewModel;

    class LabelSequenceQuickItem;

    class SCOPIC_FLOW_EXPORT LabelSequenceWidget : public QWidget {
        Q_OBJECT
    public:
        explicit LabelSequenceWidget(QWidget *parent = nullptr);
        ~LabelSequenceWidget() override;

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

    protected:
        bool eventFilter(QObject *watched, QEvent *event) override;

    private:
        LabelSequenceQuickItem *d;
    };

} // sflow

#endif //SCOPIC_FLOW_LABELSEQUENCEWIDGET_H
