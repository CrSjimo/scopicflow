#ifndef SCOPIC_FLOW_PIANOROLLQUICKITEM_P_H
#define SCOPIC_FLOW_PIANOROLLQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TimeAlignmentViewModel.h>
#include <ScopicFlow/ClavierViewModel.h>
#include <ScopicFlow/PlaybackViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>

namespace sflow {

    class PianoRollQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT PianoRollQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(PianoRollQuickItem)
        Q_PROPERTY(TimeAlignmentViewModel *timeAlignmentViewModel READ timeAlignmentViewModel WRITE setTimeAlignmentViewModel NOTIFY timeAlignmentViewModelChanged)
        Q_PROPERTY(PlaybackViewModel *playbackViewModel READ playbackViewModel WRITE setPlaybackViewModel NOTIFY playbackViewModelChanged)
        Q_PROPERTY(ClavierViewModel *clavierViewModel READ clavierViewModel WRITE setClavierViewModel NOTIFY clavierViewModelChanged)
        Q_PROPERTY(ScrollBehaviorViewModel *scrollBehaviorViewModel READ scrollBehaviorViewModel WRITE setScrollBehaviorViewModel NOTIFY scrollBehaviorViewModelChanged)
        Q_PROPERTY(AnimationViewModel *animationViewModel READ animationViewModel WRITE setAnimationViewModel NOTIFY animationViewModelChanged)
        Q_PROPERTY(PaletteViewModel *paletteViewModel READ paletteViewModel WRITE setPaletteViewModel NOTIFY paletteViewModelChanged)

    public:
        explicit PianoRollQuickItem(QQuickItem *parent = nullptr);
        ~PianoRollQuickItem() override;

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

    signals:
        void timeAlignmentViewModelChanged(TimeAlignmentViewModel *viewModel);
        void playbackViewModelChanged(PlaybackViewModel *viewModel);
        void clavierViewModelChanged(ClavierViewModel *viewModel);
        void scrollBehaviorViewModelChanged(ScrollBehaviorViewModel *viewModel);
        void animationViewModelChanged(AnimationViewModel *viewModel);
        void paletteViewModelChanged(PaletteViewModel *viewModel);

    private:
        QScopedPointer<PianoRollQuickItemPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_PIANOROLLQUICKITEM_P_H
