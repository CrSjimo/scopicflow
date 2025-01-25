#ifndef SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
#define SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H

#include <QObject>

#include <ScopicFlow/ScopicFlowGlobal.h>

namespace sflow {

    class SCOPIC_FLOW_EXPORT TrackListLayoutViewModel : public QObject {
        Q_OBJECT
        Q_PROPERTY(double viewportOffset READ viewportOffset WRITE setViewportOffset NOTIFY viewportOffsetChanged)
        Q_PROPERTY(bool trackHandleEnabled READ trackHandleEnabled WRITE setTrackHandleEnabled NOTIFY trackHandleEnabledChanged)
    public:
        explicit TrackListLayoutViewModel(QObject *parent = nullptr);
        ~TrackListLayoutViewModel() override;

        double viewportOffset() const;
        void setViewportOffset(double value);

        bool trackHandleEnabled() const;
        void setTrackHandleEnabled(bool value);

    signals:
        void viewportOffsetChanged(double value);
        void trackHandleEnabledChanged(bool value);

    private:
        double m_viewportOffset;
        bool m_trackHandleEnabled;

    };

}

#endif //SCOPIC_FLOW_TRACKLISTLAYOUTVIEWMODEL_H
