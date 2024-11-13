#ifndef SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H
#define SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TrackListViewModel.h>
#include <ScopicFlow/ScrollBehaviorViewModel.h>
#include <ScopicFlow/AnimationViewModel.h>
#include <ScopicFlow/PaletteViewModel.h>

namespace sflow {

    class TrackListQuickItemPrivate;

    class TrackListQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(TrackListQuickItem)
        Q_PROPERTY(TrackListViewModel *trackListViewModel READ trackListViewModel NOTIFY trackListViewModelChanged)
        Q_PROPERTY(ScrollBehaviorViewModel *scrollBehaviorViewModel READ scrollBehaviorViewModel NOTIFY scrollBehaviorViewModelChanged)
        Q_PROPERTY(AnimationViewModel *animationViewModel READ animationViewModel NOTIFY animationViewModelChanged)
        Q_PROPERTY(PaletteViewModel *paletteViewModel READ paletteViewModel NOTIFY paletteViewModelChanged)
    public:
        explicit TrackListQuickItem(QQuickItem *parent = nullptr);
        ~TrackListQuickItem() override;

        TrackListViewModel *trackListViewModel() const;
        void setTrackListViewModel(TrackListViewModel *trackListViewModel);

        ScrollBehaviorViewModel *scrollBehaviorViewModel() const;
        void setScrollBehaviorViewModel(ScrollBehaviorViewModel *scrollBehaviorViewModel);

        AnimationViewModel *animationViewModel() const;
        void setAnimationViewModel(AnimationViewModel *animationViewModel);

        PaletteViewModel *paletteViewModel() const;
        void setPaletteViewModel(PaletteViewModel *paletteViewModel);

        Q_INVOKABLE QObject *trackAt(int index) const;
        Q_INVOKABLE double handlePositionAt(int index) const;
        Q_INVOKABLE void handleTrackMoved(int index, int target);

    signals:
        void trackListViewModelChanged(TrackListViewModel *trackListViewModel);
        void scrollBehaviorViewModelChanged(ScrollBehaviorViewModel *scrollBehaviorViewModel);
        void animationViewModelChanged(AnimationViewModel *animationViewModel);
        void paletteViewModelChanged(PaletteViewModel *paletteViewModel);
        void layoutRequired();
        void trackDoubleClicked(int index);
        void contextMenuRequestedForTrack(int index);
        void contextMenuRequestedForTrackDragging(int index, int target);

    private:
        QScopedPointer<TrackListQuickItemPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H
