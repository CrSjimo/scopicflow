#ifndef SCOPIC_FLOW_TRACKLISTVIEWMODEL_H
#define SCOPIC_FLOW_TRACKLISTVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class TrackViewModel;

    class TrackListViewModelPrivate;

    class SCOPIC_FLOW_EXPORT TrackListViewModel : public QObject {
        Q_OBJECT
        Q_DECLARE_PRIVATE(TrackListViewModel)
        Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
        Q_PROPERTY(int count READ count NOTIFY countChanged)
        Q_PROPERTY(bool intermediate READ intermediate WRITE setIntermediate NOTIFY intermediateChanged)
        Q_PROPERTY(double viewportOffset READ viewportOffset WRITE setViewportOffset NOTIFY viewportOffsetChanged)
    public:
        explicit TrackListViewModel(QObject *parent = nullptr);
        ~TrackListViewModel() override;

        int currentIndex() const;
        void setCurrentIndex(int index);

        int count() const;

        bool intermediate() const;
        void setIntermediate(bool intermediate);

        double viewportOffset() const;
        void setViewportOffset(double offset);

        void insertTracks(int index, const QList<TrackViewModel *> &tracks);
        void removeTracks(int index, int count);
        void rotateTracks(int index, int count, int middleIndex);

        TrackViewModel *trackAt(int index) const;

    signals:
        void currentIndexChanged(int index);
        void countChanged(int count);
        void intermediateChanged(bool intermediate);
        void viewportOffsetChanged(double offset);

        void tracksInserted(int index, const QList<TrackViewModel *> &tracks);
        void tracksRemoved(int index, int count);
        void tracksRotated(int index, int count, int middleIndex);

    private:
        QScopedPointer<TrackListViewModelPrivate> d_ptr;
    };

} // sflow

#endif //SCOPIC_FLOW_TRACKLISTVIEWMODEL_H
