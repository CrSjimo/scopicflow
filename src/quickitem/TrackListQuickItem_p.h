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
        Q_PROPERTY(TrackListViewModel *trackListViewModel READ trackListViewModel WRITE setTrackListViewModel NOTIFY trackListViewModelChanged)
    public:
        explicit TrackListQuickItem(QQuickItem *parent = nullptr);
        ~TrackListQuickItem() override;

        TrackListViewModel *trackListViewModel() const;
        void setTrackListViewModel(TrackListViewModel *trackListViewModel);

        Q_INVOKABLE QObject *trackAt(int index) const;
        Q_INVOKABLE void handleTrackMoved(int index, int target);

    signals:
        void trackListViewModelChanged(TrackListViewModel *trackListViewModel);
        void layoutRequired();

    private:
        QScopedPointer<TrackListQuickItemPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H
