#ifndef SCOPIC_FLOW_PIANOROLLWIDGET_H
#define SCOPIC_FLOW_PIANOROLLWIDGET_H

#include <QWidget>

#include <ScopicFlow/ScopicFlowGlobal.h>

class QQuickItem;

namespace sflow {

    class TimeAlignmentViewModel;
    class PlaybackViewModel;
    class ClavierViewModel;
    class ScrollBehaviorViewModel;
    class AnimationViewModel;
    class PaletteViewModel;

    class PianoRollQuickItem;

    class SCOPIC_FLOW_EXPORT PianoRollWidget : public QWidget {
        Q_OBJECT

    public:
        explicit PianoRollWidget(QWidget *parent = nullptr);
        ~PianoRollWidget() override;

        TimeAlignmentViewModel *timeAlignmentViewModel() const;
        void setTimeAlignmentViewModel(TimeAlignmentViewModel *viewModel);

        PlaybackViewModel *playbackViewModel() const;
        void setPlaybackViewModel(PlaybackViewModel *viewModel);

        ClavierViewModel *clavierViewModel() const;
        void setClavierViewModel(ClavierViewModel *viewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *viewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *viewModel);

        PaletteViewModel *paletteViewModel() const;
        void setPaletteViewModel(PaletteViewModel *viewModel);

    private:
        PianoRollQuickItem *d;
    };

} // sflow

#endif //SCOPIC_FLOW_PIANOROLLWIDGET_H
