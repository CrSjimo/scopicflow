#ifndef SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H
#define SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H

#include <QQuickItem>

#include <ScopicFlow/TrackListViewModel.h>

namespace sflow {

    class TrackListQuickItemPrivate;

    class SCOPIC_FLOW_EXPORT TrackListQuickItem : public QQuickItem {
        Q_OBJECT
        Q_DECLARE_PRIVATE(TrackListQuickItem)
        Q_PROPERTY(TrackListViewModel *trackListViewModel READ trackListViewModel NOTIFY trackListViewModelChanged)
    public:
        explicit TrackListQuickItem(QQuickItem *parent = nullptr);
        ~TrackListQuickItem() override;

        TrackListViewModel *trackListViewModel() const;
        void setTrackListViewModel(TrackListViewModel *trackListViewModel);

        Q_INVOKABLE QObject *trackAt(int index) const;

    signals:
        void trackListViewModelChanged();

    private:
        QScopedPointer<TrackListQuickItemPrivate> d_ptr;

    };

} // sflow

#endif //SCOPIC_FLOW_TRACKLISTQUICKITEM_P_H
