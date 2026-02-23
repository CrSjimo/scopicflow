#ifndef SCOPIC_FLOW_TRACKLISTMANIPULATOR_H
#define SCOPIC_FLOW_TRACKLISTMANIPULATOR_H

#include <QObject>
#include <qqmlintegration.h>

#include <ScopicFlowCore/ScopicFlowCoreGlobal.h>

class QQuickItem;

namespace sflow {

    class TrackListLayoutViewModel;
    class ListViewModel;

    class TrackListManipulatorPrivate;

    class SCOPIC_FLOW_CORE_EXPORT TrackListManipulator : public QObject {
        Q_OBJECT
        QML_ELEMENT
        Q_DECLARE_PRIVATE(TrackListManipulator)
        Q_PROPERTY(TrackListLayoutViewModel *trackListLayoutViewModel READ trackListLayoutViewModel WRITE setTrackListLayoutViewModel NOTIFY trackListLayoutViewModelChanged)
        Q_PROPERTY(ListViewModel *trackListViewModel READ trackListViewModel WRITE setTrackListViewModel NOTIFY trackListViewModelChanged)
        Q_PROPERTY(QQuickItem *target READ target WRITE setTarget NOTIFY targetChanged)
        Q_PROPERTY(double viewSize READ viewSize WRITE setViewSize RESET resetViewSize NOTIFY viewSizeChanged)
        Q_PROPERTY(double viewportHeight READ viewportHeight NOTIFY viewportHeightChanged)
        Q_PROPERTY(double overriddenRowHeight READ overriddenRowHeight WRITE setOverriddenRowHeight RESET resetOverriddenRowHeight NOTIFY overriddenRowHeightChanged)
        Q_PROPERTY(QList<double> map READ map NOTIFY mapChanged)
    public:
        explicit TrackListManipulator(QObject *parent = nullptr);
        ~TrackListManipulator() override;

        TrackListLayoutViewModel *trackListLayoutViewModel() const;
        void setTrackListLayoutViewModel(TrackListLayoutViewModel *trackListLayoutViewModel);

        ListViewModel *trackListViewModel() const;
        void setTrackListViewModel(ListViewModel *trackListViewModel);

        QQuickItem *target() const;
        void setTarget(QQuickItem *target);

        double viewSize() const;
        void setViewSize(double viewSize);
        void resetViewSize();

        double overriddenRowHeight() const;
        void setOverriddenRowHeight(double overriddenRowHeight);
        void resetOverriddenRowHeight();

        double viewportHeight() const;

        QList<double> map() const;

        Q_INVOKABLE void moveViewBy(double deltaY, bool animated = false);
        Q_INVOKABLE void moveToStart(bool animated = false);
        Q_INVOKABLE void moveToEnd(bool animated = false);
        Q_INVOKABLE int mapToPosition(double y) const;
        Q_INVOKABLE double mapToY(int position) const;

    signals:
        void trackListLayoutViewModelChanged();
        void trackListViewModelChanged();
        void targetChanged();
        void viewSizeChanged();
        void overriddenRowHeightChanged();
        void viewportHeightChanged();
        void mapChanged();

    private:
        QScopedPointer<TrackListManipulatorPrivate> d_ptr;

    };

}

#endif //SCOPIC_FLOW_TRACKLISTMANIPULATOR_H
